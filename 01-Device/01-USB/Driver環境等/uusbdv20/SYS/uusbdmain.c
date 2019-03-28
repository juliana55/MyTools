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

    WDM�h���C�o�[�̃G���g���[�|�C���g�B
	I/O�V�X�e���ɂ���ČĂяo�����B

Arguments:

    DriverObject - ���̃h���C�o�[�� driver object�ւ̃|�C���^�[

    RegistryPath - �h���C�o�[���g�p���郌�W�X�g���[�p�X�iunicode)

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
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = Uusbd_ProcessSysControlIrp; // uusbdmain.c �@
    DriverObject->MajorFunction[IRP_MJ_PNP] = Uusbd_ProcessPnPIrp; // uusbdpnp.c
    DriverObject->MajorFunction[IRP_MJ_POWER] = Uusbd_ProcessPowerIrp; // uusbdpwr.c

    // The Functional Device Object (FDO) will not be created for PNP devices until 
    // this routine is called upon device plug-in.
    DriverObject->DriverExtension->AddDevice = Uusbd_PnPAddDevice; // uusbdpnp.c

	/*
	Windows98�ł́A���̃h���C�o�[�̉��w�̃h���C�o�[��PoCallDriver��
	�Ăт��ꂪ�������Ă��Ȃ����ɁAPoCallDrive���ĂԂƏ��߂�IRP���������Ȃ�
	�����h�����߁Amutex���g���V���A���C�Y����B
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

    IRP_MJ_SYSTEM_CONTROL�̃f�B�X�p�b�`���[�`��
	���ʂɂ��鏈���͂Ȃ��A���� PDO �ɏ�����`����̂݁B

Arguments:

    DeviceObject - pointer to FDO device object

    Irp          - pointer to an I/O Request Packet

Return Value:

	PDO�����Return Value�����̂܂ܕԂ�


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
    // �h���C�o�[���[�h���Ɉ�ԏゾ�����f�o�C�X�I�u�W�F�N�g
	//�i�����̉��̑w�̃f�o�C�X�I�u�W�F�N�g)�𓾂�
    stackDeviceObject = deviceExtension->TopOfStackDeviceObject;

    UUSBD_KdPrint( DBGLVL_HIGH, ( "enter Uusbd_ProcessSysControlIrp()\n") );

    Uusbd_IncrementIoCount(DeviceObject);

    UUSBD_ASSERT( IRP_MJ_SYSTEM_CONTROL == irpStack->MajorFunction );

    IoCopyCurrentIrpStackLocationToNext(Irp);

	// ���̑w�� IRP�𔭍s
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

    DriverEntry�ŃA���P�[�g�������\�[�X��S�ĉ������

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

  GUID����V���{���b�N�����N���𓾂ă����N���쐬����
  ���[�U�[���[�h����open,create���鎞�ɂ��̖��O���g�p����

Arguments:

    DeviceObject
		pointer to our Physical Device Object ( PDO )

    deviceLinkUnicodeString
		unicode��������i�[����ꏊ�B�Ăяo�������A���P�[�g���Ă����B
		��������΁A���̃f�o�C�X�C���^�[�t�F�[�X�I�u�W�F�N�g�̃V���{
		���b�N�����N�����i�[�����

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

***/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;


    //  Create the symbolic link
     
    // device interface��o�^���A�V���{���b�N�����N���𓾂�
	// ���̃����N���̓��[�U�[���[�h�A�v������SetupDi API���g���Ē��ׂ邱�Ƃ��ł���

    ntStatus = IoRegisterDeviceInterface(
                DeviceObject,
                (LPGUID)&GUID_CLASS_UUSBD,
                NULL,
                deviceLinkUnicodeString);

    UUSBD_KdPrintCond( DBGLVL_MEDIUM, (!(NT_SUCCESS(ntStatus))),
            ("FAILED to IoRegisterDeviceInterface()\n"));

   if (NT_SUCCESS(ntStatus)) {

		// �f�o�C�X�C���^�[�t�F�[�X��L���ɂ���
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

	Functional DeviceObject�����

Arguments:

    DriverObject
		�f�o�C�X�̃h���C�o�[�I�u�W�F�N�g�ւ̃|�C���^�[

	PhysicalDeviceObject
		�쐬����f�o�C�X�I�u�W�F�N�g�p��PDO

    DeviceObject
		�쐬�����f�o�C�X�I�u�W�F�N�g��Ԃ��|�C���^�[

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

	// �V���{���b�N�����N���𓾂�
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

        ntStatus = IoCreateDevice (DriverObject,		// �h���C�o�[�I�u�W�F�N�g
                           sizeof (DEVICE_EXTENSION),	// device extension�̑傫��
                           NULL,						// device object�̖��O
                           FILE_DEVICE_UNKNOWN,			// �f�o�C�X�^�C�v
                           FILE_AUTOGENERATED_DEVICE_NAME, //�f�o�C�X�̓���
                           FALSE,						// �r���t���O
                           DeviceObject);				// �쐬�����f�o�C�X�I�u�W�F�N�g

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

        // �V���{���b�N�����N����deviceExtension�ɃR�s�[���Ă���
        // Uusbd_RemoveDevice�ŃC���^�t�F�[�X��disable����Ƃ��Ɏg�p
        RtlCopyMemory(deviceExtension->DeviceLinkNameBuffer,
                      deviceLinkUnicodeString.Buffer,
                      deviceLinkUnicodeString.Length);


 		// �C�x���g�̏�����
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

    URB �����w�ɂ���USBD class driver�ɓn��
	USBD�ɂ͎��̂悤��IRP�����n��
	Irp->MajorFunction = set to IRP_MJ_INTERNAL_DEVICE_CONTROL
	Parameters.DeviceIoControl.IoControlCode =IOCTL_INTERNAL_USB_SUBMIT_URB
	Parameters.Others.Argument1 = Urb

Arguments:

    DeviceObject
		pointer to the physical device object (PDO)

    Urb
		�t�H�[�}�b�g���ꂽ�i�l���Z�b�g���ꂽ�jURB�ւ̃|�C���^�[

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

    // �����̂��߂̃C�x���g�����������Ă���
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_SUBMIT_URB,
                deviceExtension->TopOfStackDeviceObject, //�@USBD�̃f�o�C�X�I�u�W�F�N�g
                NULL, // optional input bufer; none needed here
                0,	  // input buffer len if used
                NULL, // optional output bufer; none needed here
                0,    // output buffer len if used
                TRUE, // IRP_MJ_INTERNAL_DEVICE_CONTROL, IRP_MJ_SCSI�̏ꍇ��TRUE
					  // IRP_MJ_DEVICE_CONTROL �̏ꍇ��FALSSE
                &event,     // �����������ɃV�O�i����Ԃɂ���C�x���g
                &ioStatus);  // ���N�G�X�g�������������ɃZ�b�g����I/O status block
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

		// ���N�G�X�g���������Ă��Ȃ��ꍇSTATUS_PENDING���A��
		// ���̏ꍇ�̓��N�G�X�g����������܂ő҂�
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

    �^����ꂽUSB�f�o�C�X���R���t�B�O���[�V��������

Arguments:

    DeviceObject
		���̃f�o�C�X��FDO�ւ̃|�C���^�[

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

	// �R���t�B�O���[�V�����f�B�X�N���v�^�[�͑傫�����ςȂ���
	// UsbBuildGetDescriptorRequest()���Ăяo�����ɕK�v�ȃ������[�ʂ�
	// ����Ȃ��B����512�o�C�g�Ƃ�
    siz = sizeof(USB_CONFIGURATION_DESCRIPTOR) + 512;  

	// UsbBuildGetDescriptorRequest()���Ăяo���A�������[�ʂ��s�����Ă�����
	// �������[���A���P�[�g���Ȃ����Ă�����x�Ăяo��
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

			// �������[�T�C�Y������Ȃ������ꍇ
			siz = deviceExtension->UsbConfigurationDescriptor->wTotalLength;
			UUSBD_ExFreePool(deviceExtension->UsbConfigurationDescriptor);
			deviceExtension->UsbConfigurationDescriptor = NULL;
		} else {
			break;  // we got it on the first try
		}

	} // end, while (retry loop )

    UUSBD_ExFreePool(urb);
	UUSBD_ASSERT( deviceExtension->UsbConfigurationDescriptor );

	// UsbConfigurationDescriptor������SelectInterface����
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

	USB�f�o�C�X�̑��݂���S�ẴC���^�t�F�[�X���g�p�\�ɂ���

Arguments:

    DeviceObject
		���̃f�o�C�X��FDO�ւ̃|�C���^�[

    ConfigurationDescriptor
		�R���t�B�O���[�V�����f�B�X�N���v�^�[�ւ̃|�C���^�[
		�C���^�[�t�F�[�X�f�B�X�N���v�^�[�A�G���h�|�C���g�f�B�X�N���v�^�[
		���܂܂�Ă��鎖

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

	// USB�f�o�C�X�̎��C���^�[�t�F�[�X�̐�
	num_interface = ConfigurationDescriptor->bNumInterfaces ;

	// �C���^�[�t�F�[�X�̐�+1�̑傫���̔z������
	// �Ō�̗v�f�͏I����\��NULL�Ƃ���@
	interfacelist = UUSBD_ExAllocatePool(NonPagedPool,sizeof(USBD_INTERFACE_LIST_ENTRY)*(num_interface+1)); // +1 for NULL entry
    if (!interfacelist) {
        UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_SelectInterface() USBD_CreateConfigurationRequest() failed\n  returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
	    return ntStatus; 
	}

	for(i=0;i<num_interface;i++){
		// ConfigurationDescriptor����InterfaceDescriptor�����o��
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

	// ���ۂ�ConfigurationDescriptor �ɑ��݂���interface�̐��ɏ㏑������
	num_interface = i;
	
    if (num_interface==0) {
        UUSBD_ExFreePool(interfacelist);
        UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_SelectInterface() USBD_CreateConfigurationRequest() failed\n  returning STATUS_INSUFFICIENT_RESOURCES\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
	    return ntStatus; 
	}

	// select a configuration �p��URB�����
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
			// select a configuration�����s����O�Ƀp�����[�^�[���C������
			interfacelist[j].Interface->Pipes[i].MaximumTransferSize = maxtransfer;
	        UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_SelectInterface() if=%d, pipe=%d MaximumTransferSize = %d\n", j,i,interfacelist[j].Interface->Pipes[i].MaximumTransferSize));
        }
	}
	}
	ntStatus = Uusbd_CallUSBD(DeviceObject, urb);

    if (NT_SUCCESS(ntStatus)) {
		// �C���^�[�t�F�[�X�̏���deviceExtension�ɕۑ�����

        deviceExtension->UsbConfigurationHandle =
            urb->UrbSelectConfiguration.ConfigurationHandle;

        deviceExtension->UsbNumInterface = num_interface;

		deviceExtension->UsbInterfaceInfo = UUSBD_ExAllocatePool(NonPagedPool,
			sizeof(UUSBD_INTERFACEINFO)*num_interface);

		UUSBD_KdPrint( DBGLVL_MEDIUM,("Found %d interface\n", num_interface));
		for(i=0;i<num_interface;i++){
			// �C���^�[�t�F�[�X�ɂ��Ă�loop

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

    �p�C�v�����Z�b�g����

    NOTES:

    This will reset the host to Data0 and should also reset the device to Data0 

Arguments:
	DeviceObject
		���Z�b�g����p�C�v�̃f�o�C�X��FDO�ւ̃|�C���^�[
	PipeInfo
		���Z�b�g����p�C�v

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

	�y���f�B���O����IO�̐���device extension�ɕۑ����Ă���
	IRP���󂯎��ƃJ�E���g�A�b�v���AIRP����������ƃJ�E���g�_�E������
	���̃J�E���^�[�̓f�o�C�X���ڑ����ꂽ�� 1 �ɂȂ�

	�f�o�C�X��remove�����͏�������IRP�������Ȃ��Ă���s���悤��
	IRP_MN_QUERY_REMOVE_DEVICE���������鎞�ɃJ�E���^�[���P
	�Ƀf�N�������g����Ă���܂ł܂B

Arguments:
	DeviceObject
		���Z�b�g����p�C�v�̃f�o�C�X��FDO�ւ̃|�C���^�[

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
		// 1�Ƀf�N�������g���ꂽ���y���f�B���O����IRP���Ȃ��Ȃ���
		// ���Ƃ�\��
        KeSetEvent(&deviceExtension->NoPendingIoEvent,
                   1,
                   FALSE);
    }

    if (ioCount==0) {
		// 0�Ƀf�N�������g���ꂽ�������[�u�C�x���g���������Ă悢���Ƃ�\��
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

	�y���f�B���O����IO�̐���device extension�ɕۑ����Ă���
	IRP���󂯎��ƃJ�E���g�A�b�v���AIRP����������ƃJ�E���g�_�E������

Arguments:
	DeviceObject
		���Z�b�g����p�C�v�̃f�o�C�X��FDO�ւ̃|�C���^�[

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
		// 2�ɃC���N�������g���ꂽ���y���f�B���O����IRP������
		// ���Ƃ�\��
        KeClearEvent(&deviceExtension->NoPendingIoEvent);
    }
	KeReleaseSpinLock (&deviceExtension->IoCountSpinLock, oldIrql);

    UUSBD_KdPrint( DBGLVL_HIGH,("Exit Uusbd_IncrementIoCount() Pending io count = %x\n", deviceExtension->PendingIoCount));
}


