#define MAXUSBDEVICE 128

void init_dll();
void close_dll();
HUSB add_openlist(TCHAR *name, HANDLE h_ctrl);
void delete_openlist(int husb );
HANDLE OpenPipe(HUSB husb, UCHAR interface_num, UCHAR pipe_num, BOOL overlap_enable);
HANDLE OpenDevice(TCHAR *name);
BOOL checkhandle(HUSB h);
HANDLE get_handle(HUSB husb);
BOOL opencheck(TCHAR *name);
HANDLE enter_globalaccess();
void leave_globalaccess(HANDLE h);
