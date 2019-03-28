
#include "..\sys\uusbdioctl.h"

// Pipeをアボートする
// hはパイプのハンドル
BOOL AbortPipe(HANDLE h);

// Pipeをリセットする
// hはパイプのハンドル
BOOL ResetPipe(HANDLE h);

// デバイスをリセットする
BOOL ResetDevice(HANDLE h);

// hで指定したハンドルのUSBデバイスのデバイスディスクリプタ-を得る
BOOL GetDeviceDescriptor(HANDLE h, PUSB_DEVICE_DESCRIPTOR descriptor);
BOOL GetConfigurationDescriptor(HANDLE h, char *buf, DWORD siz);
BOOL GetOtherSpeedConfigurationDescriptor(HANDLE h, char *buf, DWORD siz);
BOOL GetDeviceQualifierDescriptor(HANDLE h, PUSB_DEVICE_QUALIFIER_DESCRIPTOR descriptor);

// hにベンダーリクエスト、クラスリクエストを送る
BOOL ClassVendorRequest(HANDLE handle, UCHAR function, BOOL dir_in, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data);
BOOL ClassRequest(HANDLE h, BOOL dir_in, UCHAR recipient, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data);
BOOL VendorRequest(HANDLE h, BOOL dir_in, UCHAR recipient, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data);

BOOL ioctlcheck(HANDLE handle);
BOOL getstatus(HANDLE handle, USHORT type, USHORT index, USHORT *status);
