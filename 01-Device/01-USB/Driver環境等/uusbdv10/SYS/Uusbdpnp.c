/***************************************************

Copyright (c) 1999  kashiwano masahiro

Abstract:

    UUSBD.SYS Pnp functions
    
****************************************************/

#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "uusbd.h"

ULONG SumStagedPendingIrpCount(PDEVICE_EXTENSION deviceExtension)
{
	ULONG i,j;
	ULONG sum;
	sum = 0;
	for(i=0;i<deviceExtension->UsbNumInterface;i++){
		for(j=0;j<deviceExtension->UsbInterfaceInfo[i].NumberOfPipes;j++){
			sum += deviceExtension->UsbInterfaceInfo[i].Pipe[j].StagedPendingIrpCount; 
		}
	}
	return sum;
}


NTSTATUS
Uusbd_ProcessPnPIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/***

Routine Description:

    Plug and Play IRPの処理ルーチン.

Arguments:
	DeviceObject
		デバイスのFDOへのポインター

    Irp
		I/O Request Packetへのポインター

Return Value:

    NT status code

***/
{

    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    NTSTATUS waitStatus;
    PDEVICE_OBJECT stackDeviceObject;
    KEVENT startDeviceEvent;


    //
    // 現在のIO_STACK_LOCATIONを得る
    // ここに function codesやパラメーターがある
    //
    irpStack = IoGetCurrentIrpStackLocation (Irp);

    //
    // Get a pointer to the device extension
    //

    deviceExtension = DeviceObject->DeviceExtension;
    stackDeviceObject = deviceExtension->TopOfStackDeviceObject;

    UUSBD_KdPrint( DBGLVL_MEDIUM, ( "enter Uusbd_ProcessPnPIrp() IRP_MJ_PNP, minor %s\n",
        UUSBD_StringForPnpMnFunc( irpStack->MinorFunction ) ));

	// countをインクリメントする
    Uusbd_IncrementIoCount(DeviceObject);

    UUSBD_ASSERT( IRP_MJ_PNP == irpStack->MajorFunction );

    switch (irpStack->MinorFunction) {
    case IRP_MN_START_DEVICE:

		// PnPマネージャはリソースを割り当てた後IRP_MN_START_DEVICE
		// を送る。
		// まず下層にこのIRPを送る

		// 下層が処理完了するのを知らせるイベントを初期化しておく
        KeInitializeEvent(&startDeviceEvent, NotificationEvent, FALSE);
        IoCopyCurrentIrpStackLocationToNext(Irp);

        // 下層がIRPの処理を完了した時にUusbd_IrpCompletionRoutineが呼ばれる
		// ようにする.この関数の中で startDeviceEventをシグナル状態にする
        IoSetCompletionRoutine(Irp,
                               Uusbd_IrpCompletionRoutine,	//完了ルーチン
                               &startDeviceEvent,			//完了ルーチン Context引数
                               TRUE,    // invoke on success
                               TRUE,    // invoke on error
                               TRUE);   // invoke on cancellation


        // 下層にIRPを処理させる
        ntStatus = IoCallDriver(stackDeviceObject,
                                Irp);

        // if PDO is not done yet, wait for the event to be set in our completion routine
        if (ntStatus == STATUS_PENDING) {
	        // 下層の処理が完了してない場合完了するまで待つ。
			// 完了時にstartDeviceEventがシグナル状態になる
            waitStatus = KeWaitForSingleObject(
                &startDeviceEvent,
                Suspended,
                KernelMode,
                FALSE,
                NULL);
			ntStatus = Irp->IoStatus.Status;
        }
		if (NT_SUCCESS(ntStatus)) {

		//この時点でデバイスが使用可能になる
		//初期化処理などを行う
	        ntStatus = Uusbd_StartDevice(DeviceObject);
	        Irp->IoStatus.Status = ntStatus;
		}
		IoCompleteRequest (Irp,
						   IO_NO_INCREMENT
						   );

		Uusbd_DecrementIoCount(DeviceObject);
		return ntStatus;  // end, case IRP_MN_START_DEVICE

    case IRP_MN_QUERY_STOP_DEVICE:

		// STOP_DEVICEの要求
		// The IRP_MN_QUERY_STOP_DEVICE/IRP_MN_STOP_DEVICE sequence only occurs
		// during "polite" shutdowns, such as the user explicitily requesting the
		// service be stopped in, or requesting unplug from the Pnp tray icon.
		// This sequence is NOT received during "impolite" shutdowns,
		// such as someone suddenly yanking the USB cord or otherwise 
		// unexpectedly disabling/resetting the device.

        // If a driver sets STATUS_SUCCESS for this IRP,
        // the driver must not start any operations on the device that
        // would prevent that driver from successfully completing an IRP_MN_STOP_DEVICE
        // for the device.
        // For mass storage devices such as disk drives, while the device is in the
        // stop-pending state,the driver holds IRPs that require access to the device,
        // but for most USB devices, there is no 'persistent storage', so we will just
        // refuse any more IO until restarted or the stop is cancelled

        // If a driver in the device stack determines that the device cannot be
        // stopped for resource reconfiguration, the driver is not required to pass
        // the IRP down the device stack. If a query-stop IRP fails,
        // the PnP Manager sends an IRP_MN_CANCEL_STOP_DEVICE to the device stack,
        // notifying the drivers for the device that the query has been cancelled
        // and that the device will not be stopped.


        // It is possible to receive this irp when the device has not been started
        //  ( as on a boot device )
        if (!deviceExtension->DeviceStarted) { // if get when never started, just pass on
            UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPnPIrp() IRP_MN_QUERY_STOP_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            Uusbd_DecrementIoCount(DeviceObject);

            return ntStatus;
        }


        // read/write IRPを待っている場合、拒否する
        if( SumStagedPendingIrpCount(deviceExtension) ) {
            ntStatus = STATUS_UNSUCCESSFUL;
        }
        else { 
			// 拒否しない
			// これ以後、新たなIOCTL や read/writeしないようにフラグを立てる
             deviceExtension->StopDeviceRequested = TRUE;
			 Irp->IoStatus.Status = STATUS_SUCCESS;
        }

        break; // end, case IRP_MN_QUERY_STOP_DEVICE

    case IRP_MN_CANCEL_STOP_DEVICE:

		// PnPマネージャーは停止できないデバイスがあった時、
		// このIRPを送り停止要求をキャンセルすることを知らせる
		// このIRPはIRP_MN_QUERY_STOP_DEVICEが成功したデバイスに対してのみ送られる


        // It is possible to receive this irp when the device has not been started
        if (!deviceExtension->DeviceStarted) { // まだスタートしてない時は、下層にパスするのみ
            UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPnPIrp() IRP_MN_CANCEL_STOP_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            Uusbd_DecrementIoCount(DeviceObject);
            return ntStatus;
        }

		// 再びIOCTL や read/writeを処理可能にする
        deviceExtension->StopDeviceRequested = FALSE;
		Irp->IoStatus.Status = STATUS_SUCCESS;
        break; // end, case IRP_MN_CANCEL_STOP_DEVICE

    case IRP_MN_STOP_DEVICE:

		// PnP Managerはデバイスを停止させる時にこのIRPを送る
		// 停止させたあとハードウエアリソースの再紺リグレーションが
		// 可能になる
		// IRP_MN_QUERY_STOP_DEVICEが成功したときのみこのIRPを送る

        ntStatus = Uusbd_StopDevice(DeviceObject, TRUE);

		Irp->IoStatus.Status = ntStatus;
        break; // end, case IRP_MN_STOP_DEVICE



    case IRP_MN_QUERY_REMOVE_DEVICE:

		//このIRPは安全に（システムに悪影響無く）デバイスを取り外す
		//ことができるかどうか問い合わせるために使われる
		//もしドライバーがデバイスを取り外しても安全だと判断したら
		//まだ終わっていないIOリクエストを完了させ、これ以降のread/write
		//リクエストをやめてSTATUS_SUCCESSを返す。
		//この場合、IRP_MN_REMOVE_DEVICEの処理が確実に成功するように
		//以後新しい動作をしてはいけない

		//もし取り外せないと判断した場合、下のデバイススタックに
		//IRP_MN_QUERY_REMOVE_DEVICEを送ってはいけない。
		//もしこのIRPが失敗した場合、PnPマネージャーは、
		//IRP_MN_CANCEL_REMOVE_DEVICEをデバイススタックに送って
		//REMOVE_DEVICEの要求をキャンセルする

        // It is possible to receive this irp when the device has not been started
        if (!deviceExtension->DeviceStarted) { // if get when never started, just pass on
            UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPnPIrp() IRP_MN_QUERY_STOP_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            Uusbd_DecrementIoCount(DeviceObject);

            return ntStatus;
        }

		// これ以後、新たなIOCTL や read/writeのIRPが下層のドライバーに渡らない
		// ようにようにフラグを立てる
       deviceExtension->RemoveDeviceRequested = TRUE;

		// このルーチンの初めでインクリメントしているので一時的にデクリメントする
		Uusbd_DecrementIoCount(DeviceObject);

        // ペンディング中のIOリクエストがあればそれが終わるまでまつ
		// deviceExtension->PendingIoCountが1になった時、NoPendingIoEvent
		// がシグナル状態になる
        waitStatus = KeWaitForSingleObject(
                    &deviceExtension->NoPendingIoEvent,
                    Suspended,
                    KernelMode,
                    FALSE,
                    NULL);

		// 一時的なデクリメントを戻す		
		Uusbd_IncrementIoCount(DeviceObject);

		Irp->IoStatus.Status = STATUS_SUCCESS;
        break; // end, case IRP_MN_QUERY_REMOVE_DEVICE

    case IRP_MN_CANCEL_REMOVE_DEVICE:

		// PnPマネージャがリムーブ要求を取り消すときこのIRPを使う
		// IRP_MN_QUERY_REMOVE_DEVICEが成功したときのみこのIRPは来る

        if (!deviceExtension->DeviceStarted) { // if get when never started, just pass on
            UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPnPIrp() IRP_MN_CANCEL_REMOVE_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            Uusbd_DecrementIoCount(DeviceObject);
            return ntStatus;
        }

		// 再び、新しいIOCTL や IO Irp を受けつけるようにする
        deviceExtension->RemoveDeviceRequested = FALSE;

		Irp->IoStatus.Status = STATUS_SUCCESS;
        break; // end, case IRP_MN_CANCEL_REMOVE_DEVICE

	case IRP_MN_SURPRISE_REMOVAL:
        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPnPIrp() IRP_MN_SURPRISE_REMOVAL\n"));
		
		// 突然デバイスが取り外された場合 
		// この場合PnPマネージャーはIRP_MN_QUERY_REMOVE_DEVICEを
		// 送ることなく実際のデバイスが既に取り外されている

        // match the inc at the begining of the dispatch routine
        Uusbd_DecrementIoCount(DeviceObject);

		//
        // これ以降、新たなIOCTLやread/write irpが下層のドライバーに渡らない
		// ようにフラグを立てる
        deviceExtension->DeviceRemoved = TRUE;

		if(deviceExtension->DeviceStarted){
			Uusbd_StopDevice(DeviceObject, FALSE);
		}

		Irp->IoStatus.Status = STATUS_SUCCESS;
		// We don't explicitly wait for the below driver to complete, but just make
		// the call and go on, finishing cleanup
		// 下層のドライバー同じIRPを送る
        IoCopyCurrentIrpStackLocationToNext(Irp);

        ntStatus = IoCallDriver(stackDeviceObject,
                                Irp);

		return ntStatus;

    case IRP_MN_REMOVE_DEVICE:

		// デバイスを取り外す時 PnP ManagerはこのIRPを発行する
		// 予期された取り外しの場合、事前にIRP_MN_QUERY_REMOVE_DEVICE
		// が送られデバイスはリムーブを待つ状態になっている。
		// しかし予期されない場合（コードを抜いた場合等）なんの前触れ
		// もなく実際のデバイスそのものが取り外されている。
		// この場合は、IRP_MN_QUERY_REMOVE_DEVICEは送られない。

        // match the inc at the begining of the dispatch routine
        Uusbd_DecrementIoCount(DeviceObject);

		//
        // これ以降、新たなIOCTLやread/write irpが下層のドライバーに渡らない
		// ようにフラグを立てる
        deviceExtension->DeviceRemoved = TRUE;

		if(deviceExtension->DeviceStarted){
			Uusbd_StopDevice(DeviceObject, TRUE);
		}
		// We don't explicitly wait for the below driver to complete, but just make
		// the call and go on, finishing cleanup
		// 下層のドライバー同じIRPを送る
        IoCopyCurrentIrpStackLocationToNext(Irp);

        ntStatus = IoCallDriver(stackDeviceObject,
                                Irp);
		// PendingIoCountをデクリメントする。
		// これで、ペンディング中のIRPが無くなった時にPendingIoCountが0になる
		// 0になった時に、RemoveEventがシグナル状態になる
        Uusbd_DecrementIoCount(DeviceObject);


        // wait for any io request pending in our driver to
        // complete for finishing the remove

        KeWaitForSingleObject(
                    &deviceExtension->RemoveEvent,
                    Suspended,
                    KernelMode,
                    FALSE,
                    NULL);

        //
        // Delete the link and FDO we created
        //
        Uusbd_RemoveDevice(DeviceObject);

        return ntStatus; // end, case IRP_MN_REMOVE_DEVICE


    default:
        UUSBD_KdPrint( DBGLVL_MAXIMUM,("Uusbd_ProcessPnPIrp() Minor PnP IOCTL not handled\n"));
    } /* case MinorFunction  */


    if (!NT_SUCCESS(ntStatus)) {

		// エラーがあった場合、下層にIRPを送らず終了する
        Irp->IoStatus.Status = ntStatus;
		IoCompleteRequest (Irp,
						   IO_NO_INCREMENT
						   );

		Uusbd_DecrementIoCount(DeviceObject);

		UUSBD_KdPrint( DBGLVL_MAXIMUM,("Uusbd_ProcessPnPIrp() Exit Uusbd_ProcessPnPIrp FAILURE %x\n", ntStatus));
		return ntStatus;
    }

    //
    // 全ての PNP_POWER messagesは下層のデバイスオブジェクト（TopOfStackDeviceObject）
	// に送る
	// IRP_MN_START_DEVICEの場合は下層に送ってから、自身のデバイスに関する処理を
	// 行うので、以下のルーチンは使用しない。
	// 取り外しの場合は自身のデバイスの処理をしてから以下のルーチンを使って
	// 下層にIRPを引きつぐ
    IoCopyCurrentIrpStackLocationToNext(Irp);

    //
    // All PNP_POWER messages get passed to the TopOfStackDeviceObject
    // we were given in PnPAddDevice
    //

    UUSBD_KdPrint( DBGLVL_MAXIMUM,("Uusbd_ProcessPnPIrp() Passing PnP Irp down, status = %x\n", ntStatus));

    ntStatus = IoCallDriver(stackDeviceObject,
                            Irp);

    Uusbd_DecrementIoCount(DeviceObject);

    UUSBD_KdPrint( DBGLVL_MAXIMUM,("Uusbd_ProcessPnPIrp() Exit Uusbd_ProcessPnPIrp %x\n", ntStatus));

    return ntStatus;
}


NTSTATUS
Uusbd_PnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/***

Routine Description:

	Functional Device Object (FDO)を作成初期化する時によびだされる.

Arguments:

    DriverObject
		UUSBDのデバイスオブジェクト

    PhysicalDeviceObject
		このデバイススタックのPDO（バスドライバーによって作られる）

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

***/
{
    NTSTATUS                ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT          deviceObject = NULL;
    PDEVICE_EXTENSION       deviceExtension;
    USBD_VERSION_INFORMATION versionInformation;
    ULONG i;


    
    UUSBD_KdPrint( DBGLVL_DEFAULT,("enter Uusbd_PnPAddDevice()\n"));

    //
    // create our funtional device object (FDO)
    //

    // funtional device object (FDO)を作る
    ntStatus =
        Uusbd_CreateDeviceObject(DriverObject, PhysicalDeviceObject, &deviceObject);

    if (NT_SUCCESS(ntStatus)) {
        deviceExtension = deviceObject->DeviceExtension;

//        deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

        // read/write時、ユーザーモードメモリーに
		// 直接データーを書く direct io をサポートする 
        deviceObject->Flags |= DO_DIRECT_IO;


		// サスペンド中、レジューム中にIRP_MN_STOP_DEVICE,IRP_MN_START_DEVICE
		// のIRPが起きないようにする
		// IRP_MN_START_DEVICEでGetDescriptors()が失敗するのを防ぐため。
        deviceObject->Flags |= DO_POWER_PAGABLE;
        // device extensionの初期化
        //
        // Physical device Objectを覚えておく
        deviceExtension->PhysicalDeviceObject=PhysicalDeviceObject;

        //
        // PDOにアタッチする。
        // アタッチした時に一番上にあったデバイスを
		// TopOfStackDeviceObjectに覚えておく
		// これが自分の下層になる。

        deviceExtension->TopOfStackDeviceObject =
            IoAttachDeviceToDeviceStack(deviceObject, PhysicalDeviceObject);

		// device extensionのDEVICE_CAPABILITIESにデバイスの物理的特性を
		// コピーする
        // We are most interested in learning which system power states
        // are to be mapped to which device power states for handling
        // IRP_MJ_SET_POWER Irps.
        Uusbd_QueryCapabilities(deviceExtension->TopOfStackDeviceObject,
                                 &deviceExtension->DeviceCapabilities);


		// We want to determine what level to auto-powerdown to; This is the lowest
		//  sleeping level that is LESS than D3; 
		// If all are set to D3, auto powerdown/powerup will be disabled.

        deviceExtension->PowerDownLevel = PowerDeviceUnspecified; // init to disabled
        for (i=PowerSystemSleeping1; i<= PowerSystemSleeping3; i++) {

			if ( deviceExtension->DeviceCapabilities.DeviceState[i] < PowerDeviceD3 )
				deviceExtension->PowerDownLevel = deviceExtension->DeviceCapabilities.DeviceState[i];
        }

#if DBG

        // May want override auto power-down level from registry;
        // ( CurrentControlSet\Services\BulkUsb\Parameters )
		// Setting to 0 or 1 in registry disables auto power-down
        Uusbd_GetRegistryDword( UUSBD_REGISTRY_PARAMETERS_PATH,
                                         L"PowerDownLevel",
                                         &(deviceExtension->PowerDownLevel) );



        //
        // display the device  caps
        //

        UUSBD_KdPrint( DBGLVL_MEDIUM,(" >>>>>> DeviceCaps\n"));
        UUSBD_KdPrint( DBGLVL_MEDIUM,(" SystemWake = %s\n",
            UUSBD_StringForSysState( deviceExtension->DeviceCapabilities.SystemWake ) ));
        UUSBD_KdPrint( DBGLVL_MEDIUM,(" DeviceWake = %s\n",
            UUSBD_StringForDevState( deviceExtension->DeviceCapabilities.DeviceWake) ));

        for (i=PowerSystemUnspecified; i< PowerSystemMaximum; i++) {

            UUSBD_KdPrint( DBGLVL_MEDIUM,(" Device State Map: sysstate %s = devstate %s\n",
                 UUSBD_StringForSysState( i ),
                 UUSBD_StringForDevState( deviceExtension->DeviceCapabilities.DeviceState[i] ) ));
        }
        UUSBD_KdPrint( DBGLVL_MEDIUM,(" <<<<<<<<DeviceCaps\n"));
#endif
        // We keep a pending IO count ( extension->PendingIoCount )  in the device extension.
        // The first increment of this count is done on adding the device.
        // Subsequently, the count is incremented for each new IRP received and
        // decremented when each IRP is completed or passed on.

        // Transition to 'one' therefore indicates no IO is pending and signals
        // deviceExtension->NoPendingIoEvent. This is needed for processing
        // IRP_MN_QUERY_REMOVE_DEVICE

        // Transition to 'zero' signals an event ( deviceExtension->RemoveEvent )
        // to enable device removal. This is used in processing for IRP_MN_REMOVE_DEVICE
        //
        Uusbd_IncrementIoCount(deviceObject);

    }

    USBD_GetUSBDIVersion(&versionInformation);



    if( NT_SUCCESS( ntStatus ) )  
    {
        NTSTATUS actStat;
        // try to power down device until IO actually requested
        actStat = Uusbd_SelfSuspendOrActivate( deviceObject, TRUE );
		deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    }

    UUSBD_KdPrint( DBGLVL_DEFAULT,("Flag = %08x\n", deviceObject->Flags));

    UUSBD_KdPrint( DBGLVL_DEFAULT,("exit Uusbd_PnPAddDevice() (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
Uusbd_StartDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

    Uusbd_ProcessPnPIrp()の中から呼び出される。
	IRPの種類がIRP_MN_START_DEVICEで、下層にこのIRPの処理を渡し
	下層がスタートしたあとこの関数が呼ばれる。
	この時点で、URBを作り下層に渡すことができる。
	ここで、コンフィグレーションを行う

Arguments:
    DriverObject
		UUSBDのデバイスオブジェクト

Return Value:

    NT status code

***/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PUSB_DEVICE_DESCRIPTOR deviceDescriptor = NULL;
    PURB urb;
    ULONG siz;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("enter Uusbd_StartDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    urb = UUSBD_ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    UUSBD_KdPrintCond( DBGLVL_HIGH,!urb, ("Uusbd_StartDevice() FAILED UUSBD_ExAllocatePool() for URB\n"));

    if (urb) {

        siz = sizeof(USB_DEVICE_DESCRIPTOR);

        deviceDescriptor = UUSBD_ExAllocatePool(NonPagedPool,
                                          siz);

        UUSBD_KdPrintCond( DBGLVL_HIGH, !deviceDescriptor, ("Uusbd_StartDevice() FAILED UUSBD_ExAllocatePool() for deviceDescriptor\n"));

        if (deviceDescriptor) {

            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_DEVICE_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         deviceDescriptor,
                                         NULL,
                                         siz,
                                         NULL);


            ntStatus = Uusbd_CallUSBD(DeviceObject, urb);

            UUSBD_KdPrintCond( DBGLVL_DEFAULT, !NT_SUCCESS(ntStatus), ("Uusbd_StartDevice() FAILED Uusbd_CallUSBD(DeviceObject, urb)\n"));

            if (NT_SUCCESS(ntStatus)) {
                UUSBD_KdPrint( DBGLVL_MEDIUM,("Device Descriptor = %x, len %x\n",
                                deviceDescriptor,
                                urb->UrbControlDescriptorRequest.TransferBufferLength));

                UUSBD_KdPrint( DBGLVL_MEDIUM,("Device Descriptor:\n"));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("-------------------------\n"));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("bLength %d\n", deviceDescriptor->bLength));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("bDescriptorType 0x%x\n", deviceDescriptor->bDescriptorType));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("bcdUSB 0x%x\n", deviceDescriptor->bcdUSB));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("bDeviceClass 0x%x\n", deviceDescriptor->bDeviceClass));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("bDeviceSubClass 0x%x\n", deviceDescriptor->bDeviceSubClass));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("bDeviceProtocol 0x%x\n", deviceDescriptor->bDeviceProtocol));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("bMaxPacketSize0 0x%x\n", deviceDescriptor->bMaxPacketSize0));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("idVendor 0x%x\n", deviceDescriptor->idVendor));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("idProduct 0x%x\n", deviceDescriptor->idProduct));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("bcdDevice 0x%x\n", deviceDescriptor->bcdDevice));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("iManufacturer 0x%x\n", deviceDescriptor->iManufacturer));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("iProduct 0x%x\n", deviceDescriptor->iProduct));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("iSerialNumber 0x%x\n", deviceDescriptor->iSerialNumber));
                UUSBD_KdPrint( DBGLVL_MEDIUM,("bNumConfigurations 0x%x\n", deviceDescriptor->bNumConfigurations));
            }
        } else {
			// if we got here we failed to allocate deviceDescriptor
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

        if (NT_SUCCESS(ntStatus)) {
            deviceExtension->UsbDeviceDescriptor = deviceDescriptor;
        } else if (deviceDescriptor) {
            UUSBD_ExFreePool(deviceDescriptor);
        }

        UUSBD_ExFreePool(urb);

    } else {
		// if we got here we failed to allocate the urb
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (NT_SUCCESS(ntStatus)) {
        ntStatus = Uusbd_ConfigureDevice(DeviceObject);

        UUSBD_KdPrintCond( DBGLVL_MEDIUM,!NT_SUCCESS(ntStatus),("Uusbd_StartDevice Uusbd_ConfigureDevice() FAILURE (%x)\n", ntStatus));
    }


    if (NT_SUCCESS(ntStatus)) {
        deviceExtension->DeviceStarted = TRUE;
    }
    UUSBD_KdPrint( DBGLVL_DEFAULT, ("exit Uusbd_StartDevice (%x)\n", ntStatus));

    return ntStatus;
}

void
Uusbd_FreeMem(
    IN  PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

	Uusbd_StartDeviceでアロケートしたメモリーを解放する

Arguments:

	DeviceObject
		Uusbdのデバイスオブジェクト

Return Value:

    無し

***/
{
    PDEVICE_EXTENSION deviceExtension;
	ULONG i,j;

    deviceExtension = DeviceObject->DeviceExtension;

    UUSBD_KdPrint( DBGLVL_HIGH,("Enter UUSBD_FreeMem() gExAllocCount = dec %d\n", gExAllocCount ));
    // Free device descriptor structure
    if (deviceExtension->UsbDeviceDescriptor) {
        UUSBD_ExFreePool(deviceExtension->UsbDeviceDescriptor);
		deviceExtension->UsbDeviceDescriptor = NULL;
    }

    // Free pipe info structs
    if( deviceExtension->UsbInterfaceInfo ) { 
		for(i=0;i<deviceExtension->UsbNumInterface;i++){
			PUUSBD_INTERFACEINFO interfaceinfo = &deviceExtension->UsbInterfaceInfo[i];
			if(interfaceinfo) {
				if(interfaceinfo->Pipe) {
					UUSBD_ExFreePool(interfaceinfo->Pipe);
					interfaceinfo->Pipe = NULL;
				}
			}
		}
		UUSBD_ExFreePool(deviceExtension->UsbInterfaceInfo);
		deviceExtension->UsbInterfaceInfo = NULL;
    }

	// free up the USB config discriptor
    if (deviceExtension->UsbConfigurationDescriptor) {
        UUSBD_ExFreePool(deviceExtension->UsbConfigurationDescriptor);
		deviceExtension->UsbConfigurationDescriptor = NULL;
    }
    UUSBD_KdPrint( DBGLVL_HIGH,("exit UUSBD_FreeMem() gExAllocCount = dec %d\n", gExAllocCount ));
}

NTSTATUS
Uusbd_RemoveDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

	デバイスをリムーブする。
	アロケートしたメモリーや、シンボリックリンク名を解放する

Arguments:

	DeviceObject
		UUSBDのデバイスオブジェクト

Return Value:

    NT status code from free symbolic link operation

***/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    UNICODE_STRING deviceLinkUnicodeString;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("enter Uusbd_RemoveDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    RtlInitUnicodeString (&deviceLinkUnicodeString,
                          deviceExtension->DeviceLinkNameBuffer);

    // remove the GUID-based symbolic link
    ntStatus = IoSetDeviceInterfaceState(&deviceLinkUnicodeString, FALSE);
    UUSBD_ASSERT( NT_SUCCESS( ntStatus ) );


	UUSBD_KdPrint( DBGLVL_DEFAULT,("UUSBD_RemoveDevice() Detaching from %08X\n",
		deviceExtension->TopOfStackDeviceObject));

	IoDetachDevice(deviceExtension->TopOfStackDeviceObject);
	
	UUSBD_KdPrint( DBGLVL_DEFAULT,("UUSBD_RemoveDevice() Deleting %08X\n",
		DeviceObject));
	
	IoDeleteDevice (DeviceObject);

    UUSBD_KdPrint( DBGLVL_DEFAULT,("exit Uusbd_RemoveDevice() status = 0x%x\n", ntStatus ));

    return ntStatus;
}




NTSTATUS
Uusbd_UnconfigDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

	USBデバイスをアンコンフィグレーションの状態にする

Arguments:

	DeviceObject
		UUSBDのデバイスオブジェクト

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb;
    ULONG siz;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("enter Uusbd_UnconfigDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

	// コンフィグレーションハンドルをNULLにしてSelect configurationを呼び出す
	// USBデバイスは'unconfigured'stateになる

    siz = sizeof(struct _URB_SELECT_CONFIGURATION);

    urb = UUSBD_ExAllocatePool(NonPagedPool,
                         siz);

    if (urb) {
        UsbBuildSelectConfigurationRequest(urb,
                                          (USHORT) siz,
                                          NULL);

        ntStatus = Uusbd_CallUSBD(DeviceObject, urb);

        UUSBD_KdPrintCond( DBGLVL_DEFAULT,!NT_SUCCESS(ntStatus),("Uusbd_UnconfigDevice() FAILURE Configuration Closed status = %x usb status = %x.\n", ntStatus, urb->UrbHeader.Status));

        UUSBD_ExFreePool(urb);
    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    UUSBD_KdPrint( DBGLVL_DEFAULT,("exit Uusbd_UnconfigDevice() (%x)\n", ntStatus));

    return ntStatus;
}

NTSTATUS
Uusbd_StopDevice(
    IN  PDEVICE_OBJECT DeviceObject,
	IN  BOOLEAN do_unconfig
    )

{
    PDEVICE_EXTENSION deviceExtension;
    deviceExtension = DeviceObject->DeviceExtension;
	// リクエスト中のIRPをキャンセルする
	Uusbd_CancelPendingIo( DeviceObject ); 
	
	// オープンしているパイプがある場合、
	// USBD に URB_FUNCTION_ABORT_PIPEを送り、パイプをクローズする
	Uusbd_AbortPipes( DeviceObject );
	
	if(do_unconfig) Uusbd_UnconfigDevice(DeviceObject);

	Uusbd_FreeMem(DeviceObject);

	deviceExtension->DeviceStarted = FALSE;
    deviceExtension->StopDeviceRequested = FALSE;

	return STATUS_SUCCESS;
}

NTSTATUS
Uusbd_IrpCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/***

Routine Description:

	// 完了通知ルーチン
	// IRPが完了したときにContext引数にわたされたイベントをシグナル状態
	// にする
	// このルーチンは、PnP と Power Managementのルーチンで使用する

Arguments:

    DeviceObject
		Pointer to the device object for the class device.

    Irp
		Irp completed.

    Context
		Driver defined context, in this case a pointer to an event.

Return Value:

    The function value is the final status from the operation.

***/
{
    PKEVENT event = Context;

    // Set the input event
    KeSetEvent(event,
               1,       // Priority increment  for waiting thread.
               FALSE);  // Flag this call is not immediately followed by wait.

    // このIRPのIoFreeIrp()を呼び出してないので STATUS_MORE_PROCESSING_REQUIRED
	// を返す。
    return STATUS_MORE_PROCESSING_REQUIRED;

}


