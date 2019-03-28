#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <setupapi.h>

PSP_INTERFACE_DEVICE_DETAIL_DATA GetDeviceDeatailData(LPGUID pGuid, int index)
{
	HDEVINFO                 hardwareDeviceInfo;
	SP_INTERFACE_DEVICE_DATA deviceInfoData;
	ULONG requiredLength, predictedLength;
	DWORD err;
    PSP_INTERFACE_DEVICE_DETAIL_DATA     functionClassDeviceData = NULL;
	
	functionClassDeviceData = NULL;
	hardwareDeviceInfo = SetupDiGetClassDevs (
		pGuid,
		NULL, // Define no enumerator (global)
		NULL, // Define no
		(DIGCF_PRESENT | // Only Devices present
		DIGCF_INTERFACEDEVICE)); // Function class devices.
	
	if(!hardwareDeviceInfo) goto end;
	
	deviceInfoData.cbSize = sizeof (SP_INTERFACE_DEVICE_DATA);
	
	if (!SetupDiEnumDeviceInterfaces (hardwareDeviceInfo,
		0, // We don't care about specific PDOs
		pGuid,
		index,
		&deviceInfoData)) {
		goto end;
	}
	
    SetupDiGetDeviceInterfaceDetail (
		hardwareDeviceInfo,
		&deviceInfoData,
		NULL, // probing so no output buffer yet
		0, // probing so output buffer length of zero
		&requiredLength,
		NULL); // not interested in the specific dev-node
	
	err = GetLastError();
	assert(err == ERROR_INSUFFICIENT_BUFFER);
	if( err != ERROR_INSUFFICIENT_BUFFER) goto end;
	
    predictedLength = requiredLength;
    functionClassDeviceData = malloc (predictedLength);
    functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);
    if (! SetupDiGetInterfaceDeviceDetail (
		hardwareDeviceInfo,
		&deviceInfoData,
		functionClassDeviceData,
		predictedLength,
		&requiredLength,
		NULL)) {
		free(functionClassDeviceData);
		functionClassDeviceData = NULL;
    }
end:
	SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
	return functionClassDeviceData;
}

TCHAR  *GetDeviceName(LPGUID pGuid, int index)
{
	PSP_INTERFACE_DEVICE_DETAIL_DATA DevDetailData;
	TCHAR *name;

	DevDetailData = GetDeviceDeatailData(pGuid, index);
	
	if(DevDetailData == NULL) return NULL;
	
	name = malloc(_tcslen(DevDetailData->DevicePath)+1);
	
	_tcscpy(name, DevDetailData->DevicePath);
	
	free(DevDetailData);
	return name;
}

