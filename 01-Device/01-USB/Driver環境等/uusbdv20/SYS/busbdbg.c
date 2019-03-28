/***************************************************

Copyright (c) 1999  kashiwano masahiro

Abstract:

    UUSBD.SYS debug functions
    
****************************************************/


#if DBG


#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "uusbd.h"


// begin, data/code  used only in DBG build

//  may be overridden  in registry in DBG buils only
// higher == more verbose, default is 1, 0 turns off all
int gDebugLevel = DBGLVL_DEFAULT ; 

// count outstanding allocations via ExAllocatePool
int gExAllocCount =0;

UUSBD_DBGDATA gDbgBuf = { 0, 0, 0, 0 }; 

// ptr to global debug data struct; txt buffer is only allocated in DBG builds
PUUSBD_DBGDATA gpDbg = &gDbgBuf; 




BOOLEAN
Uusbd_GetRegistryDword(
    IN      PWCHAR    RegPath,
    IN      PWCHAR    ValueName,
    IN OUT  PULONG    Value
    )

/*++

Routine Description:

	Obtain a Dword value from the registry


Arguments:

    RegPath  -- supplies absolute registry path
    ValueName    - Supplies the Value Name.
    Value      - receives the REG_DWORD value.

Return Value:

    TRUE if successfull, FALSE on fail.

--*/

{
    UNICODE_STRING path;
    RTL_QUERY_REGISTRY_TABLE paramTable[2];  //zero'd second table terminates parms
    ULONG lDef = *Value;                     // default
    NTSTATUS status;
    BOOLEAN fres;
	WCHAR wbuf[ MAXIMUM_FILENAME_LENGTH ];

    UUSBD_KdPrint( DBGLVL_HIGH,("Enter Uusbd_GetRegistryDword() RegPath = %ws\n   ValueName =%ws\n", RegPath, ValueName));
    path.Length = 0;
    path.MaximumLength = MAXIMUM_FILENAME_LENGTH * sizeof( WCHAR );  // MAXIMUM_FILENAME_LENGTH defined in wdm.h
    path.Buffer = wbuf;


    RtlZeroMemory(path.Buffer, path.MaximumLength);
    RtlMoveMemory(path.Buffer, RegPath, wcslen( RegPath) * sizeof( WCHAR ));

    UUSBD_KdPrint( DBGLVL_HIGH,("Uusbd_GetRegistryDword() path= %ws\n", path.Buffer ));

    RtlZeroMemory(paramTable, sizeof(paramTable));

    paramTable[0].Flags = RTL_QUERY_REGISTRY_DIRECT;

    paramTable[0].Name = ValueName;

    paramTable[0].EntryContext = Value;
    paramTable[0].DefaultType = REG_DWORD;
    paramTable[0].DefaultData = &lDef;
    paramTable[0].DefaultLength = sizeof(ULONG);


    status = RtlQueryRegistryValues( RTL_REGISTRY_ABSOLUTE | RTL_REGISTRY_OPTIONAL,
                                    path.Buffer, paramTable, NULL, NULL);

    if (NT_SUCCESS(status)) {
        UUSBD_KdPrint( DBGLVL_MEDIUM,("Exit Uusbd_GetRegistryDWord() SUCCESS, value = decimal %d 0x%x\n", *Value, *Value));
        fres = TRUE;

    } else {

        UUSBD_KdPrintCond( DBGLVL_MEDIUM, (status == STATUS_INVALID_PARAMETER) ,("Uusbd_GetRegistryDWord() STATUS_INVALID_PARAMETER\n"));
 
		UUSBD_KdPrintCond( DBGLVL_MEDIUM, (status == STATUS_OBJECT_NAME_NOT_FOUND) ,("Uusbd_GetRegistryDWord() STATUS_OBJECT_NAME_NOT_FOUND\n"));

        fres = FALSE;

    }

    return fres;
}

PVOID 
    UUSBD_ExAllocatePool(
        IN POOL_TYPE PoolType,
        IN ULONG NumberOfBytes
        )
{
	gExAllocCount++;
    UUSBD_KdPrint( DBGLVL_HIGH,("UUSBD_ExAllocatePool() gExAllocCount = dec %d\n", gExAllocCount ));
	return ExAllocatePool(  PoolType, NumberOfBytes );

}


VOID 
    UUSBD_ExFreePool(
        IN PVOID p
        )
{
	gExAllocCount--;
    UUSBD_KdPrint( DBGLVL_HIGH,("UUSBD_ExFreePool() gExAllocCount = dec %d\n", gExAllocCount ));
	ExFreePool(  p );

}


#endif // end , if DBG


