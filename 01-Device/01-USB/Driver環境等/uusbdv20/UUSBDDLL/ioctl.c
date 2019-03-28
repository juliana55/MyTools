#include <windows.h>
#include <assert.h>

#include "devioctl.h"
#include "usbdi.h"

#include "..\inc\uusbd.h"
#include "..\sys\uusbdioctl.h"

//
// PIPEをアボートする
//
// 引数
//	h アボートするパイプのハンドル
//	
// 戻り値
//	NonZero	成功
//  Zero	失敗
BOOL AbortPipe(HANDLE h)
{
	DWORD size;
	BOOL err;
	err = DeviceIoControl(h,
			IOCTL_UUSBD_ABORT_PIPE,
			NULL,
			0,
			NULL,
			0,
			&size,
			NULL);
	return err;
}

//
// PIPEをリセットする
//
// 引数
//	h リセットするパイプのハンドル
//	
// 戻り値
//	NonZero	成功
//  Zero	失敗
BOOL ResetPipe(HANDLE h)
{
	DWORD size;
	BOOL err;
	err = DeviceIoControl(h,
			IOCTL_UUSBD_RESET_PIPE,
			NULL,
			0,
			NULL,
			0,
			&size,
			NULL);
	return err;
}

//
// デバイスをリセットする
//
// 引数
//	h リセットするデバイスのハンドル
//	
// 戻り値
//	NonZero	成功
//  Zero	失敗
BOOL ResetDevice(HANDLE h)
{
	DWORD size;
	BOOL err;
	err = DeviceIoControl(h,
			IOCTL_UUSBD_RESET_DEVICE,
			NULL,
			0,
			NULL,
			0,
			&size,
			NULL);
	return err;
}


// 戻り値
//	NonZero	成功
//  Zero	失敗
BOOL GetDeviceDescriptor(HANDLE h, PUSB_DEVICE_DESCRIPTOR descriptor)
{
	PARAM_GET_DESCRIPTOR param;
	BOOL ret;
	DWORD size;
	if(h == INVALID_HANDLE_VALUE) return FALSE;
	param.DescriptorType = USB_DEVICE_DESCRIPTOR_TYPE;
	param.Index = 0;
	param.LanguageId = 0;
	ret = DeviceIoControl(h,
			IOCTL_UUSBD_GETDESCRIPTOR,
			&param,
			sizeof(param),
			descriptor,
			sizeof(USB_DEVICE_DESCRIPTOR),
			&size,
			NULL);
	if(!ret) {
		DWORD err;
		err = GetLastError();
		return FALSE;
	}
	assert(size == sizeof(USB_DEVICE_DESCRIPTOR));
	return TRUE;
}

BOOL GetConfigurationDescriptor(HANDLE h, char *buf, DWORD siz)
{
	PARAM_GET_DESCRIPTOR param;
	BOOL ret;
	DWORD size;
	if(h == INVALID_HANDLE_VALUE) return FALSE;
	param.DescriptorType = USB_CONFIGURATION_DESCRIPTOR_TYPE;
	param.Index = 0;
	param.LanguageId = 0;
	ret = DeviceIoControl(h,
			IOCTL_UUSBD_GETDESCRIPTOR,
			&param,
			sizeof(param),
			buf,
			siz,
			&size,
			NULL);
	if(!ret) {
		DWORD err;
		err = GetLastError();
		return FALSE;
	}
	return TRUE;
}

BOOL GetOtherSpeedConfigurationDescriptor(HANDLE h, char *buf, DWORD siz)
{
	PARAM_GET_DESCRIPTOR param;
	BOOL ret;
	DWORD size;
	if(h == INVALID_HANDLE_VALUE) return FALSE;
	param.DescriptorType = 7;//Other Speed configuration descriptor;
	param.Index = 0;
	param.LanguageId = 0;
	ret = DeviceIoControl(h,
			IOCTL_UUSBD_GETDESCRIPTOR,
			&param,
			sizeof(param),
			buf,
			siz,
			&size,
			NULL);
	if(!ret) {
		DWORD err;
		err = GetLastError();
		return FALSE;
	}
	return TRUE;
}

BOOL GetDeviceQualifierDescriptor(HANDLE h, PUSB_DEVICE_QUALIFIER_DESCRIPTOR descriptor)
{
	PARAM_GET_DESCRIPTOR param;
	BOOL ret;
	DWORD size;
	if(h == INVALID_HANDLE_VALUE) return FALSE;
	param.DescriptorType = USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE;
	param.Index = 0;
	param.LanguageId = 0;
	ret = DeviceIoControl(h,
			IOCTL_UUSBD_GETDESCRIPTOR,
			&param,
			sizeof(param),
			descriptor,
			sizeof(*descriptor),
			&size,
			NULL);
	if(!ret) {
		DWORD err;
		err = GetLastError();
		return FALSE;
	}
	return TRUE;
}

BOOL ClassVendorRequest(HANDLE handle, UCHAR function, BOOL dir_in, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data)
{
	PARAM_VENDOR_REQUEST *param;
	DWORD len, ret, size;
	if(dir_in) {
		len = sizeof(*param);
		param = malloc(len);
		param->Function = function;
		param->TransferFlags = USBD_SHORT_TRANSFER_OK;
		param->ReservedBits = 0;
		param->Request = bRequest;
		param->Value = wValue;
		param->Index = wIndex;
		ret = DeviceIoControl(handle,
			IOCTL_UUSBD_VENDORREQUEST_IN,
			param,
			len,
			data,
			wLength,
			&size,
			NULL);
		free(param);
	} else {
		len = sizeof(*param)+wLength;
		param = malloc(len);
		param->Function = function;
		param->TransferFlags = 0;USBD_SHORT_TRANSFER_OK;
		param->ReservedBits = 0;
		param->Request = bRequest;
		param->Value = wValue;
		param->Index = wIndex;
		memcpy( ((char*)param)+sizeof(*param), data, wLength);
		ret = DeviceIoControl(handle,
			IOCTL_UUSBD_VENDORREQUEST_OUT,
			param,
			len,
			NULL,
			0,
			&size,
			NULL);
		free(param);
	}
	return ret;
}

BOOL ClassRequest(HANDLE h, BOOL dir_in, UCHAR recipient, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data)
{
	UCHAR function;
	switch(recipient){
	case UU_RECIPIENT_DEVICE:
		function = URB_FUNCTION_CLASS_DEVICE;
		break;
	case UU_RECIPIENT_INTERFACE:
		function = URB_FUNCTION_CLASS_INTERFACE;
		break;
	case UU_RECIPIENT_ENDPOINT:
		function = URB_FUNCTION_CLASS_ENDPOINT;
		break;
	case UU_RECIPIENT_OTHER:
		function = URB_FUNCTION_CLASS_OTHER;
		break;
	default:
		return FALSE;
	}
	return ClassVendorRequest(h, function, dir_in, bRequest, wValue, wIndex, wLength, data);
}

BOOL VendorRequest(HANDLE h, BOOL dir_in, UCHAR recipient, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data)
{
	UCHAR function;
	switch(recipient){
	case UU_RECIPIENT_DEVICE:
		function = URB_FUNCTION_VENDOR_DEVICE;
		break;
	case UU_RECIPIENT_INTERFACE:
		function = URB_FUNCTION_VENDOR_INTERFACE;
		break;
	case UU_RECIPIENT_ENDPOINT:
		function = URB_FUNCTION_VENDOR_ENDPOINT;
		break;
	case UU_RECIPIENT_OTHER:
		function = URB_FUNCTION_VENDOR_OTHER;
		break;
	default:
		return FALSE;
	}
	return ClassVendorRequest(h, function, dir_in, bRequest, wValue, wIndex, wLength, data);
}

BOOL ioctlcheck(HANDLE handle)
{
	DWORD size,ret;
	DWORD status;
	ret = DeviceIoControl(handle,
			IOCTL_UUSBD_CHECK,
			NULL,
			0,
			&status,
			sizeof(status),
			&size,
			NULL);
	return (ret && status);
}

BOOL getstatus(HANDLE handle, USHORT type, USHORT index, USHORT *status)
{
	DWORD size,ret;
	PARAM_GET_STATUS param;
	param.type = type;
	param.Index = index;

	ret = DeviceIoControl(handle,
			IOCTL_UUSBD_GETSTATUS,
			&param,
			sizeof(param),
			status,
			sizeof(USHORT),
			&size,
			NULL);
	return ret;
}
