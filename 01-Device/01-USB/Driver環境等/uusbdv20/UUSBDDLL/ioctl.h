
#include "..\sys\uusbdioctl.h"

// Pipe���A�{�[�g����
// h�̓p�C�v�̃n���h��
BOOL AbortPipe(HANDLE h);

// Pipe�����Z�b�g����
// h�̓p�C�v�̃n���h��
BOOL ResetPipe(HANDLE h);

// �f�o�C�X�����Z�b�g����
BOOL ResetDevice(HANDLE h);

// h�Ŏw�肵���n���h����USB�f�o�C�X�̃f�o�C�X�f�B�X�N���v�^-�𓾂�
BOOL GetDeviceDescriptor(HANDLE h, PUSB_DEVICE_DESCRIPTOR descriptor);
BOOL GetConfigurationDescriptor(HANDLE h, char *buf, DWORD siz);
BOOL GetOtherSpeedConfigurationDescriptor(HANDLE h, char *buf, DWORD siz);
BOOL GetDeviceQualifierDescriptor(HANDLE h, PUSB_DEVICE_QUALIFIER_DESCRIPTOR descriptor);

// h�Ƀx���_�[���N�G�X�g�A�N���X���N�G�X�g�𑗂�
BOOL ClassVendorRequest(HANDLE handle, UCHAR function, BOOL dir_in, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data);
BOOL ClassRequest(HANDLE h, BOOL dir_in, UCHAR recipient, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data);
BOOL VendorRequest(HANDLE h, BOOL dir_in, UCHAR recipient, UCHAR bRequest, USHORT wValue, USHORT wIndex, USHORT wLength, char *data);

BOOL ioctlcheck(HANDLE handle);
BOOL getstatus(HANDLE handle, USHORT type, USHORT index, USHORT *status);
