/***************************************************

Copyright (c) 1999  kashiwano masahiro

Abstract:

    UUSBD.SYS Ioctl function
    
****************************************************/


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "uusbd.h"

#include "uusbdioctl.h"
#include "usbdlib.h"
#include "stddef.h"


NTSTATUS
Uusbd_ProcessIOCTL(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/***

Routine Description:

    IRP_MJ_DEVICE_CONTROL���f�B�X�p�b�`����; 
    ���[�U�[���[�h���� DeviceIoControl() ���Ă΂ꂽ���̏���


Arguments:

    DriverObject
		�f�o�C�X�̃h���C�o�[�I�u�W�F�N�g�ւ̃|�C���^�[

Return Value:

    NT status code

***/
{
    PIO_STACK_LOCATION irpStack;
    PVOID ioBuffer;
    ULONG inputBufferLength;
    ULONG outputBufferLength;
    PDEVICE_EXTENSION deviceExtension;
    ULONG ioControlCode;
    NTSTATUS ntStatus;
    ULONG length;
    PUCHAR pch;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("IRP_MJ_DEVICE_CONTROL\n"));

    Uusbd_IncrementIoCount(DeviceObject);

    //
    // Get a pointer to the current location in the Irp. This is where
    //     the function codes and parameters are located.
    //

    deviceExtension = DeviceObject->DeviceExtension;
    
	// ���̂����ꂩ�̏ꍇ��IO���N�G�X�g���󂯂��Ȃ�
	// 1) �f�o�C�X�����O���ꂽ
	// 2) �܂��J�n����Ă��Ȃ�
	// 3) ��~���Ă���
	// 4) �����[�u���N�G�X�g��҂��Ă���
	// 5) �X�g�b�v���N�G�X�g��҂��Ă���
    if ( !Uusbd_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        Irp->IoStatus.Status = ntStatus;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest( Irp, IO_NO_INCREMENT );

        Uusbd_DecrementIoCount(DeviceObject);                          
        return ntStatus;
    }

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    // get pointers and lengths of the caller's (user's) IO buffer
    ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

    // ���[�U�[���[�h�����IOCTL����������
    switch (ioControlCode) {

    case IOCTL_UUSBD_RESET_PIPE:
        {
            PUUSBD_PIPE pipe;
                PFILE_OBJECT fileObject;

                    // get our context and see if it is a pipe
                fileObject = irpStack->FileObject;

                    pipe = (PUUSBD_PIPE) fileObject->FsContext;    

                    if(pipe == NULL) {
                            // error, this is not a pipe
                    ntStatus =
                            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
                    } else {            
                Uusbd_ResetPipe(DeviceObject, &pipe->PipeInfo );
            
                ntStatus = Irp->IoStatus.Status = STATUS_SUCCESS;
            }
        }
        break;


     case IOCTL_UUSBD_GET_CONFIG_DESCRIPTOR:

        //
        // This api returns a copy of the configuration descriptor
        // and all endpoint/interface descriptors.
        //

        //
        // inputs  - none
        // outputs - configuration descriptor plus interface
        //          and endpoint descriptors
        //

        pch = (PUCHAR) ioBuffer;

        configurationDescriptor =
            deviceExtension->UsbConfigurationDescriptor;

        if (configurationDescriptor) {
            
            length = configurationDescriptor->wTotalLength;

            if (outputBufferLength >= length) {

                RtlCopyMemory(pch,
                              (PUCHAR) configurationDescriptor,
                              length);
                
                Irp->IoStatus.Information = length;
                
                ntStatus = Irp->IoStatus.Status = STATUS_SUCCESS;
            }
            else {
        
                Irp->IoStatus.Information = 0;
    
                ntStatus = Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            }    
        }
        else {

            Irp->IoStatus.Information = 0;

            ntStatus = Irp->IoStatus.Status = STATUS_DEVICE_DATA_ERROR;
        }

        break;

     case IOCTL_UUSBD_RESET_DEVICE:
        
                ntStatus = Uusbd_ResetDevice( DeviceObject );
        break;               

     case IOCTL_UUSBD_VENDORREQUEST_IN:
		ntStatus = Uusbd_VendorRequest_in(
			DeviceObject,
			ioBuffer,
			inputBufferLength,
			ioBuffer,
			outputBufferLength,
			&(Irp->IoStatus.Information));
        break;               

     case IOCTL_UUSBD_VENDORREQUEST_OUT:
		ntStatus = Uusbd_VendorRequest_out(
			DeviceObject,
			ioBuffer,
			inputBufferLength,
			ioBuffer,
			outputBufferLength,
			&(Irp->IoStatus.Information));
        break;               

     case IOCTL_UUSBD_GETDESCRIPTOR:
		ntStatus = Uusbd_GetDescriptor(
			DeviceObject,
			ioBuffer,
			inputBufferLength,
			ioBuffer,
			outputBufferLength,
			&(Irp->IoStatus.Information));
        break;               
		
	 case IOCTL_UUSBD_CHECK:
		ntStatus = Uusbd_Check(
			DeviceObject,
			ioBuffer,
			inputBufferLength,
			ioBuffer,
			outputBufferLength,
			&(Irp->IoStatus.Information));
		break;

	 case IOCTL_UUSBD_GETSTATUS:
		ntStatus = Uusbd_GetStatus(
			DeviceObject,
			ioBuffer,
			inputBufferLength,
			ioBuffer,
			outputBufferLength,
			&(Irp->IoStatus.Information));
		break;

	 default:
        ntStatus =
            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
    }

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

    Uusbd_DecrementIoCount(DeviceObject);                       

    return ntStatus;

}




NTSTATUS
Uusbd_ResetDevice(
    IN PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:
		�f�o�C�X�����Z�b�g����

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.


Return Value:

    NT status code

***/
{
    NTSTATUS ntStatus;
    ULONG portStatus;

    UUSBD_KdPrint(5,("Enter Uusbd_ResetDevice()\n"));
    
    ntStatus = Uusbd_ResetParentPort(DeviceObject);
	return ntStatus;
}



NTSTATUS
Uusbd_GetPortStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PULONG PortStatus
    )
/***

Routine Description:

    returns the port status for our device

Arguments:

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

***/
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;
    PDEVICE_EXTENSION deviceExtension;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("enter Uusbd_GetPortStatus\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    *PortStatus = 0;

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    // IoBuildDeviceIoControlRequest allocates and sets up an IRP for a device control request
    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_GET_PORT_STATUS,
                deviceExtension->TopOfStackDeviceObject, //next-lower driver's device object, representing the target device.
                NULL, // no input or output buffers
                0,
                NULL,
                0,
                TRUE, // internal ( use IRP_MJ_INTERNAL_DEVICE_CONTROL )
                &event, // event to be signalled on completion ( we wait for it below )
                &ioStatus);

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    // IoGetNextIrpStackLocation gives a higher level driver access to the next-lower 
    // driver's I/O stack location in an IRP so the caller can set it up for the lower driver.
    nextStack = IoGetNextIrpStackLocation(irp);
    UUSBD_ASSERT(nextStack != NULL);

    nextStack->Parameters.Others.Argument1 = PortStatus;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_GetPortStatus() calling USBD port status api\n"));

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject,
                            irp);

    UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_GetPortStatus() return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

        UUSBD_KdPrint( DBGLVL_DEFAULT,("Wait for single object\n"));

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

        UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_GetPortStatus() Wait for single object, returned %x\n", status));
        
    } else {
        ioStatus.Status = ntStatus;
    }

    UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_GetPortStatus() Port status = %x\n", *PortStatus));

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("Exit Uusbd_GetPortStatus (%x)\n", ntStatus));


    return ntStatus;
}


NTSTATUS
Uusbd_ResetParentPort(
    IN IN PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

    Reset the our parent port

Arguments:

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

***/
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;
    PDEVICE_EXTENSION deviceExtension;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("enter Uusbd_ResetParentPort\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_RESET_PORT,
                deviceExtension->TopOfStackDeviceObject,
                NULL,
                0,
                NULL,
                0,
                TRUE, // internal ( use IRP_MJ_INTERNAL_DEVICE_CONTROL )
                &event,
                &ioStatus);

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    nextStack = IoGetNextIrpStackLocation(irp);
    UUSBD_ASSERT(nextStack != NULL);

    UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_ResetParentPort() calling USBD enable port api\n"));

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject,
                            irp);
                            
    UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_ResetParentPort() return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

        UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_ResetParentPort() Wait for single object\n"));

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

        UUSBD_KdPrint( DBGLVL_DEFAULT,("Uusbd_ResetParentPort() Wait for single object, returned %x\n", status));
        
    } else {
        ioStatus.Status = ntStatus;
    }

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("Exit Uusbd_ResetPort (%x)\n", ntStatus));

    return ntStatus;
}

//
// �f�[�^�|�o�͂𔺂��iUSBD_TRANSFER_DIRECTION_IN�łȂ��j
// �x���_�[���N�G�X�g�𔭍s���邽�߂�URB����艺�w�̃h��
// �C�o�[���Ăяo��
//
//�@����	inbuff		�x���_�[���N�G�X�g�̃p�����[�^
//			inbuffsize	sizeof(PARAM_VENDORREQUEST)+���M�f�[�^�T�C�Y
//			outbuff		���g�p
//			outbuffsize ���g�p
//			ret			�ǂݍ��݃o�C�g��Ԃ����ϐ��ւ̃|�C���^
//						*ret �� 0��������
//  �߂�l	STATUS_SUCCESS
//						����I��
//			���̑�		�ُ�I��
//
NTSTATUS
Uusbd_VendorRequest_out(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret)
{
	PARAM_VENDOR_REQUEST *p;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;
	int datalen;

	p = (PARAM_VENDOR_REQUEST *)inbuff;

    UUSBD_KdPrint(5,("Enter Uusbd_VendorRequest()\n"));

	if(inbuffsize < sizeof(PARAM_VENDOR_REQUEST)) return STATUS_INVALID_PARAMETER;
	datalen = inbuffsize - sizeof(PARAM_VENDOR_REQUEST);

	siz = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	urb = ExAllocatePool(NonPagedPool, siz);

	UsbBuildVendorRequest(
		urb,				//IN PURB Urb
		p->Function ,		//IN USHORT Function
		(USHORT)siz,		//IN USHORT Length
		p->TransferFlags & USBD_SHORT_TRANSFER_OK ,	//IN ULONG TransferFlags
		p->ReservedBits,	//IN UCHAR ReservedBits
		p->Request,			//IN UCHAR Request
		p->Value,			//IN USHORT Value
		p->Index,			//IN USHORT Index
		((char*)p) + sizeof(PARAM_VENDOR_REQUEST),
							//IN PVOID TransferBuffer     
		NULL,				//IN PMDL TransferBufferMDL 
		datalen,			//IN ULONG TransferBufferLength
		NULL,				//IN PURB Link
       );
    
	ntStatus = Uusbd_CallUSBD(DeviceObject, urb);
	*ret = datalen;

    ExFreePool(urb);

	return ntStatus;
}

//
// USBD_TRANSFER_DIRECTION_IN�̃x���_�[���N�G�X�g��
// ���s���邽�߂�URB����艺�w�̃h���C�o�[���Ăяo��
//
//�@����	inbuff		�x���_�[���N�G�X�g�̃p�����[�^
//			inbuffsize	sizeof(PARAM_VENDORREQUEST)
//			outbuff		�f�o�C�X����̎�M�f�[�^�|�i�[�̈�
//			outbuffsize �f�[�^�|�T�C�Y
//			ret			outbuffer�ɂ���f�[�^�̃o�C�g��Ԃ����ϐ��ւ̃|�C���^
//						����I�����@*ret = outbuffsize
//
//  �߂�l	STATUS_SUCCESS	����I��
//			���̑�			�ُ�I��
//
NTSTATUS
Uusbd_VendorRequest_in(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret)
{
	PARAM_VENDOR_REQUEST *p;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;

	p = (PARAM_VENDOR_REQUEST *)inbuff;

    UUSBD_KdPrint(5,("Enter Uusbd_VendorRequest()\n"));

	if(inbuffsize < sizeof(PARAM_VENDOR_REQUEST)) {
		return STATUS_INVALID_PARAMETER;
	}
	siz = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	urb = ExAllocatePool(NonPagedPool, siz);

	UsbBuildVendorRequest(
		urb,				//IN PURB Urb
		p->Function ,		//IN USHORT Function
		(USHORT)siz,		//IN USHORT Length
		USBD_TRANSFER_DIRECTION_IN | (p->TransferFlags & USBD_SHORT_TRANSFER_OK),
							//IN ULONG TransferFlags
		p->ReservedBits,	//IN UCHAR ReservedBits
		p->Request,			//IN UCHAR Request
		p->Value,			//IN USHORT Value
		p->Index,			//IN USHORT Index
		outbuff,			//IN PVOID TransferBuffer     
		NULL,				//IN PMDL TransferBufferMDL 
		outbuffsize,		//IN ULONG TransferBufferLength
		NULL,				//IN PURB Link
       );
    
	ntStatus = Uusbd_CallUSBD(DeviceObject, urb); // call USBD

    ExFreePool(urb);

	if(STATUS_SUCCESS == ntStatus) {
		*ret = outbuffsize;
	} else {
		*ret = 0;
	}
	return ntStatus;
}

//
// DESCRIPTOR�𓾂�
//
//�@����	inbuff		PARAM_GET_DESCRIPTOR* �p�����[�^
//			inbuffsize	sizeof(PARAM_GET_DESCRIPTOR)
//			outbuff		�f�o�C�X����̎�M�f�[�^�|�i�[�̈�
//			outbuffsize ��M�̈�T�C�Y�B�i�[����̂ɏ\���ȑ傫����p�ӂ���B
//						����Ȃ��ꍇ�́A����I�����Ȃ�
//			ret			���ۂɓǂݍ��񂾃o�C�g��Ԃ����ϐ��ւ̃|�C���^
//
//  �߂�l	STATUS_SUCCESS	����I��
//			���̑�			�ُ�I��
//
NTSTATUS
Uusbd_GetDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret)
{
	PARAM_GET_DESCRIPTOR *p;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;

	p = (PARAM_GET_DESCRIPTOR *)inbuff;

    UUSBD_KdPrint(5,("Enter Uusbd_GetDescriptor()\n"));

	if(inbuffsize != sizeof(PARAM_GET_DESCRIPTOR))
		return STATUS_INVALID_PARAMETER;

	siz = sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST );
	urb = ExAllocatePool(NonPagedPool, siz);

	if(p->DescriptorType != USB_STRING_DESCRIPTOR_TYPE){
		p->LanguageId = 0;
	}

	UsbBuildGetDescriptorRequest(
		urb,				//IN PURB Urb
		(USHORT)siz,		//IN USHORT Length
		p->DescriptorType,	//IN UCHAR DescriptorType
		p->Index,			//IN UCHAR Index	
		p->LanguageId,		//IN USHORT LanguageId,
		outbuff,			//IN PVOID TransferBuffer,   / OPTIONAL /
		NULL,				//IN PMDL TransferBufferMDL, / OPTIONAL /
		outbuffsize,		//IN ULONG TransferBufferLength,
		NULL				//IN PURB Link               / OPTIONAL /
       );
    
	ntStatus = Uusbd_CallUSBD(DeviceObject, urb); // call USBD

    ExFreePool(urb);
	*ret = urb->UrbControlDescriptorRequest.TransferBufferLength;
//    deviceExtension = DeviceObject->DeviceExtension;

//	RtlCopyMemory(outbuff, deviceExtension->debug, outbuffsize);

	return ntStatus;
}

//
// �K����������IOCTL
// �h���C�o�[����m�F�p
// CHECK_OK IOCTL
//
//  �߂�l	STATUS_SUCCESS	����I��
//
NTSTATUS
Uusbd_Check(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret)
{
	ULONG *p;
	p = (ULONG*)outbuff;
	if(outbuffsize != sizeof(ULONG)){
		return STATUS_INVALID_PARAMETER;
	}

	if(Uusbd_CanAcceptIoRequests(DeviceObject)){
		*p = 1;
	} else {
		*p = 0;
	}
	*ret = sizeof(ULONG);

	return STATUS_SUCCESS;
}

//
// Get Status
//
//�@����	inbuff		PARAM_GET_STATUS* �p�����[�^
//			inbuffsize	sizeof(PARAM_GET_STATUS)
//			outbuff		�f�o�C�X����̎�M�f�[�^�|�i�[�̈� 2
//			outbuffsize ��M�̈�T�C�Y�B
//			ret			���ۂɓǂݍ��񂾃o�C�g��Ԃ����ϐ��ւ̃|�C���^
//
//  �߂�l	STATUS_SUCCESS	����I��
//			���̑�			�ُ�I��
//
NTSTATUS
Uusbd_GetStatus(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret)
{
	PARAM_GET_STATUS *p;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;
	USHORT type;
	p = (PARAM_GET_STATUS *)inbuff;

    UUSBD_KdPrint(5,("Enter Uusbd_GetStatus()\n"));

	if(inbuffsize != sizeof(PARAM_GET_STATUS))
		return STATUS_INVALID_PARAMETER;
	if(outbuffsize != sizeof(USHORT))
		return STATUS_INVALID_PARAMETER;

	siz = sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST);
	urb = ExAllocatePool(NonPagedPool, siz);

	switch(p->type){
	case GET_STATUS_FROM_DEVICE:
		type = URB_FUNCTION_GET_STATUS_FROM_DEVICE ;
		break;
	case GET_STATUS_FROM_INTERFACE:
		type = URB_FUNCTION_GET_STATUS_FROM_INTERFACE  ;
		break;
	case GET_STATUS_FROM_ENDPOINT:
		type = URB_FUNCTION_GET_STATUS_FROM_ENDPOINT ;
		break;
	case GET_STATUS_FROM_OTHER:
	default:
		type = URB_FUNCTION_GET_STATUS_FROM_OTHER ;
		break;
	}
	UsbBuildGetStatusRequest( 
		urb,				//IN PURB Urb
		type,				//IN UCHAR DescriptorType
		p->Index,			//IN UCHAR Index	
		outbuff,			//IN PVOID TransferBuffer,   / OPTIONAL /
		NULL,				//IN PMDL TransferBufferMDL, / OPTIONAL /
		NULL				//IN PURB Link               / OPTIONAL /
       );
    
	ntStatus = Uusbd_CallUSBD(DeviceObject, urb); // call USBD

    ExFreePool(urb);
	*ret = urb->UrbControlDescriptorRequest.TransferBufferLength;

	return ntStatus;
}


