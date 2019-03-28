/***************************************************

Copyright (c) 1999  kashiwano masahiro

Abstract:

    UUSBD.SYS debug functions
    
****************************************************/

#ifndef BUSBDBG_INCD
#define BUSBDBG_INCD

#if DBG

// Defines for debug verbosity level,
//   i.e., test UUSBD_KdPrintLevel( n, format....) against these values
//  Example:
//
// UUSBD_KdPrint( DBGLVL_MEDIUM ,("Entering DriverEntry(), RegistryPath=\n    %ws\n", RegistryPath->Buffer ));
// 
//  The above will print iff gDebugLevel >= DBGLVL_MEDIUM
//
#define DBGLVL_OFF				0		// if gDebugLevel set to this, there is NO debug output	
#define DBGLVL_MINIMUM			1		// minimum verbosity	
#define DBGLVL_DEFAULT			2		// default verbosity level if no registry override
#define DBGLVL_MEDIUM			3		// medium verbosity
#define DBGLVL_HIGH				4		// highest 'safe' level (without severely affecting timing )
#define DBGLVL_MAXIMUM			5		// maximum level, may be dangerous



#ifndef DBGSTR_PREFIX
#define DBGSTR_PREFIX "Uusbd: " 
#endif


// registry path used for parameters global to all instances of the driver
#define UUSBD_REGISTRY_PARAMETERS_PATH  \
	L"\\REGISTRY\\Machine\\System\\CurrentControlSet\\SERVICES\\UUSBD\\Parameters"



#define DPRINT DbgPrint

#define TRAP() DbgBreakPoint();


#define UUSBD_DBGOUTSIZE		512


typedef struct _UUSBD_DBGDATA {

	// mirrors device extension pending io count
	ULONG PendingIoCount;

	// count of pipe errors detected during the life of this device instance
	ULONG PipeErrorCount;

	// count of pipe resets performed during the life of this device instance
	ULONG ResetPipeCount;

	// count of pipe resets performed during the life of this device instance
	ULONG AbortPipeCount;

} UUSBD_DBGDATA, *PUUSBD_DBGDATA;

//these declared in debug 'c' file
extern int gDebugLevel; 
extern int gExAllocCount;
extern PUUSBD_DBGDATA gpDbg; 


static const PCHAR szIrpMajFuncDesc[] =
{  // note this depends on corresponding values to the indexes in wdm.h
   "IRP_MJ_CREATE",
   "IRP_MJ_CREATE_NAMED_PIPE",
   "IRP_MJ_CLOSE",
   "IRP_MJ_READ",
   "IRP_MJ_WRITE",
   "IRP_MJ_QUERY_INFORMATION",
   "IRP_MJ_SET_INFORMATION",
   "IRP_MJ_QUERY_EA",
   "IRP_MJ_SET_EA",
   "IRP_MJ_FLUSH_BUFFERS",
   "IRP_MJ_QUERY_VOLUME_INFORMATION",
   "IRP_MJ_SET_VOLUME_INFORMATION",
   "IRP_MJ_DIRECTORY_CONTROL",
   "IRP_MJ_FILE_SYSTEM_CONTROL",
   "IRP_MJ_DEVICE_CONTROL",
   "IRP_MJ_INTERNAL_DEVICE_CONTROL",
   "IRP_MJ_SHUTDOWN",
   "IRP_MJ_LOCK_CONTROL",
   "IRP_MJ_CLEANUP",
   "IRP_MJ_CREATE_MAILSLOT",
   "IRP_MJ_QUERY_SECURITY",
   "IRP_MJ_SET_SECURITY",
   "IRP_MJ_POWER",          
   "IRP_MJ_SYSTEM_CONTROL", 
   "IRP_MJ_DEVICE_CHANGE",  
   "IRP_MJ_QUERY_QUOTA",    
   "IRP_MJ_SET_QUOTA",      
   "IRP_MJ_PNP"            
};
//IRP_MJ_MAXIMUM_FUNCTION defined in wdm.h


static const PCHAR szPnpMnFuncDesc[] =
{	// note this depends on corresponding values to the indexes in wdm.h 

    "IRP_MN_START_DEVICE",
    "IRP_MN_QUERY_REMOVE_DEVICE",
    "IRP_MN_REMOVE_DEVICE",
    "IRP_MN_CANCEL_REMOVE_DEVICE",
    "IRP_MN_STOP_DEVICE",
    "IRP_MN_QUERY_STOP_DEVICE",
    "IRP_MN_CANCEL_STOP_DEVICE",
    "IRP_MN_QUERY_DEVICE_RELATIONS",
    "IRP_MN_QUERY_INTERFACE",
    "IRP_MN_QUERY_CAPABILITIES",
    "IRP_MN_QUERY_RESOURCES",
    "IRP_MN_QUERY_RESOURCE_REQUIREMENTS",
    "IRP_MN_QUERY_DEVICE_TEXT",
    "IRP_MN_FILTER_RESOURCE_REQUIREMENTS",
    "IRP_MN_READ_CONFIG",
    "IRP_MN_WRITE_CONFIG",
    "IRP_MN_EJECT",
    "IRP_MN_SET_LOCK",
    "IRP_MN_QUERY_ID",
    "IRP_MN_QUERY_PNP_DEVICE_STATE",
    "IRP_MN_QUERY_BUS_INFORMATION",
    "IRP_MN_DEVICE_USAGE_NOTIFICATION",
	"IRP_MN_SURPRISE_REMOVAL"
};

#define IRP_PNP_MN_FUNCMAX	IRP_MN_SURPRISE_REMOVAL



static const PCHAR szSystemPowerState[] = 
{
    "PowerSystemUnspecified",
    "PowerSystemWorking",
    "PowerSystemSleeping1",
    "PowerSystemSleeping2",
    "PowerSystemSleeping3",
    "PowerSystemHibernate",
    "PowerSystemShutdown",
    "PowerSystemMaximum"
};

static const PCHAR szDevicePowerState[] = 
{
    "PowerDeviceUnspecified",
    "PowerDeviceD0",
    "PowerDeviceD1",
    "PowerDeviceD2",
    "PowerDeviceD3",
    "PowerDeviceMaximum"
};




BOOLEAN
Uusbd_GetRegistryDword(
    IN      PWCHAR    RegPath,
    IN      PWCHAR    ValueName,
    IN OUT  PULONG    Value
    );


#define UUSBD_KdPrintCond( ilev, cond, _x_) \
	if( gDebugLevel && ( ilev <= gDebugLevel ) && ( cond )) { \
			DPRINT( DBGSTR_PREFIX ); \
			DPRINT _x_ ; \
	}



#define UUSBD_KdPrint( ilev, _x_)  UUSBD_KdPrintCond( ilev, TRUE, _x_ )


#define UUSBD_TrapCond( ilev, cond ) if ( gDebugLevel && ( ilev <= gDebugLevel ) && (cond) ) TRAP()
#define UUSBD_Trap( ilev )	  UUSBD_TrapCond( ilev, TRUE )


#define UUSBD_ASSERT( cond ) ASSERT( cond )

#define UUSBD_StringForDevState( devState )  szDevicePowerState[ devState ] 

#define UUSBD_StringForSysState( sysState )  szSystemPowerState[ sysState ] 

#define UUSBD_StringForPnpMnFunc( mnfunc ) szPnpMnFuncDesc[ mnfunc ]

#define UUSBD_StringForIrpMjFunc(  mjfunc ) szIrpMajFuncDesc[ mjfunc ]

PVOID 
    UUSBD_ExAllocatePool(
        IN POOL_TYPE PoolType,
        IN ULONG NumberOfBytes
        );


VOID 
    UUSBD_ExFreePool(
        IN PVOID p
        );



#else // if not DBG

// dummy definitions that go away in the retail build

#define UUSBD_KdPrintCond( ilev, cond, _x_) 
#define UUSBD_KdPrint( ilev, _x_)  
#define UUSBD_TrapCond( ilev, cond ) 
#define UUSBD_Trap( ilev )
#define UUSBD_ASSERT( cond )
#define UUSBD_StringForDevState( devState )
#define UUSBD_StringForSysState( sysState ) 
#define UUSBD_StringForPnpMnFunc( mnfunc )
#define UUSBD_StringForIrpMjFunc(  mjfunc ) 

#define UUSBD_ExAllocatePool( typ, siz )  ExAllocatePool( typ, siz )
#define UUSBD_ExFreePool( p )   ExFreePool( p )


#endif //DBG

#endif // included




