#include <windows.h>
#include <assert.h>

#include "..\inc\uusbd.h"
#include "uusbddll.h"
#include "..\sys\uusbdguid.h"
#include "setup.h"
#include "ioctl.h"


HUSB APIENTRY Uusbd_Open_mask(ULONG flag, UCHAR Class, UCHAR SubClass, USHORT Vendor, USHORT Product, BYTE bcdDevice)
{
	LPGUID pGuid = (LPGUID)&GUID_CLASS_UUSBD;
	ULONG i=0;
	USB_DEVICE_DESCRIPTOR descriptor;
	TCHAR *name;
	HUSB husb;
	HANDLE h_ctrl;
	BOOL ret;
	name = NULL;
	i = 0;

	while(1){
		name = GetDeviceName(pGuid, i);
		i++;
		husb = INVALID_HANDLE_VALUE;
		if(name == NULL) break;
		h_ctrl = OpenDevice(name);
		if( h_ctrl != INVALID_HANDLE_VALUE ) {
			ret = GetDeviceDescriptor(h_ctrl, &descriptor);
			if(ret) {
				if( !((flag & UU_MASK_CLASS)    && Class    != descriptor.bDeviceClass) &&
					!((flag & UU_MASK_SUBCLASS) && SubClass != descriptor.bDeviceSubClass) &&
					!((flag & UU_MASK_VENDOR)   && Vendor   != descriptor.idVendor) &&
					!((flag & UU_MASK_PRODUCT)  && Product  != descriptor.idProduct) &&
					!((flag & UU_MASK_BCDDEVICE)&& bcdDevice!= descriptor.bcdDevice) ){
					husb = add_openlist(name, h_ctrl);
					if(husb != INVALID_HANDLE_VALUE) {
						free(name);
						break;
					}
				}
			}
			CloseHandle(h_ctrl);
		}
		free(name);
	}
	return husb;
}

HUSB APIENTRY Uusbd_Open()
{
	HUSB husb;
	husb = Uusbd_Open_mask(UU_MASK_NO,0,0,0,0,0);
	return husb;
}

void APIENTRY Uusbd_Close(HUSB husb)
{
	delete_openlist((int)husb);
}

HANDLE APIENTRY Uusbd_OpenPipe(HUSB husb, UCHAR interface_num, UCHAR pipe_num)
{
	return OpenPipe(husb, interface_num, pipe_num);
}

BOOL APIENTRY Uusbd_ClassRequest(HUSB husb, BOOL dir_in, UCHAR recipient, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data)
{
	if(!checkhandle(husb)) return FALSE;
	return ClassRequest(get_handle(husb), dir_in, recipient, bRequest, wValue, wIndex, wLength, data);
}

BOOL APIENTRY Uusbd_VendorRequest(HUSB husb, BOOL dir_in, UCHAR recipient, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data)
{
	if(!checkhandle(husb)) return FALSE;
	return VendorRequest(get_handle(husb), dir_in, recipient, bRequest, wValue, wIndex, wLength, data);
}

DWORD APIENTRY Uusbd_Check(HUSB husb)
{
	if(!checkhandle(husb)) return UU_CHECK_NOTOPEN; // not open
	if(!ioctlcheck(get_handle(husb))) return UU_CHECK_NODEVICE; // fail IOCTL
	return UU_CHECK_OK; // OK
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
BOOL APIENTRY Uusbd_ResetPipe(HANDLE handle)
{
	return ResetPipe(handle);
}

//
// デバイスをリセットする
//
// 引数
//	husb リセットするデバイスのハンドル
//	
// 戻り値
//	NonZero	成功
//  Zero	失敗
BOOL APIENTRY Uusbd_ResetDevice(HUSB husb)
{
	return ResetDevice(get_handle(husb));
}

// デバイスディスクリプターを得る
// 戻り値
//	NonZero	成功
//  Zero	失敗
BOOL APIENTRY Uusbd_GetDeviceDescriptor(HUSB husb, PUSB_DEVICE_DESCRIPTOR descriptor)
{
	return GetDeviceDescriptor(get_handle(husb), descriptor);
}

// コンフィグレーションディスクリプターを得る
//	NonZero	成功
//  Zero	失敗
BOOL APIENTRY Uusbd_GetConfigurationDescriptor(HUSB husb, char *buf, DWORD siz)
{
	return GetConfigurationDescriptor(get_handle(husb), buf, siz);
}

//	NonZero	成功
//  Zero	失敗
BOOL APIENTRY Uusbd_GetStatusEndpoint(HUSB husb, USHORT endp_num, USHORT *status)
{
	return getstatus(get_handle(husb), GET_STATUS_FROM_ENDPOINT, endp_num, status);
}

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
		init_dll();
		break;
	case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		close_dll();
		break;
    }
    return TRUE;
}

