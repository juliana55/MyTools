/***************************************************

Copyright (c) 2001 kashiwano masahiro

Abstract:

    UUSBD.SYS registory read functions
    
****************************************************/

#include "wdm.h"
#include "usbdi.h"
#include "usbdlib.h"
#include "uusbd.h"

NTSTATUS
GetRegistryDword(
    IN      HANDLE DevInstRegKey,
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
    RTL_QUERY_REGISTRY_TABLE paramTable[2];  //zero'd second table terminates parms
    ULONG lDef = *Value;                     // default
    NTSTATUS status;
    BOOLEAN fres;

    RtlZeroMemory(paramTable, sizeof(paramTable));

    paramTable[0].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_REQUIRED;

    paramTable[0].Name = ValueName;

    paramTable[0].EntryContext = Value;
    paramTable[0].DefaultType = REG_DWORD;
    paramTable[0].DefaultData = &lDef;
    paramTable[0].DefaultLength = sizeof(ULONG);


    status = RtlQueryRegistryValues( RTL_REGISTRY_HANDLE,
                                    DevInstRegKey, paramTable, NULL, NULL);

    return status;
}

NTSTATUS  ReadRegistry(
	IN PDEVICE_OBJECT DeviceObject,
    IN      PWCHAR    ValueName,
	ULONG *val)
{
	NTSTATUS status;
	HANDLE DevInstRegKey;

    status = IoOpenDeviceRegistryKey(
		DeviceObject,
		PLUGPLAY_REGKEY_DEVICE ,
		KEY_READ,
	    &DevInstRegKey);
	if(status != STATUS_SUCCESS) return status;
	status = GetRegistryDword(DevInstRegKey, ValueName,val);
	ZwClose(DevInstRegKey);
	return status;
}
