#include <stdio.h>

#include "..\inc\uusbd.h"
#include <process.h>    /* _beginthread, _endthread */

#define NUM_INTERFACE	0  // dump����C���^�[�t�F�[�X�̔ԍ�
#define NUM_PIPE		0  // dump����p�C�v�̔ԍ�
#define LOOPCOUNT		10 // �J��Ԃ���

USHORT get_max_packet(HUSB husb, DWORD if_num, DWORD pipe_num)
{
	char *buf,*p;
	USHORT max_len;
	DWORD pipe_count, len;
	BOOL ok;
	USB_CONFIGURATION_DESCRIPTOR conf;
	PUSB_ENDPOINT_DESCRIPTOR endp_descriptor;
	//�܂��R���t�B�O���[�V�����f�B�X�N���v�^�[�̂ݓ��đS�̂̑傫����m��
	ok = Uusbd_GetConfigurationDescriptor(husb,(char*)&conf, sizeof(conf));
	if(!ok) return 0;
	len = conf.wTotalLength;
	buf = (char*)malloc(len);
	//�S�̂𓾂�
	ok = Uusbd_GetConfigurationDescriptor(husb,buf, len);
	if(!ok) return 0;

	p = buf; max_len = 0; pipe_count = 0; ok = FALSE;
	while(len>0) {
		if(p[1] == USB_INTERFACE_DESCRIPTOR_TYPE){ // interface descriptor
			if((unsigned) p[2] == if_num) ok = TRUE;
			else ok = FALSE;
			pipe_count = 0;
		}
		if(p[1] == USB_ENDPOINT_DESCRIPTOR_TYPE && ok){ // endpoint descriptor
			if(pipe_count == pipe_num) { // found !!
				endp_descriptor = (PUSB_ENDPOINT_DESCRIPTOR)p;
				max_len = endp_descriptor->wMaxPacketSize;
				break;
			}
			pipe_count++; // fix Ver1.1
		}
		p += p[0];
		len -= p[0];
	}
	free(buf);
	return max_len;
}

void dumppipe(UCHAR ifnum, UCHAR pipenum, DWORD loopcount, int x)
{
	HUSB husb;
	HANDLE h;
	unsigned char *buf;
	char *logbuf;
	DWORD size, ret, i, j, maxlen;
	husb = Uusbd_Open();
	if(husb == INVALID_HANDLE_VALUE) {
		printf("Uusbd.sys���g����USB�f�o�C�X�͌�����܂���\n");
		logbuf = (char*)malloc(256);
		sprintf(logbuf, "Uusbd.sys���g����USB�f�o�C�X�͌�����܂���\n");
		OutputDebugString(TEXT(logbuf));
		free(logbuf);
		return;
	}
	ret = Uusbd_Check(husb);
	if(ret != UU_CHECK_OK) {
		printf("IOCTL�G���[\n");
		logbuf = (char*)malloc(256);
		sprintf(logbuf, "IOCTL�G���[\n");
		OutputDebugString(TEXT(logbuf));
		free(logbuf);
		return;
	}
	maxlen = get_max_packet(husb, ifnum, pipenum);
	if(maxlen == 0) {
		printf("Can't Get max packet length\n");
		logbuf = (char*)malloc(256);
		sprintf(logbuf, "Can't Get max packet length\n");
		OutputDebugString(TEXT(logbuf));
		free(logbuf);
		return;
	}
	printf("max packet size =%d\n",maxlen);

	buf = (char*)malloc(maxlen);
	h = Uusbd_OpenPipe(husb, ifnum, pipenum);
	Uusbd_ResetDevice(husb);
	for(i=0;i<loopcount;i++){
		memset(buf, 0, maxlen);
		ret = ReadFile(h, buf, maxlen, &size, NULL);
		if(ret ==0 || size == 0) {
			printf("read error ret=%d size=%d\n",ret,size);
			ret = Uusbd_Check(husb);
			if(ret != UU_CHECK_OK) {
				printf("�f�o�C�X�����O����܂���\n");
				break;
			}
			Uusbd_ResetPipe(h);
			Sleep(100);
			continue;
		}
		printf("%05d:",i);
		for(j=0;j<size;j++){
			printf("%02X ", buf[j]);
		}
		printf("\n");
	}

	CloseHandle(h);
	Uusbd_Close(husb);
	free(buf);
}

void main(void)
{
	// �w�肵���p�C�v�̓��e���_���v����
	//dumppipe(NUM_INTERFACE, NUM_PIPE, LOOPCOUNT,1);
	dumppipe(6, NUM_PIPE, LOOPCOUNT,1);
}