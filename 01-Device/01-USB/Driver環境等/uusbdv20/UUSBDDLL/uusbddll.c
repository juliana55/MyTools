#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <tchar.h>

#include "..\inc\uusbd.h"
#include "uusbddll.h"

typedef struct tagDEVICETABLE {
	TCHAR devicename[MAX_PATH];
	HANDLE h_ctrl;
} DEVICETABLE;

DEVICETABLE table[MAXUSBDEVICE];
CRITICAL_SECTION table_lock;

BOOL table_is_used(DEVICETABLE *table)
{
	return table->h_ctrl !=INVALID_HANDLE_VALUE;
}

BOOL opencheck(TCHAR *name)
{
	int i;
	for(i=0;i<MAXUSBDEVICE;i++){
		if(_tcscmp(table[i].devicename, name) ==0) return TRUE; // opened
	}
	return FALSE;
}


BOOL checkhandle(HUSB h)
{
	return(h !=INVALID_HANDLE_VALUE && table[(int)h].h_ctrl != INVALID_HANDLE_VALUE);
}

HANDLE get_handle(HUSB husb)
{
	assert(checkhandle(husb));
	return table[(int)husb].h_ctrl;
}
	
void init_dll()
{
	int i;
	for(i=0;i<MAXUSBDEVICE;i++){
		table[i].devicename[0] = '\0';
		table[i].h_ctrl = INVALID_HANDLE_VALUE;
	}
	InitializeCriticalSection (&table_lock);
}

void close_dll()
{
	int i;
	for(i=0;i<MAXUSBDEVICE;i++){
		if(table_is_used(&table[i])) delete_openlist(i);
	}
}

HUSB add_openlist(TCHAR *name, HANDLE h_ctrl)
{
	int i;
	HUSB husb;
	husb = INVALID_HANDLE_VALUE;
	assert(!opencheck(name)); 
	EnterCriticalSection(&table_lock);
	if(h_ctrl != INVALID_HANDLE_VALUE) {
		for(i=0;i<MAXUSBDEVICE;i++){ // find free table index
			if(!table_is_used(&table[i])) break;
		}
		if(i!=MAXUSBDEVICE) {
			table[i].h_ctrl = h_ctrl;
			_tcscpy(table[i].devicename, name);
			husb = (HUSB)i;
		}
	}
	LeaveCriticalSection(&table_lock);
	return husb;
}

void delete_openlist(int husb )
{
	assert(table[husb].devicename[0] != '\0');
	assert(table[husb].h_ctrl != INVALID_HANDLE_VALUE);

	EnterCriticalSection(&table_lock);
	if(table[husb].h_ctrl != INVALID_HANDLE_VALUE)
		CloseHandle(table[husb].h_ctrl);

	table[husb].devicename[0] = '\0';
	table[husb].h_ctrl = INVALID_HANDLE_VALUE;

	LeaveCriticalSection(&table_lock);
}


HANDLE OpenPipe(HUSB husb, UCHAR interface_num, UCHAR pipe_num, BOOL overlap_enable)
{
	TCHAR *name;
	HANDLE h;
	DWORD att = 0;
	if(!checkhandle(husb))return INVALID_HANDLE_VALUE;
	name = malloc(MAX_PATH+100);
	_stprintf(name,"%s\\if%03dpipe%03d", table[(ULONG)husb].devicename, interface_num, pipe_num);
	if(overlap_enable) att = FILE_FLAG_OVERLAPPED;
	h = CreateFile(
		name,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, // no SECURITY_ATTRIBUTES structure
		OPEN_EXISTING, // No special create flags
		att,
		NULL); // No template file
	free(name);
	return h;
}

HANDLE OpenDevice(TCHAR *name)
{
	HANDLE h;
	h = CreateFile(
		name,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, // no SECURITY_ATTRIBUTES structure
		OPEN_EXISTING, // No special create flags
		0, // No special attributes
		NULL); // No template file
	return h;
}
