/***************************************************

Copyright (c) 1999  kashiwano masahiro

Abstract:

    UUSBD.SYS Main module(driver entry function etc)
    
****************************************************/

#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "uusbd.h"
#include "uusbdGUID.h"

PRKMUTEX mutex_pocall;

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/***

Routine Description:

    WDMドライバーのエントリーポイント。
	I/Oシステムによって呼び出される。

Arguments:

    DriverObject - このドライバーの driver objectへのポインター

    RegistryPath - ドライバーが使用するレジストリーパス（unicode)

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

***/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT deviceObject = NULL;
    BOOLEAN fRes;

#if DBG
	// should be done before any debug output is done.
    // read our debug verbosity level from the registry
    Uusbd_GetRegistryDword( UUSBD_REGISTRY_PARAMETERS_PATH, //absolute registry path
                                     L"DebugLevel",     // REG_DWORD ValueName
                                     &gDebugLevel );    // Value receiver
#endif

    UUSBD_KdPrint( DBGLVL_MINIMUM ,("Entering DriverEntry(), RegistryPath=\n    %ws\n", RegistryPath->Buffer ));

    //
    // Create dispatch points for create, close, unload
    DriverObject->MajorFunction[IRP_MJ_CREATE] = Uusbd_Create;	// uusbdmain.c
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = Uusbd_Close;	// uusbdmain.c
    DriverObject->DriverUnload = Uusbd_Unload;					// uusbdmain.c

    // User mode DeviceIoControl() calls will be routed here
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Uusbd_ProcessIOCTL;// in usbdioctl.c

    // User mode ReadFile()/WriteFile() calls will be routed here
    DriverObject->MajorFunction[IRP_MJ_WRITE] = Uusbd_Write; // usbdcrw.c
    DriverObject->MajorFunction[IRP_MJ_READ] = Uusbd_Read;   // usbdcrw.c

    // routines for handling system PNP and power management requests
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = Uusbd_ProcessSysControlIrp; // uusbdmain.c 　
    DriverObject->MajorFunction[IRP_MJ_PNP] = Uusbd_ProcessPnPIrp; // uusbdpnp.c
    DriverObject->MajorFunction[IRP_MJ_POWER] = Uusbd_ProcessPowerIrp; // uusbdpwr.c

    // The Functional Device Object (FDO) will not be created for PNP devices until 
    // this routine is called upon device plug-in.
    DriverObject->DriverExtension->AddDevice = Uusbd_PnPAddDevice; // uusbdpnp.c

	/*
	Windows98では、このドライバーの下層のドライバーにPoCallDriverを
	呼びそれが完了していない時に、PoCallDriveを呼ぶと初めのIRPが完了しない
	これを防ぐため、mutexを使いシリアライズする。
	*/
	mutex_pocall = UUSBD_ExAllocatePool(NonPagedPool, sizeof(*mutex_pocall));
	UUSBD_KdPrint( DBGLVL_DEFAULT,("Mutex address=%08x\n", mutex_pocall));
	KeInitializeMutex(mutex_pocall, 1);

    UUSBD_KdPrint( DBGLVL_DEFAULT,("exiting DriverEntry (%x)\n", ntStatus));

    return ntStatus;
}





NTSTATUS
Uusbd_ProcessSysControlIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/***

Routine Description:

    IRP_MJ_SYSTEM_CONTROLのディスパッチルーチン
	特別にする処理はなく、ただ PDO に処理を伝えるのみ。

Arguments:

    DeviceObject - pointer to FDO device object

    Irp          - pointer to an I/O Request Packet

Return Value:

	PDOからのReturn Valueをそのまま返す


***/
{

    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    NTSTATUS waitStatus;
    PDEVICE_OBJECT stackDeviceObject;

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    //
    // Get a pointer to the current location in the Irp. This is where
    //     the function codes and parameters are located.
    //

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    //
    // Get a pointer to the device extension
    //

    deviceExtension = DeviceObject->DeviceExtension;
    // ドライバーロード時に一番上だったデバイスオブジェクト
	//（自分の下の層のデバイスオブジェクト)を得る
    stackDeviceObject = deviceExtension->TopOfStackDeviceObject;

    UUSBD_KdPrint( DBGLVL_HIGH, ( "enter Uusbd_ProcessSysControlIrp()\n") );

    Uusbd_IncrementIoCount(DeviceObject);

    UUSBD_ASSERT( IRP_MJ_SYSTEM_CONTROL == irpStack->MajorFunction );

    IoCopyCurrentIrpStackLocationToNext(Irp);

	// 下の層に IRPを発行
    ntStatus = IoCallDriver(stackDeviceObject,
                            Irp);

    Uusbd_DecrementIoCount(DeviceObject);

    UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_ProcessSysControlIrp() Exit Uusbd_ProcessSysControlIrp %x\n", ntStatus));

    return ntStatus;
}


VOID
Uusbd_Unload(
    IN PDRIVER_OBJECT DriverObject
    )
/***

Routine Description:

    DriverEntryでアロケートしたリソースを全て解放する

Arguments:

    DriverObject - pointer to a driver object

Return Value:


***/
{
    UUSBD_KdPrint( DBGLVL_HIGH,("enter Uusbd_Unload\n"));

    //
    // Free any global resources allocated
    // in DriverEntry.
	// We have few or none because for a PNP device, almost all
	// allocation is done in PnpAddDevice() and all freeing 
	// while handling IRP_MN_REMOVE_DEVICE:
    //
    UUSBD_ExFreePool(mutex_pocall);
	UUSBD_ASSERT( gExAllocCount == 0 );

    UUSBD_KdPrint( DBGLVL_DEFAULT,("exit Uusbd_Unload\n"));

}


NTSTATUS
Uusbd_SymbolicLink(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PUNICODE_STRING deviceLinkUnicodeString

    )
/***

Routine Description:

  GUIDからシンボリックリンク名を得てリンクを作成する
  ユーザーモードからopen,createする時にこの名前を使用する

Arguments:

    DeviceObject
		pointer to our Physical Device Object ( PDO )

    deviceLinkUnicodeString
		unicode文字列を格納する場所。呼び出し元がアロケートしておく。
		成功すれば、このデバイスインターフェースオブジェクトのシンボ
		リックリンク名が格納される

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

***/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;


    //  Create the symbolic link
     
    // device interfaceを登録し、シンボリックリンク名を得る
	// このリンク名はユーザーモードアプリからSetupDi APIを使って調べることができる

    ntStatus = IoRegisterDeviceInterface(
                DeviceObject,
                (LPGUID)&GUID_CLASS_UUSBD,
                NULL,
                deviceLinkUnicodeString);

    UUSBD_KdPrintCond( DBGLVL_MEDIUM, (!(NT_SUCCESS(ntStatus))),
            ("FAILED to IoRegisterDeviceInterface()\n"));

   if (NT_SUCCESS(ntStatus)) {

		// デバイスインターフェースを有効にする
        ntStatus = IoSetDeviceInterfaceState(deviceLinkUnicodeString, TRUE);

        UUSBD_KdPrintCond( DBGLVL_MEDIUM,
                (!(NT_SUCCESS(ntStatus))),
                ("FAILED to IoSetDeviceInterfaceState()\n"));

        UUSBD_KdPrintCond( DBGLVL_MEDIUM,
                ((NT_SUCCESS(ntStatus))),
                ("SUCCEEDED  IoSetDeviceInterfaceState()\n"));

    }

    return ntStatus;
}



NTSTATUS
Uusbd_CreateDeviceObject(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PDEVICE_OBJECT *DeviceObject
    )
/***

Routine Description:

	Functional DeviceObjectを作る

Arguments:

    DriverObject
		デバイスのドライバーオブジェクトへのポインター

	PhysicalDeviceObject
		作成するデバイスオブジェクト用のPDO

    DeviceObject
		作成したデバイスオブジェクトを返すポインター

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

***/
{
    NTSTATUS ntStatus;
    UNICODE_STRING deviceLinkUnicodeString;
    PDEVICE_EXTENSION deviceExtension;
    USHORT i;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("enter Uusbd_CreateDeviceObject() \n"));

	// シンボリックリンク名を得る
    ntStatus = Uusbd_SymbolicLink( PhysicalDeviceObject, &deviceLinkUnicodeString );

    UUSBD_KdPrintCond( DBGLVL_DEFAULT,
            (NT_SUCCESS(ntStatus)),
            ("Uusbd_CreateDeviceObject() SUCCESS Create GUID_CLASS_Uusbd_BULK-based Device name\n   %ws\n Length decimal %d, MaximumLength decimal %d\n",
            deviceLinkUnicodeString.Buffer,
            deviceLinkUnicodeString.Length,
            deviceLinkUnicodeString.MaximumLength));

    UUSBD_KdPrintCond( DBGLVL_DEFAULT,
            (!(NT_SUCCESS(ntStatus))),
            ("Uusbd_CreateDeviceObject() FAILED to Create GUID_CLASS_Uusbd_BULK-based Device name\n"));

    if (NT_SUCCESS(ntStatus)) {

        ntStatus = IoCreateDevice (DriverObject,		// ドライバーオブジェクト
                           sizeof (DEVICE_EXTENSION),	// device extensionの大きさ
                           NULL,						// device objectの名前
                           FILE_DEVICE_UNKNOWN,			// デバイスタイプ
                           FILE_AUTOGENERATED_DEVICE_NAME, //デバイスの特性
                           FALSE,						// 排他フラグ
                           DeviceObject);				// 作成したデバイスオブジェクト

        if (NT_SUCCESS(ntStatus))  {
            deviceExtension = (PDEVICE_EXTENSION) ((*DeviceObject)->DeviceExtension);
			memset(deviceExtension, 0, sizeof (DEVICE_EXTENSION));	
        }

        UUSBD_KdPrintCond( DBGLVL_DEFAULT,
                (!(NT_SUCCESS(ntStatus))),
                ("Uusbd_CreateDeviceObject() IoCreateDevice() FAILED\n"));

 
        if (!NT_SUCCESS(ntStatus))  {
             return ntStatus;
        }

        // シンボリックリンク名をdeviceExtensionにコピーしておく
        // Uusbd_RemoveDeviceでインタフェースをdisableするときに使用
        RtlCopyMemory(deviceExtension->DeviceLinkNameBuffer,
                      deviceLinkUnicodeString.Buffer,
                      deviceLinkUnicodeString.Length);


 		// イベントの初期化
		// this event is triggered when there is no pending io of any kind and device is removed
        KeInitializeEvent(&deviceExtension->RemoveEvent, NotificationEvent, FALSE);

        // this event is triggered when self-requested power irps complete
        KeInitializeEvent(&deviceExtension->SelfRequestedPowerIrpEvent, NotificationEvent, FALSE);

        // this event is triggered when when Uusbd_AsyncReadWrite_Complete() finishes or cancels last staged io irp
//        KeInitializeEvent(&deviceExtension->StagingDoneEvent, NotificationEvent, FALSE);

        // this event is triggered when there is no pending io  (pending io count == 1 )
        KeInitializeEvent(&deviceExtension->NoPendingIoEvent, NotificationEvent, FALSE);

		// spinlock used to protect inc/dec iocount logic
		KeInitializeSpinLock (&deviceExtension->IoCountSpinLock);

        // spinlock used to protect test of deviceExtension->BaseIrp in
        // Uusbd_StagedReadWrite()
//        KeInitializeSpinLock(&deviceExtension->FastCompleteSpinlock);

		//free buffer from unicode string we used to init interface
		RtlFreeUnicodeString( &deviceLinkUnicodeString );
    }


    return ntStatus;
}


NTSTATUS
Uusbd_CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB Urb
    )
/***

Routine Description:

    URB を下層にあるUSBD class driverに渡す
	USBDには次のようなIRPを作り渡す
	Irp->MajorFunction = set to IRP_MJ_INTERNAL_DEVICE_CONTROL
	Parameters.DeviceIoControl.IoControlCode =IOCTL_INTERNAL_USB_SUBMIT_URB
	Parameters.Others.Argument1 = Urb

Arguments:

    DeviceObject
		pointer to the physical device object (PDO)

    Urb
		フォーマットされた（値がセットされた）URBへのポインター

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

***/
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;

    UUSBD_KdPrint( DBGLVL_MAXIMUM,("enter Uusbd_CallUSBD\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    // 同期のためのイベントを初期化しておく
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_SUBMIT_URB,
                deviceExtension->TopOfStackDeviceObject, //　USBDのデバイスオブジェクト
                NULL, // optional input bufer; none needed here
                0,	  // input buffer len if used
                NULL, // optional output bufer; none needed here
                0,    // output buffer len if used
                TRUE, // IRP_MJ_INTERNAL_DEVICE_CONTROL, IRP_MJ_SCSIの場合はTRUE
					  // IRP_MJ_DEVICE_CONTROL の場合はFALSSE
                &event,     // 完了した時にシグナル状態にするイベント
                &ioStatus);  // リクエストが完了した時にセットするI/O status block
    //
    // As an alternative, we could call KeDelayExecutionThread, wait for some
    // period of time, and try again....but we keep it simple for right now
    //
    if (!irp) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    nextStack = IoGetNextIrpStackLocation(irp);
    UUSBD_ASSERT(nextStack != NULL);

    //
    // pass the URB to the USB driver stack
    //
    nextStack->Parameters.Others.Argument1 = Urb;

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    UUSBD_KdPrint( DBGLVL_MAXIMUM,("Uusbd_CallUSBD() return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

		// リクエストが完了していない場合STATUS_PENDINGが帰る
		// この場合はリクエストが完了するまで待つ
        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

    } else {
        ioStatus.Status = ntStatus;
    }

    UUSBD_KdPrint( DBGLVL_MAXIMUM,("Uusbd_CallUSBD() URB status = %x status = %x irp status %x\n",
        Urb->UrbHeader.Status, status, ioStatus.Status));

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    UUSBD_KdPrintCond( DBGLVL_MAXIMUM, !NT_SUCCESS( ntStatus ), ("exit Uusbd_CallUSBD FAILED (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
Uusbd_ConfigureDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

    与えられたUSBデバイスをコンフィグレーションする

Arguments:

    DeviceObject
		このデバイスのFDOへのポインター

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;

    UUSBD_KdPrint( DBGLVL_HIGH,("enter Uusbd_ConfigureDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

	UUSBD_ASSERT( deviceExtension->UsbConfigurationDescriptor == NULL );

    urb = UUSBD_ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
	if ( !urb )
		return STATUS_INSUFFICIENT_RESOURCES;

	// コンフィグレーションディスクリプターは大きさが可変なため
	// UsbBuildGetDescriptorRequest()を呼び出す時に必要なメモリー量が
	// 分らない。仮に512バイトとる
    siz = sizeof(USB_CONFIGURATION_DESCRIPTOR) + 512;  

	// UsbBuildGetDescriptorRequest()を呼び出し、メモリー量が不足していたら
	// メモリーをアロケートしなおしてもう一度呼び出す
	while( 1 ) {

		deviceExtension->UsbConfigurationDescriptor = UUSBD_ExAllocatePool(NonPagedPool, siz);

		if ( !deviceExtension->UsbConfigurationDescriptor ) {
		    UUSBD_ExFreePool(urb);
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		UsbBuildGetDescriptorRequest(urb,
									 (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
									 USB_CONFIGURATION_DESCRIPTOR_TYPE,
									 0,
									 0,
									 deviceExtension->UsbConfigurationDescriptor,
									 NULL,
									 siz,
									 NULL);

		ntStatus = Uusbd_CallUSBD(DeviceObject, urb);

		UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_CallUSBD() Configuration Descriptor = %x, len %x\n",
						deviceExtension->UsbConfigurationDescriptor,
						urb->UrbControlDescriptorRequest.TransferBufferLength));
		//
		// if we got some data see if it was enough.
		// NOTE: we may get an error in URB because of buffer overrun
		if (urb->UrbControlDescriptorRequest.TransferBufferLength>0 &&
				deviceExtension->UsbConfigurationDescriptor->wTotalLength > siz) {

			// メモリーサイズが足りなかった場合
			siz = deviceExtension->UsbConfigurationDescriptor->wTotalLength;
			UUSBD_ExFreePool(deviceExtension->UsbConfigurationDescriptor);
			deviceExtension->UsbConfigurationDescriptor = NULL;
		} else {
			break;  // we got it on the first try
		}

	} // end, while (retry loop )

    UUSBD_ExFreePool(urb);
	UUSBD_ASSERT( deviceExtension->UsbConfigurationDescriptor );

	// UsbConfigurationDescriptorを元にSelectInterfaceする
    ntStatus = Uusbd_SelectInterface(DeviceObject,
        deviceExtension->UsbConfigurationDescriptor);


    UUSBD_KdPrint( DBGLVL_HIGH,("exit Uusbd_ConfigureDevice (%x)\n", ntStatus));

    return ntStatus;
} 

NTSTATUS
Uusbd_SelectInterface(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    )
/***

Routine Description:

	USBデバイスの存在する全てのインタフェースを使用可能にする

Arguments:

    DeviceObject
		このデバイスのFDOへのポインター

    ConfigurationDescriptor
		コンフィグレーションディスクリプターへのポインター
		インターフェースディスクリプター、エンドポイントディスクリプター
		が含まれている事

Return Value:

    NT status code

***/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb = NULL;
    ULONG i, j, num_interface;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor = NULL;
	PUUSBD_INTERFACEINFO Interface = NULL;
    USHORT siz;
	PUSBD_INTERFACE_LIST_ENTRY interfacelist;
	ULONG maxtransfer;

    deviceExtension = DeviceObject->DeviceExtension;

	// USBデバイスの持つインターフェースの数
	num_interface = ConfigurationDescriptor->bNumInterfaces ;

	// インターフェースの数+1の大きさの配列を取る
	// 最後の要素は終了を表すNULLとする　
	interfacelist = UUSBD_ExAllocatePool(NonPagedPool,sizeof(USBD_INTERFACE_LIST_ENTRY)*(num_interface+1)); // +1 for NULL entry
    if (!interfacelist) {
        UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_SelectInterface() USBD_CreateConfigurationRequest() failed\n  returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
	    return ntStatus; 
	}

	for(i=0;i<num_interface;i++){
		// ConfigurationDescriptorからInterfaceDescriptorを取り出す
	    interfaceDescriptor =
            USBD_ParseConfigurationDescriptorEx(ConfigurationDescriptor,
								  ConfigurationDescriptor, // find next one
								  i,	// interface number not a criteria
								  -1,   // not interested in alternate setting here either
								  -1,   // interface class not a criteria
								  -1,   // interface subclass not a criteria
								  -1    // interface protocol not a criteria
								  );
		if(interfaceDescriptor == NULL) break;
		interfacelist[i].InterfaceDescriptor = interfaceDescriptor;
	}
	interfacelist[i].InterfaceDescriptor = NULL; // end of list

	// 実際にConfigurationDescriptor に存在したinterfaceの数に上書きする
	num_interface = i;
	
    if (num_interface==0) {
        UUSBD_ExFreePool(interfacelist);
        UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_SelectInterface() USBD_CreateConfigurationRequest() failed\n  returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
	    return ntStatus; 
	}

	// select a configuration 用のURBを作る
	urb = USBD_CreateConfigurationRequestEx(ConfigurationDescriptor, interfacelist);

    if (!urb) {
        UUSBD_ExFreePool(interfacelist);
        UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_SelectInterface() USBD_CreateConfigurationRequest() failed\n  returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
	    return ntStatus; 
	}

	maxtransfer = USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
	ntStatus = ReadRegistry(deviceExtension->PhysicalDeviceObject,
		REG_MaxTransferSize, &maxtransfer);

	if(NT_SUCCESS(ntStatus)) {

	for(j=0;j<num_interface;j++){
        for (i=0; i< interfacelist[j].Interface->NumberOfPipes; i++) {
			// select a configurationを実行する前にパラメーターを修正する
			interfacelist[j].Interface->Pipes[i].MaximumTransferSize = maxtransfer;
	        UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_SelectInterface() if=%d, pipe=%d MaximumTransferSize = %d\n", j,i,interfacelist[j].Interface->Pipes[i].MaximumTransferSize));
        }
	}
	}
	ntStatus = Uusbd_CallUSBD(DeviceObject, urb);

    if (NT_SUCCESS(ntStatus)) {
		// インターフェースの情報をdeviceExtensionに保存する

        deviceExtension->UsbConfigurationHandle =
            urb->UrbSelectConfiguration.ConfigurationHandle;

        deviceExtension->UsbNumInterface = num_interface;

		deviceExtension->UsbInterfaceInfo = UUSBD_ExAllocatePool(NonPagedPool,
			sizeof(UUSBD_INTERFACEINFO)*num_interface);

		UUSBD_KdPrint( DBGLVL_MEDIUM,("Found %d interface\n", num_interface));
		for(i=0;i<num_interface;i++){
			// インターフェースについてのloop

			Interface = &deviceExtension->UsbInterfaceInfo[i];
			Interface->NumberOfPipes = interfacelist[i].Interface->NumberOfPipes;
			UUSBD_KdPrint( DBGLVL_MEDIUM,("Interface %d have %d pipe(s)\n", i, Interface->NumberOfPipes));
			Interface->Pipe = UUSBD_ExAllocatePool(NonPagedPool,
				sizeof(UUSBD_PIPE)*Interface->NumberOfPipes);
	        for (j=0; j<Interface->NumberOfPipes; j++) {
				PUUSBD_PIPE pipe;
				pipe = &Interface->Pipe[j];
				pipe->PipeOpenCount = 0;
				RtlCopyMemory(&pipe->PipeInfo, 
					&interfacelist[i].Interface->Pipes[j],
					sizeof(USBD_PIPE_INFORMATION));
				pipe->PendingIoIrps = NULL;
				pipe->BaseIrp = NULL;
				pipe->StagedPendingIrpCount = 0;
				pipe->StagedBytesTransferred = 0;
 
				KeInitializeEvent(&pipe->StagingDoneEvent, NotificationEvent, FALSE);
				KeInitializeSpinLock(&pipe->FastCompleteSpinlock);
				{
				PUSBD_PIPE_INFORMATION pipeInformation;
				pipeInformation = &pipe->PipeInfo;
				UUSBD_KdPrint( DBGLVL_MEDIUM,("PipeInfo address 0x%08x\n", pipeInformation));
				UUSBD_KdPrint( DBGLVL_MEDIUM,("PipeType 0x%x\n", pipeInformation->PipeType));
				UUSBD_KdPrint( DBGLVL_MEDIUM,("EndpointAddress 0x%x\n", pipeInformation->EndpointAddress));
				UUSBD_KdPrint( DBGLVL_MEDIUM,("MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize));
				UUSBD_KdPrint( DBGLVL_MEDIUM,("Interval 0x%x\n", pipeInformation->Interval));
				UUSBD_KdPrint( DBGLVL_MEDIUM,("Handle 0x%x\n", pipeInformation->PipeHandle));
				UUSBD_KdPrint( DBGLVL_MEDIUM,("MaximumTransferSize 0x%x\n", pipeInformation->MaximumTransferSize));
				}
			}
	    }
	}
    if (urb) ExFreePool(urb);
	if (interfacelist) UUSBD_ExFreePool(interfacelist);

    return ntStatus; 
}

NTSTATUS
Uusbd_ResetPipe(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSBD_PIPE_INFORMATION PipeInfo
    )
/***

Routine Description:

    パイプをリセットする

    NOTES:

    This will reset the host to Data0 and should also reset the device to Data0 

Arguments:
	DeviceObject
		リセットするパイプのデバイスのFDOへのポインター
	PipeInfo
		リセットするパイプ

Return Value:

    NT status code

***/
{
    NTSTATUS ntStatus;
    PURB urb;
    PDEVICE_EXTENSION deviceExtension;

    deviceExtension = DeviceObject->DeviceExtension;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_ResetPipe() Reset Pipe %x\n", PipeInfo));

    urb = UUSBD_ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_PIPE_REQUEST));

    if (urb) {

        urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
        urb->UrbPipeRequest.PipeHandle =
            PipeInfo->PipeHandle;

        ntStatus = Uusbd_CallUSBD(DeviceObject, urb);

        UUSBD_ExFreePool(urb);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (!(NT_SUCCESS(ntStatus))) {
#if DBG
		if ( gpDbg )
			gpDbg->PipeErrorCount++;
#endif

        UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_ResetPipe() FAILED, ntStatus =0x%x\n", ntStatus ));
    }
    else {
#if DBG
		if ( gpDbg )
			gpDbg->ResetPipeCount++;
#endif

        UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_ResetPipe() SUCCESS, ntStatus =0x%x\n", ntStatus ));
    }

    return ntStatus;
}




LONG
Uusbd_DecrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

	ペンディング中のIOの数をdevice extensionに保存しておく
	IRPを受け取るとカウントアップし、IRPが完了するとカウントダウンする
	このカウンターはデバイスが接続された時 1 になる

	デバイスのremove処理は処理中のIRPが無くなってから行うように
	IRP_MN_QUERY_REMOVE_DEVICEを処理する時にカウンターが１
	にデクリメントされているまでまつ。

Arguments:
	DeviceObject
		リセットするパイプのデバイスのFDOへのポインター

Return Value:

	deviceExtension->PendingIoCount


***/
{
    PDEVICE_EXTENSION deviceExtension;
    LONG ioCount;
    KIRQL             oldIrql;

    deviceExtension = DeviceObject->DeviceExtension;
	KeAcquireSpinLock (&deviceExtension->IoCountSpinLock, &oldIrql);

    ioCount = InterlockedDecrement(&deviceExtension->PendingIoCount);
#if DBG
    InterlockedDecrement(&gpDbg->PendingIoCount);
#endif


    UUSBD_TrapCond( DBGLVL_HIGH,( 0 > ioCount ) );

    if (ioCount==1) {
		// 1にデクリメントされた時ペンディング中のIRPがなくなった
		// ことを表す
        KeSetEvent(&deviceExtension->NoPendingIoEvent,
                   1,
                   FALSE);
    }

    if (ioCount==0) {
		// 0にデクリメントされた時リムーブイベントを処理してよいことを表す
        KeSetEvent(&deviceExtension->RemoveEvent,
                   1,
                   FALSE);
    }
	KeReleaseSpinLock (&deviceExtension->IoCountSpinLock, oldIrql);

    UUSBD_KdPrint( DBGLVL_HIGH,("Exit Uusbd_DecrementIoCount() Pending io count = %x\n", ioCount));
    return ioCount;
}


VOID
Uusbd_IncrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

	ペンディング中のIOの数をdevice extensionに保存しておく
	IRPを受け取るとカウントアップし、IRPが完了するとカウントダウンする

Arguments:
	DeviceObject
		リセットするパイプのデバイスのFDOへのポインター

Return Value:

        None
***/
{
    PDEVICE_EXTENSION deviceExtension;
    LONG ioCount;
    KIRQL             oldIrql;

    deviceExtension = DeviceObject->DeviceExtension;

    UUSBD_KdPrint( DBGLVL_MAXIMUM,("Enter Uusbd_IncrementIoCount() Pending io count = %x\n", deviceExtension->PendingIoCount));

	KeAcquireSpinLock (&deviceExtension->IoCountSpinLock, &oldIrql);

    ioCount = InterlockedIncrement(&deviceExtension->PendingIoCount);
#if DBG
    InterlockedIncrement(&gpDbg->PendingIoCount);
#endif
    if (ioCount==2) {
		// 2にインクリメントされた時ペンディング中のIRPがある
		// ことを表す
        KeClearEvent(&deviceExtension->NoPendingIoEvent);
    }
	KeReleaseSpinLock (&deviceExtension->IoCountSpinLock, oldIrql);

    UUSBD_KdPrint( DBGLVL_HIGH,("Exit Uusbd_IncrementIoCount() Pending io count = %x\n", deviceExtension->PendingIoCount));
}


