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

    Plug and Play IRP�̏������[�`��.

Arguments:
	DeviceObject
		�f�o�C�X��FDO�ւ̃|�C���^�[

    Irp
		I/O Request Packet�ւ̃|�C���^�[

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
    // ���݂�IO_STACK_LOCATION�𓾂�
    // ������ function codes��p�����[�^�[������
    //
    irpStack = IoGetCurrentIrpStackLocation (Irp);

    //
    // Get a pointer to the device extension
    //

    deviceExtension = DeviceObject->DeviceExtension;
    stackDeviceObject = deviceExtension->TopOfStackDeviceObject;

    UUSBD_KdPrint( DBGLVL_MEDIUM, ( "enter Uusbd_ProcessPnPIrp() IRP_MJ_PNP, minor %s\n",
        UUSBD_StringForPnpMnFunc( irpStack->MinorFunction ) ));

	// count���C���N�������g����
    Uusbd_IncrementIoCount(DeviceObject);

    UUSBD_ASSERT( IRP_MJ_PNP == irpStack->MajorFunction );

    switch (irpStack->MinorFunction) {
    case IRP_MN_START_DEVICE:

		// PnP�}�l�[�W���̓��\�[�X�����蓖�Ă���IRP_MN_START_DEVICE
		// �𑗂�B
		// �܂����w�ɂ���IRP�𑗂�

		// ���w��������������̂�m�点��C�x���g�����������Ă���
        KeInitializeEvent(&startDeviceEvent, NotificationEvent, FALSE);
        IoCopyCurrentIrpStackLocationToNext(Irp);

        // ���w��IRP�̏�����������������Uusbd_IrpCompletionRoutine���Ă΂��
		// �悤�ɂ���.���̊֐��̒��� startDeviceEvent���V�O�i����Ԃɂ���
        IoSetCompletionRoutine(Irp,
                               Uusbd_IrpCompletionRoutine,	//�������[�`��
                               &startDeviceEvent,			//�������[�`�� Context����
                               TRUE,    // invoke on success
                               TRUE,    // invoke on error
                               TRUE);   // invoke on cancellation


        // ���w��IRP������������
        ntStatus = IoCallDriver(stackDeviceObject,
                                Irp);

        // if PDO is not done yet, wait for the event to be set in our completion routine
        if (ntStatus == STATUS_PENDING) {
	        // ���w�̏������������ĂȂ��ꍇ��������܂ő҂B
			// ��������startDeviceEvent���V�O�i����ԂɂȂ�
            waitStatus = KeWaitForSingleObject(
                &startDeviceEvent,
                Suspended,
                KernelMode,
                FALSE,
                NULL);
			ntStatus = Irp->IoStatus.Status;
        }
		if (NT_SUCCESS(ntStatus)) {

		//���̎��_�Ńf�o�C�X���g�p�\�ɂȂ�
		//�����������Ȃǂ��s��
	        ntStatus = Uusbd_StartDevice(DeviceObject);
	        Irp->IoStatus.Status = ntStatus;
		}
		IoCompleteRequest (Irp,
						   IO_NO_INCREMENT
						   );

		Uusbd_DecrementIoCount(DeviceObject);
		return ntStatus;  // end, case IRP_MN_START_DEVICE

    case IRP_MN_QUERY_STOP_DEVICE:

		// STOP_DEVICE�̗v��
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


        // read/write IRP��҂��Ă���ꍇ�A���ۂ���
        if( SumStagedPendingIrpCount(deviceExtension) ) {
            ntStatus = STATUS_UNSUCCESSFUL;
        }
        else { 
			// ���ۂ��Ȃ�
			// ����Ȍ�A�V����IOCTL �� read/write���Ȃ��悤�Ƀt���O�𗧂Ă�
             deviceExtension->StopDeviceRequested = TRUE;
			 Irp->IoStatus.Status = STATUS_SUCCESS;
        }

        break; // end, case IRP_MN_QUERY_STOP_DEVICE

    case IRP_MN_CANCEL_STOP_DEVICE:

		// PnP�}�l�[�W���[�͒�~�ł��Ȃ��f�o�C�X�����������A
		// ����IRP�𑗂��~�v�����L�����Z�����邱�Ƃ�m�点��
		// ����IRP��IRP_MN_QUERY_STOP_DEVICE�����������f�o�C�X�ɑ΂��Ă̂ݑ�����


        // It is possible to receive this irp when the device has not been started
        if (!deviceExtension->DeviceStarted) { // �܂��X�^�[�g���ĂȂ����́A���w�Ƀp�X����̂�
            UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPnPIrp() IRP_MN_CANCEL_STOP_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            Uusbd_DecrementIoCount(DeviceObject);
            return ntStatus;
        }

		// �Ă�IOCTL �� read/write�������\�ɂ���
        deviceExtension->StopDeviceRequested = FALSE;
		Irp->IoStatus.Status = STATUS_SUCCESS;
        break; // end, case IRP_MN_CANCEL_STOP_DEVICE

    case IRP_MN_STOP_DEVICE:

		// PnP Manager�̓f�o�C�X���~�����鎞�ɂ���IRP�𑗂�
		// ��~���������ƃn�[�h�E�G�A���\�[�X�̍č����O���[�V������
		// �\�ɂȂ�
		// IRP_MN_QUERY_STOP_DEVICE�����������Ƃ��݂̂���IRP�𑗂�

        ntStatus = Uusbd_StopDevice(DeviceObject, TRUE);

		Irp->IoStatus.Status = ntStatus;
        break; // end, case IRP_MN_STOP_DEVICE



    case IRP_MN_QUERY_REMOVE_DEVICE:

		//����IRP�͈��S�Ɂi�V�X�e���Ɉ��e�������j�f�o�C�X�����O��
		//���Ƃ��ł��邩�ǂ����₢���킹�邽�߂Ɏg����
		//�����h���C�o�[���f�o�C�X�����O���Ă����S���Ɣ��f������
		//�܂��I����Ă��Ȃ�IO���N�G�X�g�����������A����ȍ~��read/write
		//���N�G�X�g����߂�STATUS_SUCCESS��Ԃ��B
		//���̏ꍇ�AIRP_MN_REMOVE_DEVICE�̏������m���ɐ�������悤��
		//�Ȍ�V������������Ă͂����Ȃ�

		//�������O���Ȃ��Ɣ��f�����ꍇ�A���̃f�o�C�X�X�^�b�N��
		//IRP_MN_QUERY_REMOVE_DEVICE�𑗂��Ă͂����Ȃ��B
		//��������IRP�����s�����ꍇ�APnP�}�l�[�W���[�́A
		//IRP_MN_CANCEL_REMOVE_DEVICE���f�o�C�X�X�^�b�N�ɑ�����
		//REMOVE_DEVICE�̗v�����L�����Z������

        // It is possible to receive this irp when the device has not been started
        if (!deviceExtension->DeviceStarted) { // if get when never started, just pass on
            UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPnPIrp() IRP_MN_QUERY_STOP_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            Uusbd_DecrementIoCount(DeviceObject);

            return ntStatus;
        }

		// ����Ȍ�A�V����IOCTL �� read/write��IRP�����w�̃h���C�o�[�ɓn��Ȃ�
		// �悤�ɂ悤�Ƀt���O�𗧂Ă�
       deviceExtension->RemoveDeviceRequested = TRUE;

		// ���̃��[�`���̏��߂ŃC���N�������g���Ă���̂ňꎞ�I�Ƀf�N�������g����
		Uusbd_DecrementIoCount(DeviceObject);

        // �y���f�B���O����IO���N�G�X�g������΂��ꂪ�I���܂ł܂�
		// deviceExtension->PendingIoCount��1�ɂȂ������ANoPendingIoEvent
		// ���V�O�i����ԂɂȂ�
        waitStatus = KeWaitForSingleObject(
                    &deviceExtension->NoPendingIoEvent,
                    Suspended,
                    KernelMode,
                    FALSE,
                    NULL);

		// �ꎞ�I�ȃf�N�������g��߂�		
		Uusbd_IncrementIoCount(DeviceObject);

		Irp->IoStatus.Status = STATUS_SUCCESS;
        break; // end, case IRP_MN_QUERY_REMOVE_DEVICE

    case IRP_MN_CANCEL_REMOVE_DEVICE:

		// PnP�}�l�[�W���������[�u�v�����������Ƃ�����IRP���g��
		// IRP_MN_QUERY_REMOVE_DEVICE�����������Ƃ��݂̂���IRP�͗���

        if (!deviceExtension->DeviceStarted) { // if get when never started, just pass on
            UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPnPIrp() IRP_MN_CANCEL_REMOVE_DEVICE when device not started\n"));
            IoSkipCurrentIrpStackLocation (Irp);
            ntStatus = IoCallDriver (deviceExtension->TopOfStackDeviceObject, Irp);
            Uusbd_DecrementIoCount(DeviceObject);
            return ntStatus;
        }

		// �ĂсA�V����IOCTL �� IO Irp ���󂯂���悤�ɂ���
        deviceExtension->RemoveDeviceRequested = FALSE;

		Irp->IoStatus.Status = STATUS_SUCCESS;
        break; // end, case IRP_MN_CANCEL_REMOVE_DEVICE

	case IRP_MN_SURPRISE_REMOVAL:
        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPnPIrp() IRP_MN_SURPRISE_REMOVAL\n"));
		
		// �ˑR�f�o�C�X�����O���ꂽ�ꍇ 
		// ���̏ꍇPnP�}�l�[�W���[��IRP_MN_QUERY_REMOVE_DEVICE��
		// ���邱�ƂȂ����ۂ̃f�o�C�X�����Ɏ��O����Ă���

        // match the inc at the begining of the dispatch routine
        Uusbd_DecrementIoCount(DeviceObject);

		//
        // ����ȍ~�A�V����IOCTL��read/write irp�����w�̃h���C�o�[�ɓn��Ȃ�
		// �悤�Ƀt���O�𗧂Ă�
        deviceExtension->DeviceRemoved = TRUE;

		if(deviceExtension->DeviceStarted){
			Uusbd_StopDevice(DeviceObject, FALSE);
		}

		Irp->IoStatus.Status = STATUS_SUCCESS;
		// We don't explicitly wait for the below driver to complete, but just make
		// the call and go on, finishing cleanup
		// ���w�̃h���C�o�[����IRP�𑗂�
        IoCopyCurrentIrpStackLocationToNext(Irp);

        ntStatus = IoCallDriver(stackDeviceObject,
                                Irp);

		return ntStatus;

    case IRP_MN_REMOVE_DEVICE:

		// �f�o�C�X�����O���� PnP Manager�͂���IRP�𔭍s����
		// �\�����ꂽ���O���̏ꍇ�A���O��IRP_MN_QUERY_REMOVE_DEVICE
		// �������f�o�C�X�̓����[�u��҂�ԂɂȂ��Ă���B
		// �������\������Ȃ��ꍇ�i�R�[�h�𔲂����ꍇ���j�Ȃ�̑O�G��
		// ���Ȃ����ۂ̃f�o�C�X���̂��̂����O����Ă���B
		// ���̏ꍇ�́AIRP_MN_QUERY_REMOVE_DEVICE�͑����Ȃ��B

        // match the inc at the begining of the dispatch routine
        Uusbd_DecrementIoCount(DeviceObject);

		//
        // ����ȍ~�A�V����IOCTL��read/write irp�����w�̃h���C�o�[�ɓn��Ȃ�
		// �悤�Ƀt���O�𗧂Ă�
        deviceExtension->DeviceRemoved = TRUE;

		if(deviceExtension->DeviceStarted){
			Uusbd_StopDevice(DeviceObject, TRUE);
		}
		// We don't explicitly wait for the below driver to complete, but just make
		// the call and go on, finishing cleanup
		// ���w�̃h���C�o�[����IRP�𑗂�
        IoCopyCurrentIrpStackLocationToNext(Irp);

        ntStatus = IoCallDriver(stackDeviceObject,
                                Irp);
		// PendingIoCount���f�N�������g����B
		// ����ŁA�y���f�B���O����IRP�������Ȃ�������PendingIoCount��0�ɂȂ�
		// 0�ɂȂ������ɁARemoveEvent���V�O�i����ԂɂȂ�
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

		// �G���[���������ꍇ�A���w��IRP�𑗂炸�I������
        Irp->IoStatus.Status = ntStatus;
		IoCompleteRequest (Irp,
						   IO_NO_INCREMENT
						   );

		Uusbd_DecrementIoCount(DeviceObject);

		UUSBD_KdPrint( DBGLVL_MAXIMUM,("Uusbd_ProcessPnPIrp() Exit Uusbd_ProcessPnPIrp FAILURE %x\n", ntStatus));
		return ntStatus;
    }

    //
    // �S�Ă� PNP_POWER messages�͉��w�̃f�o�C�X�I�u�W�F�N�g�iTopOfStackDeviceObject�j
	// �ɑ���
	// IRP_MN_START_DEVICE�̏ꍇ�͉��w�ɑ����Ă���A���g�̃f�o�C�X�Ɋւ��鏈����
	// �s���̂ŁA�ȉ��̃��[�`���͎g�p���Ȃ��B
	// ���O���̏ꍇ�͎��g�̃f�o�C�X�̏��������Ă���ȉ��̃��[�`�����g����
	// ���w��IRP��������
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

	Functional Device Object (FDO)���쐬���������鎞�ɂ�т������.

Arguments:

    DriverObject
		UUSBD�̃f�o�C�X�I�u�W�F�N�g

    PhysicalDeviceObject
		���̃f�o�C�X�X�^�b�N��PDO�i�o�X�h���C�o�[�ɂ���č����j

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

    // funtional device object (FDO)�����
    ntStatus =
        Uusbd_CreateDeviceObject(DriverObject, PhysicalDeviceObject, &deviceObject);

    if (NT_SUCCESS(ntStatus)) {
        deviceExtension = deviceObject->DeviceExtension;

//        deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

        // read/write���A���[�U�[���[�h�������[��
		// ���ڃf�[�^�[������ direct io ���T�|�[�g���� 
        deviceObject->Flags |= DO_DIRECT_IO;


		// �T�X�y���h���A���W���[������IRP_MN_STOP_DEVICE,IRP_MN_START_DEVICE
		// ��IRP���N���Ȃ��悤�ɂ���
		// IRP_MN_START_DEVICE��GetDescriptors()�����s����̂�h�����߁B
        deviceObject->Flags |= DO_POWER_PAGABLE;
        // device extension�̏�����
        //
        // Physical device Object���o���Ă���
        deviceExtension->PhysicalDeviceObject=PhysicalDeviceObject;

        //
        // PDO�ɃA�^�b�`����B
        // �A�^�b�`�������Ɉ�ԏ�ɂ������f�o�C�X��
		// TopOfStackDeviceObject�Ɋo���Ă���
		// ���ꂪ�����̉��w�ɂȂ�B

        deviceExtension->TopOfStackDeviceObject =
            IoAttachDeviceToDeviceStack(deviceObject, PhysicalDeviceObject);

		// device extension��DEVICE_CAPABILITIES�Ƀf�o�C�X�̕����I������
		// �R�s�[����
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

    Uusbd_ProcessPnPIrp()�̒�����Ăяo�����B
	IRP�̎�ނ�IRP_MN_START_DEVICE�ŁA���w�ɂ���IRP�̏�����n��
	���w���X�^�[�g�������Ƃ��̊֐����Ă΂��B
	���̎��_�ŁAURB����艺�w�ɓn�����Ƃ��ł���B
	�����ŁA�R���t�B�O���[�V�������s��

Arguments:
    DriverObject
		UUSBD�̃f�o�C�X�I�u�W�F�N�g

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

	Uusbd_StartDevice�ŃA���P�[�g�����������[���������

Arguments:

	DeviceObject
		Uusbd�̃f�o�C�X�I�u�W�F�N�g

Return Value:

    ����

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

	�f�o�C�X�������[�u����B
	�A���P�[�g�����������[��A�V���{���b�N�����N�����������

Arguments:

	DeviceObject
		UUSBD�̃f�o�C�X�I�u�W�F�N�g

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

	USB�f�o�C�X���A���R���t�B�O���[�V�����̏�Ԃɂ���

Arguments:

	DeviceObject
		UUSBD�̃f�o�C�X�I�u�W�F�N�g

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

	// �R���t�B�O���[�V�����n���h����NULL�ɂ���Select configuration���Ăяo��
	// USB�f�o�C�X��'unconfigured'state�ɂȂ�

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
	// ���N�G�X�g����IRP���L�����Z������
	Uusbd_CancelPendingIo( DeviceObject ); 
	
	// �I�[�v�����Ă���p�C�v������ꍇ�A
	// USBD �� URB_FUNCTION_ABORT_PIPE�𑗂�A�p�C�v���N���[�Y����
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

	// �����ʒm���[�`��
	// IRP�����������Ƃ���Context�����ɂ킽���ꂽ�C�x���g���V�O�i�����
	// �ɂ���
	// ���̃��[�`���́APnP �� Power Management�̃��[�`���Ŏg�p����

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

    // ����IRP��IoFreeIrp()���Ăяo���ĂȂ��̂� STATUS_MORE_PROCESSING_REQUIRED
	// ��Ԃ��B
    return STATUS_MORE_PROCESSING_REQUIRED;

}


