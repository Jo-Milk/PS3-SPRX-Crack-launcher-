CDECL_BEGIN
int _sys_printf(const char *format, ...);
int _sys_vsprintf(char *s, const char *format, va_list arg);
CDECL_END
 
struct opd_s
{
    uint32_t sub;
    uint32_t toc;
};
 
opd_s ParseAddr(int Address,int TOC) {
    opd_s GLS = { Address, TOC };
    return GLS;
}
 
opd_s ReflexBegin1_t = { (0xDA4C + 0x25e0000), 0x2618de0 };
void(*ReflexBegin1)(uint64_t) = (void(*)(uint64_t))&ReflexBegin1_t;
opd_s ReflexBegin2_t = { (0x14F34 + 0x25e0000), 0x2618de0 };
void(*ReflexBegin2)(uint64_t) = (void(*)(uint64_t))&ReflexBegin2_t;
opd_s ReflexBegin3_t = { (0x16470 + 0x25e0000), 0x2618de0 };
void(*ReflexBegin3)(uint64_t) = (void(*)(uint64_t))&ReflexBegin3_t;
opd_s ReflexBegin4_t = { (0x16B04 + 0x25e0000), 0x2618de0 };
void(*ReflexBegin4)(uint64_t) = (void(*)(uint64_t))&ReflexBegin4_t;
opd_s ReflexBegin5_t = { (0x16AB8 + 0x25e0000), 0x2618de0 };
void(*ReflexBegin5)(uint64_t) = (void(*)(uint64_t))&ReflexBegin5_t;
 
void Sleep(uint64_t ms)
{
    sys_timer_usleep(ms * 1000);
}
 
int32_t sys_dbg_read_process_memory(uint64_t address, void *data, size_t size)
{
    system_call_4(904, (uint64_t)sys_process_getpid(), address, size, (uint64_t)data);
    return_to_user_prog(int32_t);
}
 
template<typename T>
int32_t ReadProcessMemory(uint32_t address, T data, size_t size)
{
    return sys_dbg_read_process_memory(address, &data, size);
}
 
int32_t sys_dbg_write_process_memory(uint64_t address, const void *data, size_t size)
{
    system_call_4(905, (uint64_t)sys_process_getpid(), address, size, (uint64_t)data);
    return_to_user_prog(int32_t);
}
void SetMemory(int Address,char* bytes,int length)
{
        for (int i = 0; i < length; i++)
        {
            *(char*)(Address + (i)) = bytes[i];
        }
}
template<typename T>
int32_t WriteProcessMemory(uint32_t address, const T value, size_t size)
{
    return sys_dbg_write_process_memory(address, &value, size);
}
 
void HookFunctionStart(uint32_t functionStartAddress, uint32_t newFunction, uint32_t functionStub)
{
    uint32_t normalFunctionStub[8], hookFunctionStub[4];
    sys_dbg_read_process_memory(functionStartAddress, normalFunctionStub, 0x10);
    normalFunctionStub[4] = 0x3D600000 + ((functionStartAddress + 0x10 >> 16) & 0xFFFF);
    normalFunctionStub[5] = 0x616B0000 + (functionStartAddress + 0x10 & 0xFFFF);
    normalFunctionStub[6] = 0x7D6903A6;
    normalFunctionStub[7] = 0x4E800420;
    sys_dbg_write_process_memory(functionStub, normalFunctionStub, 0x20);
    hookFunctionStub[0] = 0x3D600000 + ((newFunction >> 16) & 0xFFFF);
    hookFunctionStub[1] = 0x616B0000 + (newFunction & 0xFFFF);
    hookFunctionStub[2] = 0x7D6903A6;
    hookFunctionStub[3] = 0x4E800420;
    sys_dbg_write_process_memory(functionStartAddress, hookFunctionStub, 0x10);
}
 
void UnHookFunctionStart(uint32_t functionStartAddress, uint32_t functionStub)
{
    uint32_t normalFunctionStub[4];
    sys_dbg_read_process_memory(functionStub, normalFunctionStub, 0x10);
    sys_dbg_write_process_memory(functionStartAddress, normalFunctionStub, 0x10);
}
 
int32_t HookFunction(uint32_t address, uint32_t function)
{
    uint32_t opcode[4];
    opcode[0] = 0x3D600000 + ((function >> 16) & 0xFFFF);
    opcode[1] = 0x616B0000 + (function & 0xFFFF);
    opcode[2] = 0x7D6903A6;
    opcode[3] = 0x4E800420;
    return sys_dbg_write_process_memory(address, &opcode, 0x10);
}
 
int32_t HookFunctionLinked(uint32_t address, uint32_t function)
{
    uint32_t opcode[4];
    opcode[0] = 0x3D600000 + ((function >> 16) & 0xFFFF);
    opcode[1] = 0x616B0000 + (function & 0xFFFF);
    opcode[2] = 0x7D6903A6;
    opcode[3] = 0x4E800421;
    return sys_dbg_write_process_memory(address, &opcode, 0x10);
}
 
int32_t BranchLinkedHook(uint32_t branchFrom, uint32_t branchTo)
{
    uint32_t branch;
    if (branchTo > branchFrom)
        branch = 0x48000001 + (branchTo - branchFrom);
    else
        branch = 0x4C000001 - (branchFrom - branchTo);
    return sys_dbg_write_process_memory(branchFrom, &branch, 4);
}
 
int32_t BranchHook(uint32_t branchFrom, uint32_t branchTo)
{
    uint32_t branch;
    if (branchTo > branchFrom)
        branch = 0x48000000 + (branchTo - branchFrom);
    else
        branch = 0x4C000000 - (branchFrom - branchTo);
    return sys_dbg_write_process_memory(branchFrom, &branch, 4);
}
 
bool yesno_dialog_result = false;
bool yesno_dialog_input = false;
 
void YesNoDialogCallBack(int button_type, void *userdata)
{
    switch (button_type)
    {
    case CELL_MSGDIALOG_BUTTON_YES:
        yesno_dialog_result = true;
        break;
    case CELL_MSGDIALOG_BUTTON_NO:
        yesno_dialog_result = false;
        break;
    }
    yesno_dialog_input = false;
}
 
bool DrawYesNoMessageDialog(const char *format, ...)
{
    va_list argptr;
    char text[CELL_MSGDIALOG_STRING_SIZE];
    va_start(argptr, format);
    _sys_vsprintf(text, format, argptr);
    va_end(argptr);
    cellMsgDialogOpen2(CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO | CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON | CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, text, YesNoDialogCallBack, NULL, NULL);
    yesno_dialog_input = true;
    while (yesno_dialog_input)
    {
        Sleep(16);
        cellSysutilCheckCallback();
    }
    return yesno_dialog_result;
}
 
bool ok_dialog_result = false;
bool ok_dialog_input = false;
 
void OkDialogCallBack(int button_type, void *userdata)
{
    switch (button_type)
    {
    case CELL_MSGDIALOG_BUTTON_OK:
        ok_dialog_result = true;
        break;
    }
    ok_dialog_input = false;
}
 
bool DrawOkayMessageDialog(const char *format, ...)
{
    va_list argptr;
    char text[CELL_MSGDIALOG_STRING_SIZE];
    va_start(argptr, format);
    _sys_vsprintf(text, format, argptr);
    va_end(argptr);
    cellMsgDialogOpen2(CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK | CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON | CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_OK, text, OkDialogCallBack, NULL, NULL);
    ok_dialog_input = true;
    while (ok_dialog_input)
    {
        Sleep(16);
        cellSysutilCheckCallback();
    }
    return ok_dialog_result;
}
 
sys_prx_id_t LoadSPRX(const char *path)
{
    return sys_prx_load_module(path, 0, NULL);
}
 
void StartSPRX(sys_prx_id_t prxID)
{
    int modres;
    sys_prx_start_module(prxID, 0, NULL, &modres, 0, NULL);
}
 
static int connect_to_CAPPI(void)
{
 struct sockaddr_in sin;
 int s;
 
 sin.sin_family = AF_INET;
 sin.sin_addr.s_addr = 0x7F000001; //127.0.0.1 (localhost)
 sin.sin_port = htons(6333);
 s = socket(AF_INET, SOCK_STREAM, 0);
 if (s < 0)
 {
  return -1;
 }
 
 if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
 {
  return -1;
 }
 
 return s;
}
 
 
static int connect_to_webman(void)
{
 struct sockaddr_in sin;
 int s;
 
 sin.sin_family = AF_INET;
 sin.sin_addr.s_addr = 0x7F000001; //127.0.0.1 (localhost)
 sin.sin_port = htons(80);         //http port (80)
 s = socket(AF_INET, SOCK_STREAM, 0);
 if (s < 0)
 {
  return -1;
 }
 
 if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
 {
  return -1;
 }
 
 return s;
}
 
static void sclose(int *socket_e)
{
 if (*socket_e != -1)
 {
  shutdown(*socket_e, SHUT_RDWR);
  socketclose(*socket_e);
  *socket_e = -1;
 }
}
 
 
static void send_wm_request(const char *cmd)
{
 // send command
 int conn_s = -1;
 conn_s = connect_to_webman();
 
 struct timeval tv;
 tv.tv_usec = 0;
 tv.tv_sec = 3;
 setsockopt(conn_s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
 setsockopt(conn_s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
 
 if (conn_s >= 0)
 {
  char wm_cmd[1048];
  int cmd_len = sprintf(wm_cmd, "GET %s HTTP/1.0\r\n", cmd);
  send(conn_s, wm_cmd, cmd_len, 0);
  sclose(&conn_s);
 }
}
 
static void send_CCAPI_request(const char *cmd)
{
 // send command
 int conn_s = -1;
 conn_s = connect_to_CAPPI();
 
 struct timeval tv;
 tv.tv_usec = 0;
 tv.tv_sec = 3;
 setsockopt(conn_s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
 setsockopt(conn_s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
 
 if (conn_s >= 0)
 {
  char wm_cmd[1048];
  int cmd_len = sprintf(wm_cmd, "GET %s HTTP/1.0\r\n", cmd);
  send(conn_s, wm_cmd, cmd_len, 0);
  sclose(&conn_s);
 }
}
 
void fixSpaces(char** szReturn) {
 char *str = *szReturn;
 char _returnStr[512];// = (char*)Malloc(128);
 
 _returnStr[0] = '\0';
 
 int dwStrIndex = 0;
 
 for (int i = 0; i < strlen(str) + 1; i++) {
  if (str[i] != 0x20) {
   _returnStr[dwStrIndex] = str[i];
   dwStrIndex++;
  }
  else
  {
   _returnStr[dwStrIndex] = '%';
   _returnStr[dwStrIndex + 1] = '2';
   _returnStr[dwStrIndex + 2] = '0';
   dwStrIndex += 3;
  }
 
 
  _returnStr[dwStrIndex] = '\0';
 }
 
 *szReturn = _returnStr;
}
 
void DoNotify(char *szFormat,int id = 0) {
 char _notify_buffer[512];
 if(CCAPI == true)
 {
 snprintf(_notify_buffer, 512, "/ccapi/notify?id=%i&msg=%s",id,szFormat);
 send_CCAPI_request(_notify_buffer);
 }
 else if(WebMan == true)
 {
 fixSpaces(&szFormat);
 snprintf(_notify_buffer, 512, "/notify.ps3mapi?msg=%s", szFormat);
 send_wm_request(_notify_buffer);
 }
}
 
void ShutDownPS3() {
 char _notify_buffer[512];
 if(CCAPI == true)
 {
 snprintf(_notify_buffer, 512, "/ccapi/shutdown?mode=1");
 send_CCAPI_request(_notify_buffer);
 }
 else if(WebMan == true)
 {
 snprintf(_notify_buffer, 512, "/shutdown.ps3");
 send_wm_request(_notify_buffer);
 }
}
void RestartPS3() {
 char _notify_buffer[512];
 if(CCAPI == true)
 {
 snprintf(_notify_buffer, 512, "/ccapi/shutdown?mode=2");
 send_CCAPI_request(_notify_buffer);
 }
 else if(WebMan == true)
 {
 snprintf(_notify_buffer, 512, "/restart.ps3");
 send_wm_request(_notify_buffer);
 }
}
void Buzzer(int snd = 1)
{
 char _notify_buffer[512];
 if(CCAPI == true)
 {
 snprintf(_notify_buffer, 512, "/ccapi/ringbuzzer?type=%i",snd);
 send_CCAPI_request(_notify_buffer);
 }
 else if(WebMan == true)
 {
 snprintf(_notify_buffer, 512, "/buzzer.ps3mapi?mode=%i",snd);
 send_wm_request(_notify_buffer);
 }
}
 
void AreYouCCAPIorWebMan()
{
    int CCAPI_s = -1;
    CCAPI_s = connect_to_CAPPI();
    if(CCAPI_s != -1)
    {
        CCAPI = true;
        /*WebMan = false;
        NoCW = false;*/
    }
    else
    {
        int web_s = -1;
        web_s= connect_to_webman();
        if(web_s >= 0)
        {
        CCAPI = false;
        /*WebMan = true;
        NoCW = false;*/
        }
        else
        {
            CCAPI = false;
            /*WebMan = false;
            NoCW = true;*/
        }
    }
 
}
