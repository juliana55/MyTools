/***

***/
#ifndef UUSBD_INCD
#define UUSBD_INCD

#include "BusbDbg.h"

#define REG_MaxTransferSize L"MaxTransferSize"

typedef struct _UUSBD_RW_CONTEXT UUSBD_RW_CONTEXT, *PUUSBD_RW_CONTEXT;

typedef struct tagUUSBD_PIPE {
	// Pipeをopenした時にcount upする。同時に複数回オープンしないように監視する
    ULONG					PipeOpenCount;

	// pipeのハンドル
	USBD_PIPE_INFORMATION	PipeInfo; 

	// used to save base Irp ( user-originated via IOCTL ) of staged read/write request
	PIRP BaseIrp;

	// save ptr to array of info on self-generated IRPS for staged read/writes;
	//  will allocate this separately
	PUUSBD_RW_CONTEXT PendingIoIrps;

	// count of self-staged irps pending
    ULONG StagedPendingIrpCount;

	// count of self-staged bytes read or written so far
    ULONG StagedBytesTransferred;

	// set when UUSBD_AsyncReadWrite_Complete() finishes or cancels last staged io irp
    KEVENT StagingDoneEvent;

    // spinlock used to protect test of deviceExtension->BaseIrp in
    // UUSBD_StagedReadWrite()
    KSPIN_LOCK FastCompleteSpinlock;

} UUSBD_PIPE, *PUUSBD_PIPE;

typedef struct tagUUSBD_INTERFACEINFO {

	ULONG		NumberOfPipes;
	PUUSBD_PIPE	Pipe;
	
} UUSBD_INTERFACEINFO, *PUUSBD_INTERFACEINFO;

// used to track driver-generated io irps for staged read/write processing
typedef struct _UUSBD_RW_CONTEXT {
    PURB Urb;
    PDEVICE_OBJECT	DeviceObject;
	PUUSBD_PIPE		BasePipe;
    PIRP  Irp;
    PMDL  Mdl;
} UUSBD_RW_CONTEXT, *PUUSBD_RW_CONTEXT;

//
// A structure representing the instance information associated with
// this particular device.
//

typedef struct _DEVICE_EXTENSION {

    // Device object we call when submitting Urbs
    PDEVICE_OBJECT TopOfStackDeviceObject;

	// The bus driver object
    PDEVICE_OBJECT PhysicalDeviceObject;

    DEVICE_POWER_STATE CurrentDevicePowerState;

    // USB configuration handle and ptr for the configuration the
    // device is currently in
    USBD_CONFIGURATION_HANDLE UsbConfigurationHandle;
	PUSB_CONFIGURATION_DESCRIPTOR UsbConfigurationDescriptor;


    // ptr to the USB device descriptor
    // for this device
    PUSB_DEVICE_DESCRIPTOR UsbDeviceDescriptor;

    // we support one interface
    // this is a copy of the info structure
    // returned from select_configuration or
    // select_interface
	//    PUSBD_INTERFACE_INFORMATION *UsbInterface;
	ULONG UsbNumInterface;
	PUUSBD_INTERFACEINFO	UsbInterfaceInfo;

	//Bus drivers set the appropriate values in this structure in response
	//to an IRP_MN_QUERY_CAPABILITIES IRP. Function and filter drivers might
	//alter the capabilities set by the bus driver.
    DEVICE_CAPABILITIES DeviceCapabilities;

	// used to save the currently-being-handled system-requested power irp request
    PIRP PowerIrp;

	// used to save  URB of short, non-staged read/write requests
//	PURB BaseUrb;

	// set when PendingIoCount goes to 0; flags device can be removed
    KEVENT RemoveEvent;

	// set when PendingIoCount goes to 1 ( 1st increment was on add device )
	// this indicates no IO requests outstanding, either user, system, or self-staged
    KEVENT NoPendingIoEvent;

	// set to signal driver-generated power request is finished
    KEVENT SelfRequestedPowerIrpEvent;


	// spinlock used to protect inc/dec iocount logic
	KSPIN_LOCK	IoCountSpinLock;

	// incremented when device is added and any IO request is received;
	// decremented when any io request is completed or passed on, and when device is removed
    ULONG PendingIoCount;

	// count of open pipes
	ULONG OpenPipeCount;

    // ptr to array of structs to track pipeinfo;
    //  in this basic sample it's only used to track if open/closed;
 //   PUUSBD_PIPEINFO *PipeInfo;


    // Name buffer for our named Functional device object link
	// The name is generated based on the driver's class GUID
    WCHAR DeviceLinkNameBuffer[ MAXIMUM_FILENAME_LENGTH ];  // MAXIMUM_FILENAME_LENGTH defined in wdm.h

	//flag set when processing IRP_MN_REMOVE_DEVICE
    BOOLEAN DeviceRemoved;

 	// flag set when driver has answered success to IRP_MN_QUERY_REMOVE_DEVICE
    BOOLEAN RemoveDeviceRequested;

	// flag set when driver has answered success to IRP_MN_QUERY_STOP_DEVICE
    BOOLEAN StopDeviceRequested;

	// flag set when device has been successfully started
	BOOLEAN DeviceStarted;

    // flag set when IRP_MN_WAIT_WAKE is received and we're in a power state
    // where we can signal a wait
    BOOLEAN EnabledForWakeup;

	// used to flag that we're currently handling a self-generated power request
    BOOLEAN SelfPowerIrp;

	// default power state to power down to on self-suspend 
	ULONG PowerDownLevel; 

	// countup when create handle for non pipe  
	// countdown when close handle for non pipe 
	ULONG OpenBaseDevice;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


// function prototypes

NTSTATUS
Uusbd_ProcessPnPIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
Uusbd_ProcessSysControlIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

VOID
Uusbd_Unload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
Uusbd_StartDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
Uusbd_StopDevice(
    IN  PDEVICE_OBJECT DeviceObject,
	IN  BOOLEAN do_unconfig
    );

NTSTATUS
Uusbd_RemoveDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
Uusbd_CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB Urb
    );

NTSTATUS
Uusbd_PnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );

NTSTATUS
Uusbd_CreateDeviceObject(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PDEVICE_OBJECT *DeviceObject
    );

NTSTATUS
Uusbd_ConfigureDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
Uusbd_IrpCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
Uusbd_PoRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    );

NTSTATUS
Uusbd_PoSelfRequestCompletion(
    IN PDEVICE_OBJECT       DeviceObject,
    IN UCHAR                MinorFunction,
    IN POWER_STATE          PowerState,
    IN PVOID                Context,
    IN PIO_STATUS_BLOCK     IoStatus
    );

PURB
Uusbd_BuildAsyncRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PUSBD_PIPE_INFORMATION PipeHandle,
    IN BOOLEAN Read,
	IN ULONG length
    );

NTSTATUS
Uusbd_GetPortStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PULONG PortStatus
    );

NTSTATUS
Uusbd_ResetParentPort(
    IN IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
Uusbd_SelfRequestPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN POWER_STATE PowerState
    );

BOOLEAN
Uusbd_SetDevicePowerState(
    IN PDEVICE_OBJECT DeviceObject,
    IN DEVICE_POWER_STATE DeviceState
    );

NTSTATUS
Uusbd_AsyncReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
Uusbd_SimpleReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );


NTSTATUS
Uusbd_PowerIrp_Complete(
    IN PDEVICE_OBJECT NullDeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
Uusbd_QueryCapabilities(
    IN PDEVICE_OBJECT PdoDeviceObject,
    IN PDEVICE_CAPABILITIES DeviceCapabilities
    );


NTSTATUS
Uusbd_Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
Uusbd_Create(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
Uusbd_Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
Uusbd_AbortPipes(
    IN PDEVICE_OBJECT DeviceObject
    );


NTSTATUS
Uusbd_ProcessIOCTL(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
Uusbd_SelectInterface(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    );



NTSTATUS
Uusbd_ResetDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
Uusbd_Close(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
Uusbd_ResetPipe(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSBD_PIPE_INFORMATION PipeInfo
    );


VOID
Uusbd_IncrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    );

LONG
Uusbd_DecrementIoCount(
    IN PDEVICE_OBJECT DeviceObject
    );   


NTSTATUS
Uusbd_ProcessPowerIrp(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );    


NTSTATUS
Uusbd_StagedReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN BOOLEAN Read
    );

NTSTATUS
Uusbd_SelfSuspendOrActivate(
    IN PDEVICE_OBJECT DeviceObject,
	IN BOOLEAN fSuspend
    );

NTSTATUS 
Uusbd_SymbolicLink(
    IN PDEVICE_OBJECT DeviceObject, 
	IN OUT PUNICODE_STRING deviceLinkUnicodeString
    );


BOOLEAN
Uusbd_CancelPendingIo(
    IN PDEVICE_OBJECT DeviceObject
    );

BOOLEAN
Uusbd_CanAcceptIoRequests(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
Uusbd_VendorRequest_out(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret);

NTSTATUS
Uusbd_VendorRequest_in(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret);

NTSTATUS
Uusbd_GetDescriptor(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret);

NTSTATUS
Uusbd_Check(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret);

NTSTATUS
Uusbd_GetStatus(
    IN PDEVICE_OBJECT DeviceObject,
	IN PVOID inbuff,
	IN ULONG inbuffsize,
	IN PVOID outbuff,
	IN ULONG outbuffsize,
	IN PULONG ret);

NTSTATUS  ReadRegistry(
	IN PDEVICE_OBJECT DeviceObject,
    IN      PWCHAR    ValueName,
	ULONG *val);


NTSTATUS
Uusbd_AbortPipes_sub(PDEVICE_OBJECT DeviceObject, PUUSBD_PIPE pipe);

#endif // already included







