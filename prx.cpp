
#include "JM_printf.h"//deferent from release to clean up the code
#include <string.h>
#include <limits.h>
#include <np.h>
#include <fastmath.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#include <sys/process.h>
#include <sysutil/sysutil_msgdialog.h>
#include <sysutil/sysutil_oskdialog.h>
#include <sysutil/sysutil_userinfo.h>
#include <cell/sysmodule.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netex/net.h>
#include <netex/errno.h>
#include <netex/libnetctl.h>
#include <netex/sockinfo.h>
#include <sys/socket.h>

bool CCAPI = true;
bool WebMan = false; //usually I like to check Are you you using ccapi no then are you using webman if still no the notify using game drawing cmd

#include "PS3.h"

SYS_MODULE_INFO(CrackMe, 0, 1, 1);
SYS_MODULE_START(CrackMe_prx_entry);

#define TEXT_SEGMENT 0
#define DATA_SEGMENT 1

#define IW4_MP_STRING_ADDR (const char *)0x588CF8

#define CE_MW2_PATH "/dev_hdd0/tmp/ReflexMW2.sprx"

struct CE_MW2_INFO
{
	sys_prx_id_t prx_id;
	uint32_t text_seg_addr;
	uint32_t data_seg_addr;
	uint32_t toc_addr;
	uint32_t server_load_addr_start_point;
};

CE_MW2_INFO ce_mw2_info;


#define CE_MW2_TEXT_SEGMENT_ADDR_IDA 0x0
#define CE_MW2_DATA_SEGMENT_ADDR_IDA 0x23BC0
#define CE_MW2_TOC_ADDR_IDA 0x2C9A0
//#define CE_MW2_SERVER_ADDR_LOAD_POINT_IDA 0x3B7F8 idk sc58 used that for paradox never worked and I rather write server shit directly to the sprx 
#define CE_MW2_UNHASH_FUNC_ADDR_IDA 0xA30





uint32_t IDAPRXDATA2MEM(uint32_t baseAddr, uint32_t idaAddr)
{
	return baseAddr + (idaAddr - CE_MW2_DATA_SEGMENT_ADDR_IDA);
}




char * UnHash_Stub(char *data)
{
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
}

char * UnHash_Hook(char *data)
{
	char *value = UnHash_Stub(data);
	_sys_printf("pointer %p: Hash: %s | UnHash: %s\n",(0x2CEA0 +(data - ce_mw2_info.data_seg_addr)), data, value);
	return value;
}


sys_ppu_thread_t CrackThreadtest1;
sys_ppu_thread_t CrackThreadtest2;
sys_ppu_thread_t CrackThreadtest3;
sys_ppu_thread_t CrackThreadtest4;
sys_ppu_thread_t CrackThreadtest5;

int timeshooked = 0;
void thread(int r3,sys_ppu_thread_t* r4,int r5 ,void (*r6) (__CSTD uint64_t),int r7,int r8,int r9,int r10)// okay this wasn't used a lot basically his thread was weird so I hooked and use regular threading did I bypass security idk I thought so at first
{
	const char* stringJM;
	if(timeshooked == 0)
		stringJM = "XINIT";
	else
		stringJM = ".";
	timeshooked++;
	_sys_printf("thread( %p , %p , %p , %p , %p , %p , %p , %p )\n",r3, r4, r5, r6, r7, r8, r9, r10);// prints arguments 
	sys_ppu_thread_create(&CrackThreadtest1, r6,0, r7, r8, r9,stringJM);//threads 
	//sys_ppu_thread_create(&CrackThread1, Crack_Thread_1, 0, 100, 0x64, 0, "Crack_Thread_1");

}

void ReflexHook(int r3,int r4,int r5 ,int r6,int r7)//he uses a weird trampoline hookcall bypass that too I use hooks into sprx to steal arguments etc not in release
{
	_sys_printf("HookCall( %p , %p , %p , %p , %p)\n",r3, r4, r5, r6, r7);//prints arguments 
	((void(*)(int ,int,int))&ParseAddr((ce_mw2_info.text_seg_addr +0x2574),ce_mw2_info.toc_addr))( r5, r6, r7);//calls the real hook function in his sprx
}


#include "JM_Memory.h"
void Crack_Thread_1(uint64_t)
{
	DoNotify("Please Wait...");//tell them to wait while loading
	/*The main event is here 
	follow my logic when you make a SPRX you run your code through hooks and threads soo lets not execute his sprx just thread the threads he uses and hook what he hooks
	in ida I searched for his threads and hook after the string Authentication success then I used StartSPRX(ce_mw2_info.prx_id); to execute his sprx and 
	with a valid key to get all the varaiable that he uses to validate our key how you may ask Dump the sprx with text_segment then in Crack_ReflexMw2(); I write what I dumped
	because bypassing his code won't work as he probably has validation in his hooks and threads you can test without Crack_ReflexMw2(); the sprx will work on target manager
	you will see the threads but nothing will happen*/
ReflexBegin1_t.sub = (0xDA4C + ce_mw2_info.text_seg_addr);//here we make opd_s of his threads
ReflexBegin1_t.toc = 0x2C9A0 + ce_mw2_info.text_seg_addr + 0xC440;//TOC of his sprx
ReflexBegin2_t.sub = (0x14F34 + ce_mw2_info.text_seg_addr);
ReflexBegin2_t.toc = 0x2C9A0 + ce_mw2_info.text_seg_addr + 0xC440;
ReflexBegin3_t.sub = (0x16470 + ce_mw2_info.text_seg_addr);
ReflexBegin3_t.toc = 0x2C9A0 + ce_mw2_info.text_seg_addr + 0xC440;
ReflexBegin4_t.sub =  (0x16B04 + ce_mw2_info.text_seg_addr);
ReflexBegin4_t.toc = 0x2C9A0 + ce_mw2_info.text_seg_addr + 0xC440;
ReflexBegin5_t.sub =  (0x16AB8 + ce_mw2_info.text_seg_addr);
ReflexBegin5_t.toc = 0x2C9A0 + ce_mw2_info.text_seg_addr + 0xC440;
	Sleep(10);
	/*here we hook where he hooks in memory*/
	((void(*)(int ,int,int))&ParseAddr((ce_mw2_info.text_seg_addr +0x2574),0x2C9A0 + ce_mw2_info.text_seg_addr + 0xC440))( 0x4c2a0, 0x24960+ ce_mw2_info.text_seg_addr + 0xC440, 0x24830+ ce_mw2_info.text_seg_addr + 0xC440);
	((void(*)(int ,int,int))&ParseAddr((ce_mw2_info.text_seg_addr +0x2574),0x2C9A0 + ce_mw2_info.text_seg_addr + 0xC440))( 0xa1a88, 0x24958+ ce_mw2_info.text_seg_addr + 0xC440, 0x24780+ ce_mw2_info.text_seg_addr + 0xC440);
	((void(*)(int ,int,int))&ParseAddr((ce_mw2_info.text_seg_addr +0x2574),0x2C9A0 + ce_mw2_info.text_seg_addr + 0xC440))( 0x253670, 0x24968+ ce_mw2_info.text_seg_addr + 0xC440, 0x248F0+ ce_mw2_info.text_seg_addr + 0xC440);

	Crack_ReflexMw2();//we download the variables needed for valide key authentication

	Sleep(1000);
	sys_ppu_thread_create(&CrackThreadtest1, ReflexBegin1,0, 0x64, 0x64, 0,"1");//finaly execute his threads
	sys_ppu_thread_create(&CrackThreadtest2, ReflexBegin2,0, 0x64, 0x64, 0,"2");
	sys_ppu_thread_create(&CrackThreadtest3, ReflexBegin3,0, 0x64, 0x64, 0,"3");
	sys_ppu_thread_create(&CrackThreadtest4, ReflexBegin4,0, 0x64, 0x64, 0,"4");
	sys_ppu_thread_create(&CrackThreadtest5, ReflexBegin5,0, 0x64, 0x64, 0,"5");
	
	//}//little advert and warning
	if (DrawOkayMessageDialog("Reflex V2.5 has been cracked by Jo-Milk, Enjoy!\n\nPlease Note I take no responsibility for any problems this SPRX causes to your ps3...\nYou have been warned!\n\n\n\nFor more information visit: www.youtube.com/JoMilk15MoDz <3"))
	{
		DoNotify("Reflex Cracked by Jo-Milk",4);
	}
	//StartSPRX(ce_mw2_info.prx_id);//if you use this hook to unhash function but don't bypass code yet this is only to get dumps
	sys_ppu_thread_exit(0);
}

//first hook 0x2F9E8E



void PatchCE_MW2()
{
	//HookFunctionStart(ce_mw2_info.text_seg_addr + CE_MW2_UNHASH_FUNC_ADDR_IDA, *(uint32_t*)UnHash_Hook, *(uint32_t*)UnHash_Stub);//this is must useful I use this to dump all hashed strings Tip: Authentication success was what I searched in ida to find the hooks 
	sys_ppu_thread_t CrackThread1;
	sys_ppu_thread_create(&CrackThread1, Crack_Thread_1, 0, 100, 0x2000, 0, "Crack_Thread_1");//idk should maybe only use one thread but fuck it why not
	sys_ppu_thread_exit(0);
}

void Crack_Thread_0(uint64_t)
{
	Sleep(15000);
	if (DrawYesNoMessageDialog("Do you want to crack Reflex V2.5?"))//this is my reflex crack source but can use on any sprx
	{
		int connectionStatus;//this is to help the noobs cuz some can't read
		sceNpManagerGetStatus(&connectionStatus);//Are you signed in
	if (connectionStatus == SCE_NP_MANAGER_STATUS_ONLINE)//if yess go further the reason for this is xmb gets slow so you can't sign in afterwards = time out
		PatchCE_MW2();//That's where it gets interesting 
	else
	{
		Buzzer(2);//Using Ring buzzer if not signed in cuz signing in with two sprx loaded = freeze
		DrawOkayMessageDialog("Error!\nYou must be signed in before starting the Game");
	}
	}
	sys_ppu_thread_exit(0);
}




void CrackMeInit()
{
	//add game check here (Are you in game if yes do this)
	  ce_mw2_info.prx_id = LoadSPRX(CE_MW2_PATH);//We load sprx 

		sys_prx_module_info_t info;
		sys_prx_segment_info_t segments[10];
		char filename[SYS_PRX_MODULE_FILENAME_SIZE];
		info.size = sizeof(info);
		info.segments = segments;
		info.segments_num = sizeof(segments) / sizeof(sys_prx_segment_info_t);
		info.filename = filename;
		info.filename_size = sizeof(filename);
		sys_prx_get_module_info(ce_mw2_info.prx_id, NULL, &info);//Get info can be usefull 

		uint32_t textAddr = info.segments[TEXT_SEGMENT].base;//most important
		uint32_t dataAddr = info.segments[DATA_SEGMENT].base;//if you wanted different segments just increment DATA_SEGMENT if you don't understand use ida to understand the different segment

		ce_mw2_info.text_seg_addr = textAddr;
		ce_mw2_info.data_seg_addr = dataAddr;
		ce_mw2_info.toc_addr = IDAPRXDATA2MEM(dataAddr, CE_MW2_TOC_ADDR_IDA);// This don't work as I made a mistake SC58 had the right way of doing it
		_sys_printf("toc_addr: %p data_seg_addr:%p text_seg_addr: %p\n",ce_mw2_info.toc_addr,ce_mw2_info.data_seg_addr,ce_mw2_info.text_seg_addr);//this is removed from release but used to get message in target manager text_seg_addr is where the prx is loaded in memory side note if you ever face a sprx like paradox where it's obfuscated dump the sprx in memory to get the clean code ;) Fuck protection 101 
		sys_ppu_thread_t CrackThread0;
		sys_ppu_thread_create(&CrackThread0, Crack_Thread_0, 0, 100, 0x64, 0, "Crack_Thread_0");//we are in the entry meaning Eboot is handling all the loading we want to thread to free the eboot so it loads the game while we do Evil stuff 

		//Side note This code loads the sprx in memory at ce_mw2_info.text_seg_addr value but it doesn't execute 
		//PS3 now has the sprx loaded in memory will slow down xmb and causes freezes Using this methode on lower cfw might not work
    //2021 fix use page_allocate from kernel to save memory and have better results 
}

extern "C" int CrackMe_prx_entry()
{
	CrackMeInit();
    return SYS_PRX_RESIDENT;
}


/*
Credits Jo-Milk
Sc58 (making this for paradox even if didn't work I found its utility)
Exile (Some info)
BadLuckbrian (teaching me ppc)
BassHaxor (being dope)
*/
