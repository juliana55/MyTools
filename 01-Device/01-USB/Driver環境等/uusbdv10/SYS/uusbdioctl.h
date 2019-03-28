/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    BulkUsb.h

Abstract:

    Driver-defined special IOCTL's    

Environment:

    Kernel & user mode

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.
Revision History:

	11/17/97: created

--*/

#ifndef UUSBDH_INC
#define UUSBDH_INC

#define UUSBD_IOCTL_INDEX  0x0000


#define IOCTL_UUSBD_GET_CONFIG_DESCRIPTOR     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   UUSBD_IOCTL_INDEX,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
                                                   
#define IOCTL_UUSBD_RESET_DEVICE   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   UUSBD_IOCTL_INDEX+1,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                              
                                                   
#define IOCTL_UUSBD_RESET_PIPE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   UUSBD_IOCTL_INDEX+2,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_UUSBD_VENDORREQUEST_IN  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   UUSBD_IOCTL_INDEX+3,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_UUSBD_VENDORREQUEST_OUT  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   UUSBD_IOCTL_INDEX+4,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_UUSBD_GETDESCRIPTOR  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   UUSBD_IOCTL_INDEX+5,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_UUSBD_CHECK  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   UUSBD_IOCTL_INDEX+6,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_UUSBD_GETSTATUS  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   UUSBD_IOCTL_INDEX+8,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           
/*
Functionの値

URB_FUNCTION_VENDOR_DEVICE 
	Indicates the URB is a vendor-defined request for a USB device. 
URB_FUNCTION_VENDOR_INTERFACE 
	Indicates the URB is a vendor-defined request for an interface on a USB device. 
URB_FUNCTION_VENDOR_ENDPOINT 
	Indicates the URB is a vendor-defined request for an endpoint, in an interface, on a USB device. 
URB_FUNCTION_VENDOR_OTHER 
	Indicates the URB is a vendor-defined request for a device-defined target. 
URB_FUNCTION_CLASS_DEVICE 
	Indicates the URB is a USB-defined class request for a USB device. 
URB_FUNCTION_CLASS_INTERFACE 
	Indicates the URB is a USB-defined class request for an interface on a USB device. 
URB_FUNCTION_CLASS_ENDPOINT 
	Indicates the URB is a USB-defined class request for an endpoint, in an interface, on a USB device. 
URB_FUNCTION_CLASS_OTHER 
	Indicates the URB is a USB-defined class request for a device-defined target. 
*/
typedef struct tagPARAM_VENDOR_REQUEST {
	IN USHORT Function;
	IN ULONG TransferFlags;		// 指定長より実際の転送長が短くてもエラー
								// としない場合USBD_SHORT_TRANSFER_OKを指定 
	IN UCHAR ReservedBits;
	IN UCHAR Request;
	IN USHORT Value;
	IN USHORT Index;
} PARAM_VENDOR_REQUEST;

typedef struct tagPARAM_PARAM_GET_DESCRIPTOR {
	UCHAR	DescriptorType;		// 次のいずれか
								// USB_DEVICE_DESCRIPTOR_TYPE
								// USB_CONFIGURATION_DESCRIPTOR_TYPE
								// USB_STRING_DESCRIPTOR_TYPE 

	UCHAR	Index;				// Index	
	USHORT	LanguageId;			// DescriptorTypeがUSB_STRING_DESCRIPTOR_TYPE
								// の時のみ有効
} PARAM_GET_DESCRIPTOR;

#define GET_STATUS_FROM_DEVICE		0
#define GET_STATUS_FROM_INTERFACE	1
#define GET_STATUS_FROM_ENDPOINT	2
#define GET_STATUS_FROM_OTHER		3

typedef struct tagPARAM_GET_STATUS {
	USHORT	type;
	USHORT	Index;
} PARAM_GET_STATUS;

#endif // end, #ifndef UUSBDH_INC


