/***************************************************

Copyright (c) 1999  kashiwano masahiro

Abstract:

    UUSBD.SYS PowerIRP functions
    
****************************************************/


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "uusbd.h"


extern PRKMUTEX mutex_pocall;

NTSTATUS
Uusbd_ProcessPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/***

Routine Description:

    IRP_MJ_POWERのディスパッチ

Arguments:

    DeviceObject - pointer to our device object (FDO)

    Irp          - pointer to an I/O Request Packet

Return Value:

    NT status code

***/
{

    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    BOOLEAN fGoingToD0 = FALSE;
    POWER_STATE sysPowerState, desiredDevicePowerState;
    KEVENT event;

    UUSBD_KdPrint( DBGLVL_MEDIUM,(" Uusbd_ProcessPowerIrp() IRP_MJ_POWER\n"));

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    Uusbd_IncrementIoCount(DeviceObject);

    switch (irpStack->MinorFunction) {
    case IRP_MN_WAIT_WAKE:
        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Enter IRP_MN_WAIT_WAKE\n"));

		// A driver sends IRP_MN_WAIT_WAKE to indicate that the system should 
		// wait for its device to signal a wake event. The exact nature of the event
		// is device-dependent. 
		// Drivers send this IRP for two reasons: 
		// 1) To allow a device to wake the system
		// 2) To wake a device that has been put into a sleep state to save power
		//    but still must be able to communicate with its driver under certain circumstances. 
		// When a wake event occurs, the driver completes the IRP and returns 
		// STATUS_SUCCESS. If the device is sleeping when the event occurs, 
		// the driver must first wake up the device before completing the IRP. 
		// In a completion routine, the driver calls PoRequestPowerIrp to send a 
		// PowerDeviceD0 request. When the device has powered up, the driver can 
		//  handle the IRP_MN_WAIT_WAKE request.

        // deviceExtension->DeviceCapabilities.DeviceWake specifies the lowest device power state (least powered)
        // from which the device can signal a wake event 
        deviceExtension->PowerDownLevel = deviceExtension->DeviceCapabilities.DeviceWake;


        if  ( ( PowerDeviceD0 == deviceExtension->CurrentDevicePowerState )  ||
              ( deviceExtension->DeviceCapabilities.DeviceWake > deviceExtension->CurrentDevicePowerState ) ) {
			//
			//    STATUS_INVALID_DEVICE_STATE is returned if the device in the PowerD0 state
			//    or a state below which it can support waking, or if the SystemWake state
			//    is below a state which can be supported. A pending IRP_MN_WAIT_WAKE will complete
			//    with this error if the device's state is changed to be incompatible with the wake 
			//    request.

            //  If a driver fails this IRP, it should complete the IRP immediately without
            //  passing the IRP to the next-lower driver.
            ntStatus = STATUS_INVALID_DEVICE_STATE;
            Irp->IoStatus.Status = ntStatus;
            IoCompleteRequest (Irp,IO_NO_INCREMENT );
            UUSBD_KdPrint( DBGLVL_HIGH, ( "Exit Uusbd_ProcessPowerIrp(), ntStatus STATUS_INVALID_DEVICE_STATE\n" ) );
            Uusbd_DecrementIoCount(DeviceObject);
            return ntStatus;
        }

        // flag we're enabled for wakeup
        deviceExtension->EnabledForWakeup = TRUE;

        // init an event for our completion routine to signal when PDO is done with this Irp
        KeInitializeEvent(&event, NotificationEvent, FALSE);

       // If not failing outright, pass this on to our PDO for further handling
        IoCopyCurrentIrpStackLocationToNext(Irp);

        // Set a completion routine so it can signal our event when
        //  the PDO is done with the Irp
        IoSetCompletionRoutine(Irp,
                               Uusbd_IrpCompletionRoutine,
                               &event,  // pass the event to the completion routine as the Context
                               TRUE,    // invoke on success
                               TRUE,    // invoke on error
                               TRUE);   // invoke on cancellation

        PoStartNextPowerIrp(Irp);
        ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                Irp);

         // if PDO is not done yet, wait for the event to be set in our completion routine
        if (ntStatus == STATUS_PENDING) {
             // wait for irp to complete

            NTSTATUS waitStatus = KeWaitForSingleObject(
                &event,
                Suspended,
                KernelMode,
                FALSE,
                NULL);

            UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() done waiting for PDO to finish IRP_MN_WAIT_WAKE\n"));
        }

		// now tell the device to actually wake up
		Uusbd_SelfSuspendOrActivate( DeviceObject, FALSE );

        // flag we're done with wakeup irp
        deviceExtension->EnabledForWakeup = FALSE;

        Uusbd_DecrementIoCount(DeviceObject);

        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Exit IRP_MN_WAIT_WAKE\n"));
        break;

    case IRP_MN_SET_POWER:
        {
		// システムパワーポリシーマネージャーはシステムパワーステートを変更する時
		// や、デバイスパワーポリシーマネージャーがデバイスのパワーステートを変更
		// する時このIRPを送る
		
        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Enter IRP_MN_SET_POWER\n"));

        // Irp->IoStatus.Status には STATUS_SUCCESS をセットする
		// これはデバイスが要求通りの状態になったことを示す
        // このIRPは失敗させてはいけない。

        switch (irpStack->Parameters.Power.Type) {
            case SystemPowerState:

                // Get input system power state
                sysPowerState.SystemState = irpStack->Parameters.Power.State.SystemState;

                UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Set Power, type SystemPowerState = %s\n",
                    UUSBD_StringForSysState( sysPowerState.SystemState ) ));

                // システムが working state の場合、デバイスを D0ステートにする
                // IRP_MN_WAIT_WAKE待ちであって無条件にDOにする
                if ( sysPowerState.SystemState ==  PowerSystemWorking) {
                    desiredDevicePowerState.DeviceState = PowerDeviceD0;

                     UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() PowerSystemWorking, will set D0, not use state map\n"));


                } else {
                    if ( deviceExtension->EnabledForWakeup ) {
						// IRP_MN_WAIT_WAKEがペンディングの場合
						// デバイスエクステンションに保存してあるシステムパワーステートと
						// デバイスパワーステートの対応表に従ってデバイスパワーステートを
						// 決める
                        desiredDevicePowerState.DeviceState =
                            deviceExtension->DeviceCapabilities.DeviceState[ sysPowerState.SystemState ];

                        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() IRP_MN_WAIT_WAKE pending, will use state map\n"));

                    } else {  
						// IRP_MN_WAIT_WAKEがペンディングでない場合
                        // この場合デバイスを完全に止めても問題ない
                        desiredDevicePowerState.DeviceState = PowerDeviceD3;

                        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Not EnabledForWakeup and the system's not in working state,\n  settting PowerDeviceD3 (off )\n"));
                    }
                }

                UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Set Power, desiredDevicePowerState = %s\n",
                    UUSBD_StringForDevState( desiredDevicePowerState.DeviceState ) ));

                if (desiredDevicePowerState.DeviceState !=
                    deviceExtension->CurrentDevicePowerState) {

                    // Uusbd_IncrementIoCount(DeviceObject);

                    // デバイスのステートを変更しなければならない場合
					// IRP_MN_SET_POWERを送ってデバイスのパワーステートを変える
                    deviceExtension->PowerIrp = Irp;
                    ntStatus = PoRequestPowerIrp(deviceExtension->PhysicalDeviceObject,
                                               IRP_MN_SET_POWER,
                                               desiredDevicePowerState,
											   // completion routine will pass the Irp down to the PDO
                                               Uusbd_PoRequestCompletion, 
                                               DeviceObject,
                                               NULL);

                } else {
                    // デバイスのステートに変更ない場合
                    // あとは、下層にIRPを引き継ぐ
                    PoStartNextPowerIrp(Irp);
					IoSkipCurrentIrpStackLocation(Irp);
                    ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                            Irp);
	
                    Uusbd_DecrementIoCount(DeviceObject);
                    UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Exit IRP_MN_SET_POWER\n"));

                }
                break;

            case DevicePowerState:

                UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Set Power, type DevicePowerState = %s\n",
                    UUSBD_StringForDevState( irpStack->Parameters.Power.State.DeviceState ) ));

				// D1, D2, or D3にする場合
				//
				// まず、デバイスのステートを変更する。(deviceExtension->CurrentDevicePowerState に DeviceStateの値を
				// をセットする。）
				// その後下層にIRPを渡す

				// D0する場合
				// 始めに下層にIRPを渡しそれが完了したら、デバイスのステートを変更する。
				// デバイスのステート変更は完了ルーチン内で行う。

                fGoingToD0 = Uusbd_SetDevicePowerState(DeviceObject,
                                                      irpStack->Parameters.Power.State.DeviceState
                                                      ); // returns TRUE for D0


                if (fGoingToD0) {
                    UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Set PowerIrp Completion Routine, fGoingToD0 =%d\n", fGoingToD0));
	                IoCopyCurrentIrpStackLocationToNext(Irp);
                    IoSetCompletionRoutine(Irp,
                           Uusbd_PowerIrp_Complete,
                           DeviceObject,	// 完了ルーチンのContextにはFDOのデバイスオブジェクトを渡す
                           TRUE,            // invoke on success
                           TRUE,            // invoke on error
                           TRUE);           // invoke on cancellation of the Irp
                } else {
	                PoStartNextPowerIrp(Irp);
					IoSkipCurrentIrpStackLocation(Irp);
				}

                UUSBD_KdPrint( 1,("PoCallDriver(%08x,%08x)\n",deviceExtension->TopOfStackDeviceObject,Irp ));
                ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                        Irp);
                if ( !fGoingToD0 ) // completion routine will decrement
                    Uusbd_DecrementIoCount(DeviceObject);

                UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() Exit IRP_MN_SET_POWER\n"));
                break;
            } /* case irpStack->Parameters.Power.Type */

        }
        break; /* IRP_MN_SET_POWER */

    case IRP_MN_QUERY_POWER:
		//
		// A power policy manager sends this IRP to determine whether it can change
		// the system or device power state, typically to go to sleep.
		//

        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() IRP_MN_QUERY_POWER\n"));

		/*
		このドライバーで処理しないIRPはなにもせず下層に渡す。
		その場合PoStartNextPowerIrp,IoSkipCurrentIrpStackLocation,PoCallDriver
		の順に呼び出すこと（W2KRC3DDKより）
		*/
        PoStartNextPowerIrp(Irp);
		IoSkipCurrentIrpStackLocation(Irp);
        ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                                Irp);


        Uusbd_DecrementIoCount(DeviceObject);

        break; /* IRP_MN_QUERY_POWER */

    default:

        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_ProcessPowerIrp() UNKNOWN POWER MESSAGE (%x)\n", irpStack->MinorFunction));

        //
        // All unhandled power messages are passed on to the PDO
        //

		/*
		このドライバーで処理しないIRPはなにもせず下層に渡す。
		その場合PoStartNextPowerIrp,IoSkipCurrentIrpStackLocation,PoCallDriver
		の順に呼び出すこと（W2KRC3DDKより）
		*/
        PoStartNextPowerIrp(Irp);
		IoSkipCurrentIrpStackLocation(Irp); // ***
        ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

        Uusbd_DecrementIoCount(DeviceObject);

    } /* irpStack->MinorFunction */

    UUSBD_KdPrint( DBGLVL_MEDIUM,  ( "Exit Uusbd_ProcessPowerIrp()  ntStatus = 0x%x\n", ntStatus ) );
    return ntStatus;
}


NTSTATUS
Uusbd_PoRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    )
/***

Routine Description:

	This is the completion routine set in a call to PoRequestPowerIrp()
	that was made in Uusbd_ProcessPowerIrp() in response to receiving
    an IRP_MN_SET_POWER of type 'SystemPowerState' when the device was
	not in a compatible device power state. In this case, a pointer to
	the IRP_MN_SET_POWER Irp is saved into the FDO device extension 
	(deviceExtension->PowerIrp), and then a call must be
	made to PoRequestPowerIrp() to put the device into a proper power state,
	and this routine is set as the completion routine.

    We decrement our pending io count and pass the saved IRP_MN_SET_POWER Irp
	on to the next driver

Arguments:

    DeviceObject - Pointer to the device object for the class device.
        Note that we must get our own device object from the Context

    Context - Driver defined context, in this case our own functional device object ( FDO )

Return Value:

    The function value is the final status from the operation.

***/
{
    PIRP irp;
    PDEVICE_EXTENSION deviceExtension;
    PDEVICE_OBJECT deviceObject = Context;
    NTSTATUS ntStatus;

    deviceExtension = deviceObject->DeviceExtension;

	// Get the Irp we saved for later processing in Uusbd_ProcessPowerIrp()
	// when we decided to request the Power Irp that this routine 
	// is the completion routine for.
    irp = deviceExtension->PowerIrp;

	// We will return the status set by the PDO for the power request we're completing
    ntStatus = IoStatus->Status;

    UUSBD_KdPrint( DBGLVL_HIGH,("Enter Uusbd_PoRequestCompletion()\n"));

    // we should not be in the midst of handling a self-generated power irp
    UUSBD_ASSERT( !deviceExtension->SelfPowerIrp );


    // Calling PoStartNextPowerIrp() indicates that the driver is finished
    // with the previous power IRP, if any, and is ready to handle the next power IRP.
    // It must be called for every power IRP.Although power IRPs are completed only once,
    // typically by the lowest-level driver for a device, PoStartNextPowerIrp must be called
    // for every stack location. Drivers must call PoStartNextPowerIrp while the current IRP
    // stack location points to the current driver. Therefore, this routine must be called
    // before IoCompleteRequest, IoSkipCurrentStackLocation, and PoCallDriver.

    PoStartNextPowerIrp(irp);

    // we must pass down to the next driver in the stack
	IoSkipCurrentIrpStackLocation(irp);

    // PoCallDriver is used to pass any power IRPs to the PDO instead of IoCallDriver.
    // When passing a power IRP down to a lower-level driver, the caller should use
    // IoSkipCurrentIrpStackLocation or IoCopyCurrentIrpStackLocationToNext to copy the IRP to
    // the next stack location, then call PoCallDriver. Use IoCopyCurrentIrpStackLocationToNext
    // if processing the IRP requires setting a completion routine, or IoSkipCurrentStackLocation
    // if no completion routine is needed.

    PoCallDriver(deviceExtension->TopOfStackDeviceObject,
                 irp);

    Uusbd_DecrementIoCount(deviceObject);

    UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_PoRequestCompletion() Exit IRP_MN_SET_POWER\n"));

    deviceExtension->PowerIrp = NULL;

    return ntStatus;
}




NTSTATUS
Uusbd_PowerIrp_Complete(
    IN PDEVICE_OBJECT NullDeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:

	UTK_ProcessPowerIrp()でデバイスワパーステートを変更する
	IRP_MN_SET_POWERをうけとり、デバイスのパワーステートを
	PowerDeviceD0以外からPowerDeviceD0に変更するときにIRPの完了
	ルーチンとしてこのルーチンをセットする

Arguments:

    DeviceObject - Pointer to the device object for the class device.

    Irp - Irp completed.

    Context - Driver defined context.

Return Value:

    The function value is the final status from the operation.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT deviceObject;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;

    UUSBD_KdPrint( DBGLVL_HIGH,("enter Uusbd_PowerIrp_Complete\n"));

    deviceObject = (PDEVICE_OBJECT) Context;

    deviceExtension = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;
    //  If the lower driver returned PENDING, mark our stack location as pending also.
    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    // We can assert that we're a  device powerup-to D0 request,
    // because that was the only type of request we set a completion routine
    // for in the first place
    UUSBD_ASSERT(irpStack->MajorFunction == IRP_MJ_POWER);
    UUSBD_ASSERT(irpStack->MinorFunction == IRP_MN_SET_POWER);
    UUSBD_ASSERT(irpStack->Parameters.Power.Type==DevicePowerState);
    UUSBD_ASSERT(irpStack->Parameters.Power.State.DeviceState==PowerDeviceD0);

    // Now that we know we've let the lower drivers do what was needed to power up,
    //  we can set our device extension flags accordingly
    deviceExtension->CurrentDevicePowerState = PowerDeviceD0;

    Irp->IoStatus.Status = ntStatus;

    Uusbd_DecrementIoCount(deviceObject);
	PoStartNextPowerIrp(Irp);

    UUSBD_KdPrint( DBGLVL_MEDIUM,("exit Uusbd_PowerIrp_Complete Exit IRP_MN_SET_POWER D0 complete\n"));
    return ntStatus;
}



NTSTATUS
Uusbd_SelfSuspendOrActivate(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN fSuspend
    )
/***

Routine Description:

        Called on Uusbd_PnPAddDevice() to power down until needed (i.e., till a pipe is actually opened).
        Called on Uusbd_Create() to power up device to D0 before opening 1st pipe.
        Called on Uusbd_Close() to power down device if this is the last pipe.

Arguments:

    DeviceObject - Pointer to the device object

    fSuspend; TRUE to Suspend, FALSE to acivate.


Return Value:

    If the operation is not attemtped, SUCCESS is returned.
    If the operation is attemtped, the value is the final status from the operation.

***/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    POWER_STATE PowerState;
    PDEVICE_EXTENSION deviceExtension;


    deviceExtension = DeviceObject->DeviceExtension;
    UUSBD_KdPrint( DBGLVL_MAXIMUM,("Enter Uusbd_SelfSuspendOrActivate(),fSuspend = %d\n", fSuspend));


	// Can't accept request if:
    //  1) device is removed, 
    //  2) has never been started, 
    //  3) is stopped,
    //  4) has a remove request pending,
    //  5) has a stop device pending
    if ( !Uusbd_CanAcceptIoRequests( DeviceObject ) ) {
        ntStatus = STATUS_DELETE_PENDING;
        
		UUSBD_KdPrint( DBGLVL_MEDIUM,("ABORTING Uusbd_SelfSuspendOrActivate()\n"));
        return ntStatus;
    }
  

    // don't do anything if any System-generated Device Pnp irps are pending
    if ( NULL != deviceExtension->PowerIrp ) {
        UUSBD_KdPrint( DBGLVL_MAXIMUM,("Exit Uusbd_SelfSuspendOrActivate(),refusing on pending deviceExtension->PowerIrp 0x%x\n", deviceExtension->PowerIrp));
        return ntStatus;
    }

    // don't do anything if any self-generated Device Pnp irps are pending
    if ( deviceExtension->SelfPowerIrp ) {
        UUSBD_KdPrint( DBGLVL_MAXIMUM,("Exit Uusbd_SelfSuspendOrActivate(),refusing on pending deviceExtension->SelfPowerIrp\n" ));
        return ntStatus;
    }

    // don't auto-suspend if any pipes are open
    if ( fSuspend && ( 0 != deviceExtension->OpenPipeCount || deviceExtension->OpenBaseDevice) ) {
        UUSBD_KdPrint( DBGLVL_MAXIMUM,("Exit Uusbd_SelfSuspendOrActivate(),refusing to self-suspend on OpenPipeCount %d\n", deviceExtension->OpenPipeCount));
        return ntStatus;
    }

    // don't auto-activate if no pipes are open
    if ( !fSuspend && ( 0 == deviceExtension->OpenPipeCount && !deviceExtension->OpenBaseDevice) ) {
        UUSBD_KdPrint( DBGLVL_MAXIMUM,("Exit Uusbd_SelfSuspendOrActivate(),refusing to self-activate, no pipes open\n"));
        return ntStatus;
    }

    // dont do anything if registry CurrentControlSet\Services\BulkUsb\Parameters\PowerDownLevel
    //  has been set to  zero, PowerDeviceD0 ( 1 ), or a bogus high value
    if ( ( deviceExtension->PowerDownLevel == PowerDeviceD0 ) || 
         ( deviceExtension->PowerDownLevel == PowerDeviceUnspecified)  ||
         ( deviceExtension->PowerDownLevel >= PowerDeviceMaximum ) ) {
        UUSBD_KdPrint( DBGLVL_MAXIMUM,("Exit Uusbd_SelfSuspendOrActivate(), refusing on deviceExtension->PowerDownLevel == %d\n", deviceExtension->PowerDownLevel));
        return ntStatus;
    }

    if ( fSuspend )
        PowerState.DeviceState = deviceExtension->PowerDownLevel;
    else
        PowerState.DeviceState = PowerDeviceD0;  // power up all the way; we're probably just about to do some IO

	/*
	Windows98では、このドライバーの下層のドライバーにPoCallDriverを
	呼びそれが完了していない時に、PoCallDriveを呼ぶと初めのIRPが完了しない
	これを防ぐため、mutexを使いシリアライズする。
	*/
	KeWaitForSingleObject(mutex_pocall, Executive, KernelMode, FALSE ,NULL);
    ntStatus = Uusbd_SelfRequestPowerIrp( DeviceObject, PowerState );
	KeReleaseMutex(mutex_pocall, FALSE);

    UUSBD_KdPrint( DBGLVL_MAXIMUM,("Uusbd_SelfSuspendOrActivate() status 0x%x on setting dev state %s\n", ntStatus, UUSBD_StringForDevState(PowerState.DeviceState ) ));

    return ntStatus;

}


NTSTATUS
Uusbd_SelfRequestPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN POWER_STATE PowerState
    )
/*++

Routine Description:

    This routine is called by Uusbd_SelfSuspendOrActivate() to
    actually make the system request for a powerdown/up to PowerState.
    It first checks to see if we are already in Powerstate and immediately
    returns  SUCCESS with no further processing if so


Arguments:

    DeviceObject - Pointer to the device object

    PowerState. power state requested, e.g PowerDeviceD0.


Return Value:

    The function value is the final status from the operation.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    PIRP pIrp = NULL;

    deviceExtension =  DeviceObject->DeviceExtension;

    // This should have been reset in completion routine
    UUSBD_ASSERT( !deviceExtension->SelfPowerIrp );

    if (  deviceExtension->CurrentDevicePowerState ==  PowerState.DeviceState )
        return STATUS_SUCCESS;  // nothing to do

    UUSBD_KdPrint( DBGLVL_HIGH,("Enter Uusbd_SelfRequestPowerIrp() will request power irp to state %s\n",
        UUSBD_StringForDevState( PowerState.DeviceState )));

    Uusbd_IncrementIoCount(DeviceObject);

	// flag we're handling a self-generated power irp
    deviceExtension->SelfPowerIrp = TRUE;

	// actually request the Irp
    ntStatus = PoRequestPowerIrp(deviceExtension->PhysicalDeviceObject, 
                         IRP_MN_SET_POWER,
                         PowerState,
                         Uusbd_PoSelfRequestCompletion,
                         DeviceObject,
                         NULL);


    if  ( ntStatus == STATUS_PENDING ) { 
        // status pending is the return code we wanted

        // We only need to wait for completion if we're powering up
        if ( (ULONG) PowerState.DeviceState < deviceExtension->PowerDownLevel ) {

            NTSTATUS waitStatus;

            waitStatus = KeWaitForSingleObject(
                           &deviceExtension->SelfRequestedPowerIrpEvent,
                           Suspended,
                           KernelMode,
                           FALSE,
                           NULL);

        }

        ntStatus = STATUS_SUCCESS;

        deviceExtension->SelfPowerIrp = FALSE;

        UUSBD_KdPrint( DBGLVL_HIGH, ("Uusbd_SelfRequestPowerIrp() SUCCESS\n    IRP 0x%x to state %s\n",
            pIrp, UUSBD_StringForDevState(PowerState.DeviceState) ));


    }
    else {
        // The return status was not STATUS_PENDING; any other codes must be considered in error here;
        //  i.e., it is not possible to get a STATUS_SUCCESS  or any other non-error return from this call;
        UUSBD_KdPrint( DBGLVL_HIGH, ("Uusbd_SelfRequestPowerIrp() to state %s FAILED, status = 0x%x\n",
            UUSBD_StringForDevState( PowerState.DeviceState ),ntStatus));
    }

    return ntStatus;
}



NTSTATUS
Uusbd_PoSelfRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    )
/*++

Routine Description:

    This routine is called when the driver completes a self-originated power IRP 
	that was generated by a call to Uusbd_SelfSuspendOrActivate().
    We power down whenever the last pipe is closed and power up when the first pipe is opened.

    For power-up , we set an event in our FDO extension to signal this IRP done
    so the power request can be treated as a synchronous call.
    We need to know the device is powered up before opening the first pipe, for example.
    For power-down, we do not set the event, as no caller waits for powerdown complete.

Arguments:

    DeviceObject - Pointer to the device object for the class device. ( Physical Device Object )

    Context - Driver defined context, in this case our FDO ( functional device object )

Return Value:

    The function value is the final status from the operation.

--*/
{
    PDEVICE_OBJECT deviceObject = Context;
    PDEVICE_EXTENSION deviceExtension = deviceObject->DeviceExtension;
    NTSTATUS ntStatus = IoStatus->Status;

    // we should not be in the midst of handling a system-generated power irp
    UUSBD_ASSERT( NULL == deviceExtension->PowerIrp );

    // We only need to set the event if we're powering up;
    // No caller waits on power down complete
    if ( (ULONG) PowerState.DeviceState < deviceExtension->PowerDownLevel ) {

        // Trigger Self-requested power irp completed event;
        //  The caller is waiting for completion
        KeSetEvent(&deviceExtension->SelfRequestedPowerIrpEvent, 1, FALSE);
    }

    Uusbd_DecrementIoCount(deviceObject);

    UUSBD_KdPrintCond( DBGLVL_HIGH, !NT_SUCCESS(ntStatus),("Exit Uusbd_PoSelfRequestCompletion() FAILED, ntStatus = 0x%x\n", ntStatus ));
   
    return ntStatus;
}


BOOLEAN
Uusbd_SetDevicePowerState(
    IN PDEVICE_OBJECT DeviceObject,
    IN DEVICE_POWER_STATE DeviceState
    )
/*++

Routine Description:

    This routine is called when An IRP_MN_SET_POWER of type 'DevicePowerState'
    has been received by Uusbd_ProcessPowerIrp().


Arguments:

    DeviceObject - Pointer to the device object for the class device.

    DeviceState - Device specific power state to set the device in to.


Return Value:

    For requests to DeviceState D0 ( fully on ), returns TRUE to signal caller 
    that we must set a completion routine and finish there.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    BOOLEAN fRes = FALSE;

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    switch (DeviceState) {
    case PowerDeviceD3:

        //
        // Device will be going OFF, 
		// TODO: add any needed device-dependent code to save state here.
		//  ( We have nothing to do in this sample )
        //

        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_SetDevicePowerState() PowerDeviceD3 (OFF)\n"));

        deviceExtension->CurrentDevicePowerState = DeviceState;
        break;

    case PowerDeviceD1:
    case PowerDeviceD2:
        //
        // power states D1,D2 translate to USB suspend

        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_SetDevicePowerState()  %s\n",
            UUSBD_StringForDevState(DeviceState) ));

        deviceExtension->CurrentDevicePowerState = DeviceState;
        break;

    case PowerDeviceD0:


        UUSBD_KdPrint( DBGLVL_MEDIUM,("Uusbd_SetDevicePowerState() PowerDeviceD0 (ON)\n"));

        // We'll need to finish the rest in the completion routine;
        //   signal caller we're going to D0 and will need to set a completion routine
        fRes = TRUE;

        // Caller will pass on to PDO ( Physical Device object )
        break;

    default:

        UUSBD_KdPrint( DBGLVL_MEDIUM,(" Bogus DeviceState = %x\n", DeviceState));
    }

    return fRes;
}



NTSTATUS
Uusbd_QueryCapabilities(
    IN PDEVICE_OBJECT PdoDeviceObject,
    IN PDEVICE_CAPABILITIES DeviceCapabilities
    )

/*++

Routine Description:

    This routine generates an internal IRP from this driver to the PDO
    to obtain information on the Physical Device Object's capabilities.
    We are most interested in learning which system power states
    are to be mapped to which device power states for honoring IRP_MJ_SET_POWER Irps.

    This is a blocking call which waits for the IRP completion routine
    to set an event on finishing.

Arguments:

    DeviceObject        - Physical DeviceObject for this USB controller.

Return Value:

    NTSTATUS value from the IoCallDriver() call.

--*/

{
    PIO_STACK_LOCATION nextStack;
    PIRP irp;
    NTSTATUS ntStatus;
    KEVENT event;


    // This is a DDK-defined DBG-only macro that ASSERTS we are not running pageable code
    // at higher than APC_LEVEL.
    PAGED_CODE();


    // Build an IRP for us to generate an internal query request to the PDO
    irp = IoAllocateIrp(PdoDeviceObject->StackSize, FALSE);

    if (!irp) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

	//
    // Preinit the device capability structures appropriately. (DDK2KRC3)
	//
    RtlZeroMemory( DeviceCapabilities, sizeof(DEVICE_CAPABILITIES) );
    DeviceCapabilities->Size = sizeof(DEVICE_CAPABILITIES);
    DeviceCapabilities->Version = 1;
    DeviceCapabilities->Address = -1;
    DeviceCapabilities->UINumber = -1;

    // IoGetNextIrpStackLocation gives a higher level driver access to the next-lower
    // driver's I/O stack location in an IRP so the caller can set it up for the lower driver.
    nextStack = IoGetNextIrpStackLocation(irp);
//    UUSBD_ASSERT(nextStack != NULL); (DDK2KRC3)
    nextStack->MajorFunction= IRP_MJ_PNP;
    nextStack->MinorFunction= IRP_MN_QUERY_CAPABILITIES;

    // init an event to tell us when the completion routine's been called
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    // Set a completion routine so it can signal our event when
    //  the next lower driver is done with the Irp
    IoSetCompletionRoutine(irp,
                           Uusbd_IrpCompletionRoutine,
                           &event,  // pass the event as Context to completion routine
                           TRUE,    // invoke on success
                           TRUE,    // invoke on error
                           TRUE);   // invoke on cancellation of the Irp


    // set our pointer to the DEVICE_CAPABILITIES struct
    nextStack->Parameters.DeviceCapabilities.Capabilities = DeviceCapabilities;

	// preset the irp to report not supported (DDK2KRC3)
	irp->IoStatus.Status = STATUS_NOT_SUPPORTED;


    ntStatus = IoCallDriver(PdoDeviceObject,
                            irp);

    UUSBD_KdPrint( DBGLVL_MEDIUM,(" Uusbd_QueryCapabilities() ntStatus from IoCallDriver to PCI = 0x%x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {
       // wait for irp to complete

       KeWaitForSingleObject(
            &event,
            Suspended,
            KernelMode,
            FALSE,
            NULL);
			ntStatus = irp->IoStatus.Status; // (DDK2KRC3)
    }

    // failed? this is probably a bug
    UUSBD_KdPrintCond( DBGLVL_DEFAULT,(!NT_SUCCESS(ntStatus)), ("Uusbd_QueryCapabilities() failed\n"));

    IoFreeIrp(irp);

    return ntStatus;
}








