#include <Windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <direct.h>
#include <thread>


using namespace std;

string killprocess = "TASKKILL /IM svchost.exe /F >nul 2>&1";


void bsod11()
{
	string killprocess = ("TASKKILL /IM svchost.exe /F >nul 2>&1");
}

void debuggerfound1() {
	if (IsDebuggerPresent())
	{
		bsod11();
	}
}

void nignog1() {
	if (FindWindowA(NULL, ("The Wireshark Network Analyzer"))) { bsod11(); }
	if (FindWindowA(NULL, ("Progress Telerik Fiddler Web Debugger"))) { bsod11(); }
	if (FindWindowA(NULL, ("Fiddler"))) { bsod11(); }
	if (FindWindowA(NULL, ("HTTP Debugger"))) { bsod11(); }
	if (FindWindowA(NULL, ("KsDumper"))) { bsod11(); }
	if (FindWindowA(NULL, ("KsDumperClient"))) { bsod11(); }
	if (FindWindowA(NULL, ("x64dbg"))) { bsod11(); }
	if (FindWindowA(NULL, ("dnSpy"))) { bsod11(); }
	if (FindWindowA(NULL, ("FolderChangesView"))) { bsod11(); }
	if (FindWindowA(NULL, ("FolderChangesView (32 bit)"))) { bsod11(); }
	if (FindWindowA(NULL, ("BinaryNinja"))) { bsod11(); }
	if (FindWindowA(NULL, ("HxD"))) { bsod11(); }
	/*if (FindWindowA(NULL, ("Cheat Engine 7.2"))) { bsod11(); }
	if (FindWindowA(NULL, ("Cheat Engine 7.1"))) { bsod11(); }
	if (FindWindowA(NULL, ("Cheat Engine 7.0"))) { bsod11(); }
	if (FindWindowA(NULL, ("Cheat Engine 6.9"))) { bsod11(); }
	if (FindWindowA(NULL, ("Cheat Engine 6.8"))) { bsod11(); }*/
	if (FindWindowA(NULL, ("Ida"))) { bsod11(); }
	if (FindWindowA(NULL, ("Ida Pro"))) { bsod11(); }
	if (FindWindowA(NULL, ("Ida Freeware"))) { bsod11(); }
	if (FindWindowA(NULL, ("HTTP Debugger Pro"))) { bsod11(); }
	if (FindWindowA(NULL, ("Process Hacker"))) { bsod11(); }
	if (FindWindowA(NULL, ("Process Hacker 2"))) { bsod11(); }
	if (FindWindowA(NULL, ("OllyDbg"))) { bsod11(); }
}


void system_no_output1(std::string command)
{
	command.insert(0, "/C ");

	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = "cmd.exe";
	ShExecInfo.lpParameters = command.c_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	if (ShellExecuteExA(&ShExecInfo) == FALSE)

		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	DWORD rv;
	GetExitCodeProcess(ShExecInfo.hProcess, &rv);
	CloseHandle(ShExecInfo.hProcess);
}

void tasky1()
{




	system_no_output1("net stop FACEIT >nul 2>&1");
	system_no_output1("net stop ESEADriver2 >nul 2>&1");
	system_no_output1("sc stop HTTPDebuggerPro >nul 2>&1");
	system_no_output1("sc stop KProcessHacker3 >nul 2>&1");
	system_no_output1("sc stop KProcessHacker2 >nul 2>&1");
	system_no_output1("sc stop KProcessHacker1 >nul 2>&1");
	system_no_output1("sc stop wireshark >nul 2>&1");
	system_no_output1("sc stop npf >nul 2>&1");



}
