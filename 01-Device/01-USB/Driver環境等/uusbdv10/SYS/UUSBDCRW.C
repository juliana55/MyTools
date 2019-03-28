/***************************************************

Copyright (c) 1999  kashiwano masahiro

Abstract:

    UUSBD.SYS Create, Read, Write function
    
****************************************************/


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#define DRIVER 

#include "usbdi.h"
#include "usbdlib.h"
#include "uusbd.h"

typedef struct _tagParamForCompletionRoutine {
	PDEVICE_OBJECT	DeviceObject;
	PURB			urb;
} PARAMCOMPROUTINE, *PPARAMCOMPROUTINE;

NTSTATUS
Uusbd_SingleUrbReadWrite(
	IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN BOOLEAN Read
	)
/***

Routine Description:

    このルーチンは転送長がdeviceExtension->MaximumTransferSizeより短い
	場合に Uusbd_StagedReadWrite() から呼ばれる。
	この場合、一度で転送できるので分割する必要がない。

Arguments:

    DeviceObject - pointer to our FDO ( Functional Device Object )

    Irp - pointer to the IRP_MJ_READ or IRP_MJ_WRITE

    Read - TRUE for read, FALSE for write


Return Value:

    NT status code

***/
{
    int                 totalLength = 0;
    NTSTATUS            ntStatus;
    ULONG				siz;
    PURB				urb;
    PFILE_OBJECT		fileObject;
    PIO_STACK_LOCATION	irpStack, nextStack;
    PUSBD_PIPE_INFORMATION pipeHandle = NULL;
    PDEVICE_EXTENSION	deviceExtension = DeviceObject->DeviceExtension;
	PPARAMCOMPROUTINE	param;
	PUUSBD_PIPE			pipe;
    UUSBD_KdPrint ( DBGLVL_HIGH, (" Enter Uusbd_SingleUrbReadWrite\n"));


	// We should have already checked this in Uusbd_StagedReadWrite();
	UUSBD_ASSERT( Uusbd_CanAcceptIoRequests( DeviceObject ) );

    Irp->IoStatus.Information = 0;

    siz = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    pipe =  fileObject->FsContext;
    UUSBD_KdPrint( DBGLVL_DEFAULT,("UUSBD_SingleUrbReadWrite() pipe address=%08x\n",pipe));

    if (!pipe)
    {
       ntStatus = STATUS_INVALID_HANDLE;
	   UUSBD_KdPrint ( DBGLVL_DEFAULT, ("UUSBD_SingleUrbReadWrite() Rejecting on invalid pipeHandle 0x%x decimal %d\n",pipe, pipe ));
       Irp->IoStatus.Status = ntStatus;
       IoCompleteRequest (Irp, IO_NO_INCREMENT );
       return ntStatus;
    }

//	UUSBD_ASSERT( UsbdPipeTypeBulk == pipeHandle->PipeType );


	if ( Irp->MdlAddress ) { // could be NULL for 0-len request
		totalLength = MmGetMdlByteCount(Irp->MdlAddress);
	}


    // Build our URB for USBD
    UUSBD_KdPrint( DBGLVL_DEFAULT,("PipeInfo address=%08x\n",pipe->PipeInfo));

    urb = Uusbd_BuildAsyncRequest(DeviceObject,
                                           Irp,
                                           &pipe->PipeInfo,
                                           Read);
	if ( !urb ) {  

	    UUSBD_KdPrint ( DBGLVL_DEFAULT, ("Uusbd_SingleUrbReadWrite()failed to alloc urb\n" ));

		ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        Irp->IoStatus.Status = ntStatus;
        IoCompleteRequest (Irp, IO_NO_INCREMENT );
        return ntStatus;

	}

//	deviceExtension->BaseUrb = urb;  // save pointer to URb we alloced for this xfer; we free in completion routine

    //
    // Now that we have created the urb, we will send a
    // request to the USB device object.
    //

    //
    // Call the class driver to perform the operation.  


    // IoGetNextIrpStackLocation gives a higher level driver access to the next-lower 
    // driver's I/O stack location in an IRP so the caller can set it up for the lower driver.
    nextStack = IoGetNextIrpStackLocation(Irp);

    UUSBD_ASSERT(nextStack != NULL);
    //
    // pass the URB to the USB driver stack
    //
    nextStack->Parameters.Others.Argument1 = urb;

    nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    nextStack->Parameters.DeviceIoControl.IoControlCode =
        IOCTL_INTERNAL_USB_SUBMIT_URB;

	param = (PPARAMCOMPROUTINE)UUSBD_ExAllocatePool(NonPagedPool,	sizeof(PARAMCOMPROUTINE));
	param->DeviceObject = DeviceObject;
	param->urb = urb;

    IoSetCompletionRoutine(
                Irp,                       // irp to use
                Uusbd_SimpleReadWrite_Complete,   // routine to call when irp is done
                param,		                     // we pass our FDO as context to pass routine
                TRUE,                      // call on success
                TRUE,                      // call on error
                TRUE);                     // call on cancel


    //
    // Call IoCallDriver to send the irp to the usb port.
    //
    UUSBD_KdPrint ( DBGLVL_HIGH, (" Uusbd_SingleUrbReadWrite about to IoCallDriver\n"));

    Uusbd_IncrementIoCount(DeviceObject);

    UUSBD_KdPrint( DBGLVL_MAXIMUM,(" Uusbd_SingleUrbReadWrite urb Length = 0x%x decimal %d, bufMDL = 0x%x buf = 0x%x\n",
            urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
            urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
            urb->UrbBulkOrInterruptTransfer.TransferBufferMDL,
            urb->UrbBulkOrInterruptTransfer.TransferBuffer));
	ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp );

    UUSBD_KdPrint ( DBGLVL_HIGH, (" Uusbd_SingleUrbReadWrite after IoCallDriver status (pending) 0x%x\n", ntStatus));
    //
    // The USB driver should always return STATUS_PENDING when
    // it receives an irp with major function code IRP_MJ_WRITE or IRP_MJ_READ.
    //

    ASSERT( ntStatus == STATUS_PENDING);


    UUSBD_KdPrint ( DBGLVL_HIGH, (" Exit Uusbd_SingleUrbReadWrite\n")); 

    return ntStatus; 

}


NTSTATUS
Uusbd_StagedReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN BOOLEAN Read
    )
/*++

Routine Description:

    This routine is called by Uusbd_Read() for IRP_MJ_READ.
    This routine is called by Uusbd_Write() for IRP_MJ_WRITE.

    Breaks up a read or write in to specified sized chunks,
    as specified by deviceExtension->MaximumTransferSize

Arguments:

    DeviceObject - pointer to our FDO ( Functional Device Object )

    Irp - pointer to the IRP_MJ_READ or IRP_MJ_WRITE

    Read - TRUE for read, FALSE for write


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
	NTSTATUS resetPipeStatus;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack, nextStack;
    PURB urb;
    PIRP irp;
    PMDL mdl; 
    PVOID va;
    CHAR stackSize;
    KIRQL OldIrql;
	BOOLEAN fRes;
    NTSTATUS waitStatus;
    ULONG i, nIrps = 0, totalLength = 0, totalIrpsNeeded, used;
    PUSBD_PIPE_INFORMATION pipeHandle = NULL;
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PUCHAR pCon =  NULL;
	ULONG ChunkSize = deviceExtension->MaximumTransferSize;
	ULONG arraySize;
    PUUSBD_RW_CONTEXT context = NULL;
	PUUSBD_PIPE pipe;

    UUSBD_KdPrint ( DBGLVL_DEFAULT, ("enter Uusbd_StagedReadWrite()\n"));

    Irp->IoStatus.Information = 0;

    // Can't accept a new io request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !Uusbd_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        Irp->IoStatus.Status = ntStatus;
		UUSBD_KdPrint ( DBGLVL_DEFAULT, ("Uusbd_StagedReadWrite() can't accept requests, returning STATUS_INSUFFICIENT_RESOURCES\n"));
        IoCompleteRequest (Irp, IO_NO_INCREMENT );
        return ntStatus;
    }

	if ( Irp->MdlAddress ) { // could be NULL for 0-len request
		totalLength = MmGetMdlByteCount(Irp->MdlAddress);
	}


	UUSBD_KdPrint ( DBGLVL_HIGH, ("Uusbd_StagedReadWrite() totalLength = decimal %d, Irp->MdlAddress = 0x%x\n",totalLength, Irp->MdlAddress ));

	if ( totalLength <= deviceExtension->MaximumTransferSize ) {

		// for short or zero-len transfers, no need to do the staging; do it in a single request

		return Uusbd_SingleUrbReadWrite( DeviceObject, Irp, Read );

	}


    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    pipe =  fileObject->FsContext;
  
    if (!pipe)
    {
       ntStatus = STATUS_INVALID_HANDLE;
	   UUSBD_KdPrint ( DBGLVL_DEFAULT, ("Uusbd_StagedReadWrite() Rejecting on invalid pipeHandle 0x%x decimal %d\n",pipeHandle, pipeHandle ));
       Irp->IoStatus.Status = ntStatus;
       IoCompleteRequest (Irp, IO_NO_INCREMENT );
       return ntStatus;
    }

    //
    // submit the request to USB
    //
//	UUSBD_ASSERT( UsbdPipeTypeBulk == pipeHandle->PipeType );

	UUSBD_ASSERT( NULL != Irp->MdlAddress );

	UUSBD_ASSERT(totalLength > deviceExtension->MaximumTransferSize );

    // calculate total # of staged irps that will be needed
	totalIrpsNeeded =  totalLength / deviceExtension->MaximumTransferSize ;
	if ( totalLength % deviceExtension->MaximumTransferSize )
		totalIrpsNeeded++;


	UUSBD_ASSERT( !pipe->PendingIoIrps );  // this should have been cleaned up last time
	UUSBD_ASSERT( !pipe->BaseIrp );  // this should have been cleaned up last time

    used = 0;
	// alloc one extra for termination
	arraySize =  ( totalIrpsNeeded +1 ) * sizeof(UUSBD_RW_CONTEXT);

    // allocate space for an array of UUSBD_RW_CONTEXT structs for the staged irps
    pipe->PendingIoIrps = UUSBD_ExAllocatePool(NonPagedPool, arraySize );
    
    if ( !pipe->PendingIoIrps ) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        UUSBD_KdPrint ( DBGLVL_DEFAULT, ("Uusbd_StagedReadWrite() !pipe->PendingIoIrps STATUS_INSUFFICIENT_RESOURCES\n"));
        Irp->IoStatus.Status = ntStatus;
        IoCompleteRequest (Irp, IO_NO_INCREMENT );
        return ntStatus;
    }

    RtlZeroMemory(pipe->PendingIoIrps, arraySize );

    // init ptr to 1st UUSBD_RW_CONTEXT struct in array
    pCon =  (PUCHAR) pipe->PendingIoIrps;

    pipe->BaseIrp = Irp; // this is the original user's irp
    pipe->StagedBytesTransferred = 0;
	pipe->StagedPendingIrpCount = totalIrpsNeeded;

    // we need to build a series of irps & urbs to represent 
    // this request.

    while (NT_SUCCESS(ntStatus) ) {
        context =  (PUUSBD_RW_CONTEXT) pCon;
        irp = NULL;
        urb = NULL;
        mdl = NULL;

        if ( !Uusbd_CanAcceptIoRequests( DeviceObject ) ) {
            // got sudden remove! ( i.e. plug was yanked )
            ntStatus = STATUS_DELETE_PENDING;
            Irp->IoStatus.Status = ntStatus;
		    UUSBD_KdPrint ( DBGLVL_MAXIMUM, ("Uusbd_StagedReadWrite() got sudden remove, breaking out of URB-building loop\n"));
            break;
        }

        stackSize = (CCHAR)(deviceExtension->TopOfStackDeviceObject->StackSize + 1);
        irp = IoAllocateIrp(stackSize, FALSE);
        
        // Get the virtual address for the buffer described by 
        // our original input Irp's MDL. 
        va = MmGetMdlVirtualAddress(Irp->MdlAddress);

        if (irp) {
           // Each new Irp will 'see' the entire buffer, but map it's IO location
           // to a single ChunkSize section within it via IoBuildPartialMdl()
           mdl = IoAllocateMdl(va,
                                totalLength,
                                FALSE,
                                FALSE,
                                irp);
        }                                    
                            
        if (mdl) {        
			// see if we're done yet
			if( ( used + ChunkSize ) > totalLength  ) {
				// make sure to truncate last transfer if neccy
				ChunkSize = totalLength - used;

			}

            // Map the sub-area of the full user buffer this staged Irp will be using for IO
            IoBuildPartialMdl(Irp->MdlAddress, // Points to an MDL describing the original buffer,
                                               // of which a subrange is to be mapped
                              mdl,             // our allocated target mdl
                              (PUCHAR)va + used, // base virtual address of area to be mapped
                              ChunkSize);      // size of area to be mapped

            used+=ChunkSize;
                                              
            urb = Uusbd_BuildAsyncRequest(DeviceObject,
                                           irp,
                                           &pipe->PipeInfo,
                                           Read);
        }
        
        if (urb && irp && mdl) {

            context->Urb = urb;
            context->DeviceObject = DeviceObject;
			context->BasePipe = pipe;
            context->Irp = irp;
            context->Mdl = mdl;
			nIrps++;
        
            // IoGetNextIrpStackLocation gives a higher level driver access to the next-lower 
            // driver's I/O stack location in an IRP so the caller can set it up for the lower driver.
            nextStack = IoGetNextIrpStackLocation(irp);
            UUSBD_ASSERT(nextStack != NULL);
            UUSBD_ASSERT(DeviceObject->StackSize>1);

            nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
            nextStack->Parameters.Others.Argument1 = urb;
            nextStack->Parameters.DeviceIoControl.IoControlCode =
                IOCTL_INTERNAL_USB_SUBMIT_URB;

            IoSetCompletionRoutine(irp,
                    Uusbd_AsyncReadWrite_Complete,
                    context, // pass the context array element to completion routine
                    TRUE,    // invoke on success
                    TRUE,    // invoke on error
                    TRUE);   // invoke on cancellation of the Irp

            UUSBD_KdPrint ( DBGLVL_MAXIMUM, ("Uusbd_StagedReadWrite() created staged irp #%d %x\n", nIrps, irp));
                                   
            // We keep an array of all pending read/write Irps; we may have to cancel
            // them explicitly on sudden device removal or other error
			(( PUUSBD_RW_CONTEXT) pCon)->Irp = irp;

            Uusbd_IncrementIoCount(DeviceObject);

			ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject,
             (( PUUSBD_RW_CONTEXT) pCon)->Irp);


        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
			UUSBD_KdPrint ( DBGLVL_DEFAULT, ("Uusbd_StagedReadWrite() Dumped from irp loop on failed Irp or urb allocate\n"));
            break;
        }

        if (used >= totalLength) {
            break;      // we're done
        }


        // point to next UUSBD_RW_CONTEXT struct
        pCon +=  sizeof(UUSBD_RW_CONTEXT);

    } // end while


    Irp->IoStatus.Status = ntStatus;

    if (!NT_SUCCESS(ntStatus)) {

        UUSBD_KdPrint ( DBGLVL_DEFAULT, ("Uusbd_StagedReadWrite() FAILED, ntStatus = 0x%x\n", ntStatus));

	    // try to reset the pipe on error ( unless device has been suddenly removed )
	    if ( pipe  && Uusbd_CanAcceptIoRequests( DeviceObject ) ) {

		    resetPipeStatus = Uusbd_ResetPipe(DeviceObject, &pipe->PipeInfo );

		    UUSBD_KdPrint( DBGLVL_DEFAULT, ("Uusbd_StagedReadWrite() Tried to reset pipe 0x%x, Status = 0x%x\n", pipeHandle, resetPipeStatus));
		    UUSBD_KdPrintCond ( DBGLVL_DEFAULT, (!NT_SUCCESS(resetPipeStatus)), ("Uusbd_StagedReadWrite() Uusbd_ResetPipe() FAILED\n"));

		    if( !NT_SUCCESS(resetPipeStatus) ) {
			    // if can't reset pipe, try to reset device ( parent port )
			    UUSBD_KdPrint( DBGLVL_DEFAULT, ("Will try to reset device \n"));

			    resetPipeStatus = Uusbd_ResetDevice(DeviceObject);

			    UUSBD_KdPrintCond ( DBGLVL_DEFAULT, (!NT_SUCCESS(resetPipeStatus)), ("Uusbd_StagedReadWrite() Uusbd_ResetDevice() FAILED\n"));
		    }
	    }
    } // end, if !NT_SUCCESS( ntStatus )


    if ( 0 == nIrps ) {
        // only complete the request here if we created no staged irps
		UUSBD_KdPrint ( DBGLVL_HIGH, ("Uusbd_StagedReadWrite() 0 irps staged, completing  base IRP now!\n"));
        IoCompleteRequest (Irp, IO_NO_INCREMENT  );
    } else {
		UUSBD_KdPrint ( DBGLVL_HIGH, ("Uusbd_StagedReadWrite() %d irps staged\n", nIrps));

        // We need to protect the below  test with the spinlock because it is possible for
        // Uusbd_AsynReadWriteComplete() to fire off while we are in this code
        KeAcquireSpinLock(&pipe->FastCompleteSpinlock, &OldIrql);
        if ( pipe->BaseIrp ) {
            //
            // Mark the original input Irp pending; it will be completed when the last staged irp
            //  is handled ( in Uusbd_AsyncReadWrite_Complete() ).
            //
		    UUSBD_KdPrint ( DBGLVL_HIGH, ("Uusbd_StagedReadWrite(),marking base IRP  0x%x pending!\n", Irp));
            UUSBD_ASSERT( Irp == pipe->BaseIrp );
            ntStatus = STATUS_PENDING;
            Irp->IoStatus.Status = ntStatus;
            IoMarkIrpPending(Irp);
        } else {
            // It is possible for Uusbd_AsyncReadWrite_Complete() to have completed the
            //  original irp before we even get here! 
            // If this happens, it will have NULLED-out deviceExtension->BaseIrp.
            ntStatus = STATUS_SUCCESS;
        }
       KeReleaseSpinLock (&pipe->FastCompleteSpinlock, OldIrql);
    }

    UUSBD_KdPrint ( DBGLVL_HIGH, ("Uusbd_StagedReadWrite() StagedReadWrite  ntStatus = 0x%x decimal %d\n", ntStatus, ntStatus));
    UUSBD_KdPrint ( DBGLVL_HIGH, ("EXIT Uusbd_StagedReadWrite() gExAllocCount = dec %d\n", gExAllocCount ));

    return ntStatus;
}



PURB
Uusbd_BuildAsyncRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PUSBD_PIPE_INFORMATION PipeHandle,
    IN BOOLEAN Read
    )
/***

Routine Description:

    Called from Uusbd_StageReadWrite() for IRP_MJ_READ or IRP_MJ_WRITE

Arguments:

    DeviceObject - pointer to the FDO ( Functional Device Object )

    Irp - A staged IRP allocated and mapped by this driver in Uusbd_StageReadWrite()
          to perform a single deviceExtension->MaximumTransferSize IO request

    PipeHandle - handle to the endpoint we're reading or writing

    Read - TRUE for reads, FALSE for writes

Return Value:

    ptr to initialized async urb. ( USB Request Block )

***/
{
    ULONG siz;
    ULONG length = 0;
    PURB urb = NULL;

	if (Irp->MdlAddress) { // could be NULL for 0-len request
		length = MmGetMdlByteCount(Irp->MdlAddress);
	}

    siz = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
    urb = UUSBD_ExAllocatePool(NonPagedPool, siz);

    UUSBD_KdPrint( DBGLVL_MAXIMUM,("Enter Uusbd_BuildAsyncRequest() len = 0x%x decimal %d \n siz = 0x%x urb 0x%x\n Pipehandle 0x%x\n", length, length, siz, urb, PipeHandle));

    if (urb) {
        RtlZeroMemory(urb, siz);

        urb->UrbBulkOrInterruptTransfer.Hdr.Length = (USHORT) siz;
        urb->UrbBulkOrInterruptTransfer.Hdr.Function =
                    URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER;
        urb->UrbBulkOrInterruptTransfer.PipeHandle =
                   PipeHandle->PipeHandle;
        urb->UrbBulkOrInterruptTransfer.TransferFlags =
            Read ? USBD_TRANSFER_DIRECTION_IN : 0;

        // short packet is not treated as an error.
        urb->UrbBulkOrInterruptTransfer.TransferFlags |= 
            USBD_SHORT_TRANSFER_OK;            
                
        //
        // not using linked urb's
        //
        urb->UrbBulkOrInterruptTransfer.UrbLink = NULL;

        urb->UrbBulkOrInterruptTransfer.TransferBufferMDL =
            Irp->MdlAddress;
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength =
            length;

        UUSBD_KdPrint( DBGLVL_MAXIMUM,("Uusbd_BuildAsyncRequest() Init async urb Length = 0x%x decimal %d, bufMDL = 0x%x buf = 0x%x\n",
            urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
            urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
            urb->UrbBulkOrInterruptTransfer.TransferBufferMDL,
            urb->UrbBulkOrInterruptTransfer.TransferBuffer));
    }

    UUSBD_KdPrint( DBGLVL_MAXIMUM,("exit Uusbd_BuildAsyncRequest\n"));

    return urb;
}


NTSTATUS
Uusbd_AsyncReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/***

Routine Description:

	staged read/write Irpの完了ルーチン

Arguments:

    DeviceObject - Pointer to the device object for next lower device
	in the  driver stack; 

    Irp - Irp completed.

    Context - Driver defined context.

Return Value:

    The function value is the final status from the operation.

***/
{
    NTSTATUS			ntStatus = STATUS_SUCCESS;
    PURB				urb;
    KIRQL OldIrql;
    PUUSBD_RW_CONTEXT context = Context;
    PIO_STACK_LOCATION	irpStack;
    PDEVICE_OBJECT      deviceObject;
	PDEVICE_EXTENSION   deviceExtension;
	PUUSBD_PIPE			pipe;
	// We have to get the deviceObject from the context, since the DeviceObject passed in
	//  here belongs to the next lower driver in the stack because we were invoked via
	//   IoCallDriver in Uusbd_StagedReadWrite()
    deviceObject = context->DeviceObject;
	deviceExtension = deviceObject->DeviceExtension;
	pipe = context->BasePipe;

    // prevent Uusbd_StagedReadWrite() from testing deviceExtension->BaseIrp while this routine is running
    KeAcquireSpinLock(&pipe->FastCompleteSpinlock, &OldIrql);

	UUSBD_ASSERT( pipe->PendingIoIrps );  
	UUSBD_ASSERT( pipe->BaseIrp );  

	UUSBD_ASSERT( context->Irp == Irp ); 
    
    urb = context->Urb;

    
    UUSBD_KdPrint( DBGLVL_MAXIMUM,  ("\n\n ENTER Uusbd_AsyncReadWrite_Complete():  Length 0x%08X decimal %d\n   Status 0x%08X\n",
                     urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
                     urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
                     urb->UrbHeader.Status));

    // decrement count of staged pending irps
    pipe->StagedPendingIrpCount--;

    // decrement the driver's overall pending irp count
    Uusbd_DecrementIoCount(deviceObject);
    
    // 
    // IoCallDriver has been called on this Irp;
    // Set the length based on the TransferBufferLength
    // value in the URB
    //
	if(urb->UrbHeader.Status == 0) { // if no error
	    Irp->IoStatus.Information =
	        urb->UrbBulkOrInterruptTransfer.TransferBufferLength;

	    pipe->StagedBytesTransferred += 
	        urb->UrbBulkOrInterruptTransfer.TransferBufferLength; 
	} else {
	    Irp->IoStatus.Information = 0;
	}
    ntStatus = STATUS_MORE_PROCESSING_REQUIRED;

    UUSBD_KdPrint ( DBGLVL_MAXIMUM,("Uusbd_AsyncReadWrite_Complete(): Staged Async Completion %d, bytes = %d\n", 
        pipe->StagedPendingIrpCount,
        pipe->StagedBytesTransferred)); 

    IoFreeIrp(context->Irp);
    context->Irp = NULL; 

    IoFreeMdl(context->Mdl);
    context->Mdl = NULL; 

    if (pipe->StagedPendingIrpCount == 0) {
        
		UUSBD_KdPrint ( DBGLVL_HIGH,("Uusbd_AsyncReadWrite_Complete(): StagedPendingIrpCount == 0, completeting BaseIrp 0x%x\n    Total bytes xferred = 0x%x, decimal %d\n", pipe->BaseIrp, pipe->StagedBytesTransferred, pipe->StagedBytesTransferred));

        pipe->BaseIrp->IoStatus.Status = STATUS_SUCCESS; 

        pipe->BaseIrp->IoStatus.Information = 
            pipe->StagedBytesTransferred;

        IoCompleteRequest(pipe->BaseIrp,
                          IO_NO_INCREMENT);

        UUSBD_ExFreePool( pipe->PendingIoIrps ); 
		pipe->PendingIoIrps  = NULL;
		pipe->BaseIrp = NULL;

        // the event is only waited on if Uusbd_CancelPendingIo() has been called
        KeSetEvent(&pipe->StagingDoneEvent, 1, FALSE);
    }
    UUSBD_ExFreePool(urb);


    UUSBD_KdPrint ( DBGLVL_HIGH, ("Exit Uusbd_AsyncReadWrite_Complete() gExAllocCount = dec %d\n", gExAllocCount ));
    UUSBD_KdPrint ( DBGLVL_MAXIMUM,("Exit Uusbd_AsyncReadWrite_Complete(), ntStatus = 0x%x\n\n",ntStatus )); 

    KeReleaseSpinLock (&pipe->FastCompleteSpinlock, OldIrql);
    return ntStatus;
}




NTSTATUS
Uusbd_SimpleReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/***

Routine Description:

  Completion routine for our single-urb short read/write transfers


Arguments:

    DeviceObject - Pointer to the device object for next lower device
	in the  driver stack; 

    Irp - Irp completed.

    Context - our FDO.

Return Value:

    The function value is the final status from the operation.

***/
{
    NTSTATUS			ntStatus = STATUS_SUCCESS;
    PURB				urb;
    PDEVICE_OBJECT      deviceObject;
	PDEVICE_EXTENSION   deviceExtension;
	PIO_STACK_LOCATION	stack;
	PPARAMCOMPROUTINE	param;
	// We have to get the deviceObject from the context, since the DeviceObject passed in
	//  here belongs to the next lower driver in the stack because we were invoked via
	//   IoCallDriver in Uusbd_SingleUrbReadWrite(); we want OUR device object
	param = (PPARAMCOMPROUTINE) Context;
	deviceObject =  param->DeviceObject;
	deviceExtension = deviceObject->DeviceExtension;

	urb = param->urb;  //get the urb we alloced for this xfer


    //  If the lower driver returned PENDING, mark our stack location as pending also.
    if ( Irp->PendingReturned ) {  
        IoMarkIrpPending(Irp);
    }

    
    UUSBD_KdPrint( DBGLVL_MAXIMUM,  ("\n\n ENTER Uusbd_SimpleReadWrite_Complete():  Length 0x%08X decimal %d\n   Status 0x%08X\n",
                     urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
                     urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
                     urb->UrbHeader.Status));


    // decrement the driver's overall pending irp count
    Uusbd_DecrementIoCount(deviceObject);
    
    ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
	Irp->IoStatus.Status = STATUS_SUCCESS; 

	if(urb->UrbHeader.Status == 0) {
	    Irp->IoStatus.Information =
	        urb->UrbBulkOrInterruptTransfer.TransferBufferLength;
	} else {
	    Irp->IoStatus.Information = 0;
	}

    IoCompleteRequest( Irp,
                      IO_NO_INCREMENT);

	param->urb = NULL;

    UUSBD_ExFreePool(urb);
    UUSBD_ExFreePool(param);


    UUSBD_KdPrint ( DBGLVL_MAXIMUM,("Exit Uusbd_SimpleReadWrite_Complete(), ntStatus = 0x%x\n\n",ntStatus )); 

    return ntStatus;
}






NTSTATUS
Uusbd_Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/***

Routine Description:

	This is the IRP_MJ_READ routine set in our dispatch table;
	ReadFile() calls from user mode ultimately land here

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.

    IRP - pointer to the IRP_MJ_READ

Return Value:

    NT status code

***/
{

    NTSTATUS ntStatus = Uusbd_StagedReadWrite(DeviceObject,
                                  Irp,
                                  TRUE);	// false to write, true to read

    return ntStatus;                                  

}

NTSTATUS
Uusbd_Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/***

Routine Description:
	This is the IRP_MJ_WRITE routine set in our dispatch table;
	WriteFile() calls from user mode ultimately land here

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.

    IRP - pointer to the IRP_MJ_WRITE

Return Value:

    NT status code

***/
{

    NTSTATUS ntStatus = Uusbd_StagedReadWrite(DeviceObject,
                                  Irp,
                                  FALSE);	// false to write, true to read

    return ntStatus;                                  

}

BOOLEAN IsDigit(WCHAR c) // c が数字かどうか調べる
{
	return ( c >= (WCHAR)'0' && c <= (WCHAR)'9');
}

BOOLEAN parse_filename(WCHAR *name, ULONG *n_interface, ULONG *n_pipe)
//
// 引数
//		name			ファイル名 
//		n_interface		インターフェース番号　
//		n_pipe			パイプ番号　
//　帰り値
//		TRUE			正常終了
//		FALSE			ファイル名フォーマットエラー
//
//	機能
//		file nameからインターフェース番号とパイプ番号を求める
//		例	nameの文字列が"\IF00PIPE01" の時、
//		*n_interface = 0, *n_pipe = 1を返す
{
	int n_if = 0;
	int n_pi = 0;
	while(!IsDigit(*name)){ // skip none digit
		if(*name == '\0') return FALSE;
		name++;
	}
	while(IsDigit(*name)){
		if(*name == '\0') return FALSE;
		n_if = n_if * 10 + (*name -(WCHAR)'0');
		name++;
	}
	while(!IsDigit(*name)){// skip none digit
		if(*name == '\0') return FALSE;
		name++;
	}
	while(IsDigit(*name)){
		if(*name == '\0') break;
		n_pi = n_pi * 10 + (*name -(WCHAR)'0');
		name++;
	}
	*n_interface = n_if;
	*n_pipe = n_pi;
	return TRUE;
}

BOOLEAN get_if_and_pipenum( 
    IN PDEVICE_OBJECT DeviceObject,
    IN PUNICODE_STRING FileName,
	OUT PULONG num_if,
	OUT PULONG num_pipe
   )
{
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension; 
	ULONG n_interface, n_pipe;

    if (FileName->Length != 0) {
		if(parse_filename(FileName->Buffer, &n_interface, &n_pipe) &&
			n_interface < deviceExtension->UsbNumInterface &&
			n_pipe < deviceExtension->UsbInterfaceInfo[n_interface].NumberOfPipes) {
	
			*num_if = n_interface;
			*num_pipe = n_pipe;

			return TRUE;
		}
	}
    return FALSE;
}

NTSTATUS
Uusbd_Close(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

	IRP_MJ_CLOSEディスパッチルーチン.
    ユーザーモードで CloseHandle() が呼ばれた時に呼ばれる

Arguments:

    DeviceObject - pointer to our FDO (Functional Device Object )


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
	NTSTATUS actStat;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    PUSBD_PIPE_INFORMATION pipeHandle = NULL;
    PUUSBD_PIPE ourPipe = NULL;
	ULONG num_if, num_pipe;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("entering Uusbd_Close\n"));

    
    Uusbd_IncrementIoCount(DeviceObject);

    deviceExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    if (fileObject->FsContext) {
        // closing pipe handle
        ourPipe =  fileObject->FsContext;

		if ( ourPipe->PipeOpenCount>0 ) { // set if opened
			// may have been aborted
			UUSBD_KdPrint( DBGLVL_DEFAULT,("closing pipe %x\n", pipeHandle));
			deviceExtension->OpenPipeCount--;
			InterlockedDecrement(&ourPipe->PipeOpenCount);
		}
		else {
			// pipe was already closed; this can only be if we got a sudden REMOVE_DEVICE or STOP_DEVICE
			UUSBD_ASSERT( deviceExtension->DeviceRemoved || !deviceExtension->DeviceStarted);
			UUSBD_KdPrint( DBGLVL_DEFAULT,("Pipe %x was already closed \n", pipeHandle));

		}
    } else {
		InterlockedDecrement(&deviceExtension->OpenBaseDevice);
	}

	Uusbd_DecrementIoCount(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    ntStatus = Irp->IoStatus.Status;


    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );
                       
	// try to power down device if this is the last pipe
	actStat = Uusbd_SelfSuspendOrActivate( DeviceObject, TRUE );

    UUSBD_KdPrint( DBGLVL_DEFAULT,("exit Uusbd_Close OpenPipeCount = decimal %d, status %x\n",deviceExtension->OpenPipeCount, ntStatus));

    return ntStatus;
}


NTSTATUS
Uusbd_Create(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/***

Routine Description:

    IRP_MJ_CREATEのディスパッチルーチン.
	ユーザーモードでCreateFile()が呼ばれた時呼ばれる
	パイプのハンドルを得る場合、ファイル名には "<GUID>.\ifXXpipeYY"とする
    XXはパイプの番号、YYはインターフェースの番号とする

Arguments:

    DeviceObject - pointer to our FDO ( Functional Device Object )


Return Value:

    NT status code

***/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    ULONG i, ix;
	NTSTATUS actStat;
    PUUSBD_PIPE ourPipe = NULL;
	ULONG num_if, num_pipe;

    deviceExtension = DeviceObject->DeviceExtension;

    UUSBD_KdPrint( DBGLVL_DEFAULT,("entering Uusbd_Create\n"));

    Uusbd_IncrementIoCount(DeviceObject);

    // Can't accept a new io request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !Uusbd_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;

		UUSBD_KdPrint( DBGLVL_DEFAULT,("ABORTING Uusbd_Create\n"));
        goto done;
    }
    
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    // fscontext is null for device
    fileObject->FsContext = NULL;

    if ( 0 == fileObject->FileName.Length ) {// this is the case if opening device as opposed to pipe
	    UUSBD_KdPrint( DBGLVL_DEFAULT,("open base handle\n"));
		if( InterlockedIncrement(&deviceExtension->OpenBaseDevice)>1){
		    UUSBD_KdPrint( DBGLVL_DEFAULT,("already open \n"));
			ntStatus = STATUS_INVALID_PARAMETER; //既にオープンしている場合
			InterlockedDecrement(&deviceExtension->OpenBaseDevice);
			goto done;      // nothing more to do
		}
	} else { // open pipe
	    UUSBD_KdPrint( DBGLVL_DEFAULT,("open pipe handle\n"));
		if(!get_if_and_pipenum(DeviceObject, &fileObject->FileName, &num_if, &num_pipe)) {
			ntStatus = STATUS_INVALID_PARAMETER;
			goto done;
		}
	    UUSBD_KdPrint( DBGLVL_DEFAULT,("if=%d pipe=%d\n",num_if, num_pipe));	

		ourPipe = &deviceExtension->UsbInterfaceInfo[num_if].Pipe[num_pipe]; // PUSBD_PIPE_INFORMATION  PipeInfo;		
	    UUSBD_KdPrint( DBGLVL_DEFAULT,("pipe address=%08x\n",ourPipe));
	    UUSBD_KdPrint( DBGLVL_DEFAULT,("PipeInfo address=%08x\n",&ourPipe->PipeInfo));
		
		if( InterlockedIncrement(&ourPipe->PipeOpenCount)>1){//既にオープンしている場合
			ntStatus = STATUS_INVALID_PARAMETER;
			InterlockedDecrement(&ourPipe->PipeOpenCount);
			goto done;
		}
		
		fileObject->FsContext = ourPipe;
		UUSBD_KdPrint( DBGLVL_DEFAULT,("open pipe %x\n",fileObject->FsContext ));

		ntStatus = STATUS_SUCCESS;
		deviceExtension->OpenPipeCount++;
	}
	// try to power up device if its not in D0
	ntStatus = Uusbd_SelfSuspendOrActivate( DeviceObject, FALSE );

done:
    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;


    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

    Uusbd_DecrementIoCount(DeviceObject);                               

    UUSBD_KdPrint( DBGLVL_DEFAULT,("exit Uusbd_Create %x\n", ntStatus));


    return ntStatus;
}


BOOLEAN
Uusbd_CancelPendingIo_pipe(PUUSBD_PIPE pipe)
{
	PIRP Irp;
	PUUSBD_RW_CONTEXT	pcontext;
	USHORT uDriverCancel = 0;  // count cancelled via iocancelirp()
	BOOLEAN cRes; 
    NTSTATUS ntStatus, waitStatus;
	// nothing pending
	if ( !pipe->PendingIoIrps )
		return FALSE;

    UUSBD_KdPrint ( DBGLVL_MAXIMUM, ("enter UUSBD_CancelPendingIo()\n"));
    // the UUSBD_RW_CONTEXT array is terminated by an entry with a NULL Irp
	pcontext = pipe->PendingIoIrps;
	while(1){
		Irp = pcontext->Irp;
		if(Irp == NULL) break;
		//
		// Since IoCallDriver has been called on this request, we call IoCancelIrp
		//  and let our completion routine handle it
		//
		cRes = IoCancelIrp( Irp );

		UUSBD_KdPrint ( DBGLVL_MAXIMUM,  ("UUSBD_CancelPendingIo() IoCancelIrp() cRes=%d, IRP 0x%x\n", cRes, Irp));

		// if cancel call failed, they all will, so dump out
		if ( !cRes )
			break;

		uDriverCancel++; // flag we tried to cancel at least one

		// point to next context struct in array
        pcontext++;
	}

    if ( uDriverCancel && cRes ) {

		// We only get here if we cancelled at least one and all cancellations were successfull.
		// Wait on the event set on last cancel in UUSBD_AsyncReadWriteComplete();
		UUSBD_KdPrint ( DBGLVL_MAXIMUM, ("UUSBD_CancelPendingIo() before waiting for StagingDoneEvent()\n" ));
		waitStatus = KeWaitForSingleObject(
                       &pipe->StagingDoneEvent,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);
	    UUSBD_KdPrint ( DBGLVL_MAXIMUM, ("UUSBD_CancelPendingIo() finished waiting for StagingDoneEvent()\n" ));
    }

	UUSBD_KdPrintCond ( DBGLVL_HIGH, uDriverCancel,
        ("UUSBD_CancelPendingIo() cancelled %d via IoCancelIrp()\n",uDriverCancel));

    return (BOOLEAN) uDriverCancel;                                  

}

BOOLEAN
Uusbd_CancelPendingIo(
    IN PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

	ペンディング中のIRPをキャンセルする（突然のIRP_MN_REMOVE_DEVICEの場合）
	このドライバーは自分で作成したIRPをUTK_RW_CONTEXT構造体の配列として
	持っている。
	このルーチンではそれをキャンセルする

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.


Return Value:

    TRUE if cancelled any, else FALSE

***/
{
	PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
 	ULONG i, j;
	PUUSBD_PIPE pipe;
	BOOLEAN f;
	f = FALSE;
	for(i=0;i<deviceExtension->UsbNumInterface;i++){
		for(j=0;j<deviceExtension->UsbInterfaceInfo[i].NumberOfPipes;j++){
			pipe = &deviceExtension->UsbInterfaceInfo[i].Pipe[j]; 
		    UUSBD_KdPrint ( DBGLVL_MAXIMUM, ("call UUSBD_CancelPendingIo_pipe(%08x)\n", pipe));
			if(Uusbd_CancelPendingIo_pipe(pipe)) f = TRUE;
		    UUSBD_KdPrint ( DBGLVL_MAXIMUM, ("return from UUSBD_CancelPendingIo_pipe()\n"));
		}
	}
	return f;
}

NTSTATUS
Uusbd_AbortPipes_sub(PDEVICE_OBJECT DeviceObject, PUUSBD_PIPE pipe)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb;
	ULONG i,j;

    PUSBD_INTERFACE_INFORMATION interface;
	UUSBD_KdPrint( DBGLVL_HIGH,("UUSBD_AbortPipes() Aborting open  Pipe %d\n", i));
				
	urb = UUSBD_ExAllocatePool(NonPagedPool,
		sizeof(struct _URB_PIPE_REQUEST));
				
	if (urb) {
		
		urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
		urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
		urb->UrbPipeRequest.PipeHandle =
			pipe->PipeInfo.PipeHandle;
		
		ntStatus = Uusbd_CallUSBD(DeviceObject, urb);
		
		UUSBD_ExFreePool(urb);
		
	} else {
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;
		UUSBD_KdPrint( DBGLVL_HIGH,("UUSBD_AbortPipes() FAILED urb alloc\n" ));
	}
				
				
	if (!(NT_SUCCESS(ntStatus))) {
		// if we failed, dump out
#if DBG
		if ( gpDbg )
			gpDbg->PipeErrorCount++;
#endif
	}else {
		pipe->PipeOpenCount = 0; // mark the pipe 'closed'
		
#if DBG
		if ( gpDbg )
			gpDbg->AbortPipeCount++;
#endif
	}
    return ntStatus;
}

NTSTATUS
Uusbd_AbortPipes(
    IN PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

	Called as part of sudden device removal handling.
    Cancels any pending transfers for all open pipes. 
	If any pipes are still open, call USBD with URB_FUNCTION_ABORT_PIPE
	Also marks the pipe 'closed' in our saved  configuration info.

Arguments:

    Ptrs to our FDO

Return Value:

    NT status code

***/
{
	PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
 	ULONG i, j;
	PUUSBD_PIPE pipe;
	BOOLEAN f;
	NTSTATUS status, ret;
	UUSBD_KdPrint( DBGLVL_HIGH,("enter:UUSBD_AbortPipes()"));
	ret = STATUS_SUCCESS;
	for(i=0;i<deviceExtension->UsbNumInterface;i++){
		for(j=0;j<deviceExtension->UsbInterfaceInfo[i].NumberOfPipes;j++){
			pipe = &deviceExtension->UsbInterfaceInfo[i].Pipe[j]; 
			if(pipe->PipeOpenCount >0){
				status = Uusbd_AbortPipes_sub(DeviceObject, pipe);
				if(status == STATUS_SUCCESS){
					deviceExtension->OpenPipeCount--;
				} else {
					ret = status;
				}
			}
		}
	}
	UUSBD_KdPrint( DBGLVL_HIGH,("leave:UUSBD_AbortPipes()"));
	return ret;
}


BOOLEAN
Uusbd_CanAcceptIoRequests(
    IN PDEVICE_OBJECT DeviceObject
    )
/***

Routine Description:

  Check device extension status flags; 

     Can't accept a new io request if device:
      1) is removed, 
      2) has never been started, 
      3) is stopped,
      4) has a remove request pending, or
      5) has a stop device pending


Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.


Return Value:

    return TRUE if can accept new io requests, else FALSE

***/
{
    PDEVICE_EXTENSION deviceExtension;
	BOOLEAN fCan = FALSE;

    deviceExtension = DeviceObject->DeviceExtension;

	//flag set when processing IRP_MN_REMOVE_DEVICE
    if ( !deviceExtension->DeviceRemoved &&
		 // device must be started( enabled )
		 deviceExtension->DeviceStarted &&
 		 // flag set when driver has answered success to IRP_MN_QUERY_REMOVE_DEVICE
		 !deviceExtension->RemoveDeviceRequested &&
		 // flag set when driver has answered success to IRP_MN_QUERY_STOP_DEVICE
		 !deviceExtension->StopDeviceRequested ){
			fCan = TRUE;
	}

    UUSBD_KdPrintCond( DBGLVL_MAXIMUM, !fCan, ("**** FALSE return from Uusbd_CanAcceptIoRequests()!\n"));

	return fCan;
}


