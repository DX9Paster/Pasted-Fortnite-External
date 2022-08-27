#include <iostream>
#include <Windows.h>
#include "../Win32/win_utils.h"
#include "../Protection/xor.hpp"
#include <dwmapi.h>
#include "Main.h"
#include <vector>
#include "../Misc/stdafx.h"
#include "../Defines/define.h"
#include <iostream>
#include <fstream>

#include "../Driver/driver.h"
#include "../Driver/PIDManager.h"
#include <windows.h>
#include <windows.h>
#include <Lmcons.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include "../Imgui/imgui_internal.h"

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <random>
#include "../Imgui/imgui.h"
#include "main header.h"
#include "../Imgui/imgui_internal.h"
#include "../Protection/protect.h"
#include "main.h"


namespace offests {

	static ULONG64 offset_uworld;
	static ULONG64 offest_gname;
	static ULONG64 uworld;
	static ULONG64 persistent_level;
	static ULONG64 game_instance;
	static ULONG64 local_players_array;
	static ULONG64 player_controller;
	static ULONG64 camera_manager;
	static ULONG64 rootcomponent;
	static ULONG64 Pawn;
	static Vector3 relativelocation;

	static ULONG64 actor_array;
	static ULONG32 actor_count;
}

float MiccaX;
float MiccaY;
float MiccaZ;

DWORD_PTR UWorldSig;



//uintptr_t GetBoneMatrix = 0x15612FC;
//Vector3 GetBoneWithRotation(DWORD_PTR Actor, int id)
//{
//	Vector3 output;
//
//	auto mesh = read<uintptr_t>(Actor + 0x318);//0x318
//
//	auto fGetBoneMatrix = reinterpret_cast<void(__fastcall*)(uintptr_t, FMatrix*, int)>(GetBoneMatrix);
//
//	fGetBoneMatrix((__int64)mesh, myMatrix, id);
//
//	output.x = myMatrix->M[3][0];
//	output.y = myMatrix->M[3][1];
//	output.z = myMatrix->M[3][2];
//
//	return output;
//}


inline FTransform GetBoneIndex(DWORD_PTR mesh, int index) { //new
	DWORD_PTR bonearray = read<DWORD_PTR>(mesh + 0x590);
	if (bonearray == NULL) {
		bonearray = read<DWORD_PTR>(mesh + 0x590 + 0x10);
	}
	return read<FTransform>(bonearray + (index * 0x60));
}
__forceinline Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id) {
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = read<FTransform>(mesh + 0x240);
	D3DMATRIX Matrix;

	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}
D3DMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0)) {
	double radPitch = (rot.x * float(M_PI) / 180.f);
	double radYaw = (rot.y * float(M_PI) / 180.f);
	double radRoll = (rot.z * float(M_PI) / 180.f);

	double SP = sinf(radPitch);
	double CP = cosf(radPitch);
	double SY = sinf(radYaw);
	double CY = cosf(radYaw);
	double SR = sinf(radRoll);
	double CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}


Vector3 Camera;

uintptr_t PlayerCameraManager = 0;

float FovAngle;

static int camfov = 50;
static int test1337 = 0;
static float test13372 = 1.19f;
struct CamewaDescwipsion
{
	float FieldOfView;
	Vector3 Rotation;
	Vector3 Location;
};

CamewaDescwipsion UndetectedCamera(__int64 a1)
{
	CamewaDescwipsion VirtualCamera;
	__int64 v1;
	__int64 v6;
	__int64 v7;
	__int64 v8;

	v1 = read<__int64>(Localplayer + 0xC8);
	__int64 v9 = read<__int64>(v1 + 8);

	VirtualCamera.FieldOfView = 80.f / (read<double>(v9 + 0x690) / 1.19f);

	VirtualCamera.Rotation.x = read<double>(v9 + 0x7E0);
	VirtualCamera.Rotation.y = read<double>(a1 + 0x148);

	v6 = read<__int64>(Localplayer + 0x70);
	v7 = read<__int64>(v6 + 0x98);
	v8 = read<__int64>(v7 + 0x180);

	VirtualCamera.Location = read<Vector3>(v8 + 0x20);
	return VirtualCamera;
}

Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{
	CamewaDescwipsion vCamera = UndetectedCamera(Rootcomp);
	vCamera.Rotation.x = (asin(vCamera.Rotation.x)) * (180.0 / M_PI);

	D3DMATRIX tempMatrix = Matrix(vCamera.Rotation);

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - vCamera.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector3((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}







ImFont* gay;




HRESULT DirectXInit(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		exit(4);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontDefault();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	
	

	gay = io.Fonts->AddFontFromFileTTF(E("C:\\Windows\\Fonts\\Arial.ttf"), 14.f);


	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF,
		0x0400, 0x044F,
		0,
	};


	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(p_Device);

	return S_OK;
}bool IsVec3Valid(Vector3 vec3)
{
	return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
}
void SetupWindow()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.lpszClassName = L"Windows IP Over USB PC Service";
	wcex.lpfnWndProc = WinProc;
	RegisterClassEx(&wcex);

	if (hwnd)
	{
		GetClientRect(hwnd, &GameRect);
		POINT xy;
		ClientToScreen(hwnd, &xy);
		GameRect.left = xy.x;
		GameRect.top = xy.y;

		Width = GameRect.right;
		Height = GameRect.bottom;
	}

	MyWnd = CreateWindowExA(NULL, E("Windows IP Over USB PC Service"), E("Windows IP Over USB PC Service"), WS_POPUP, GameRect.left, GameRect.top, GameRect.right, GameRect.bottom, NULL, NULL, wcex.hInstance, NULL);
	SetWindowLong(MyWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(MyWnd, RGB(0, 0, 0), 255, LWA_ALPHA);

	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(MyWnd, &margin);

	ShowWindow(MyWnd, SW_SHOW);
	UpdateWindow(MyWnd);
}
Vector3 AimbotCorrection(float bulletVelocity, float bulletGravity, float targetDistance, Vector3 targetPosition, Vector3 targetVelocity) {
	Vector3 recalculated = targetPosition;
	float gravity = fabs(bulletGravity);
	float time = targetDistance / fabs(bulletVelocity);
	float bulletDrop = (gravity / 250) * time * time;
	recalculated.z += bulletDrop * 120;
	recalculated.x += time * (targetVelocity.x);
	recalculated.y += time * (targetVelocity.y);
	recalculated.z += time * (targetVelocity.z);
	return recalculated;
}

void SetMouseAbsPosition(DWORD x, DWORD y)
{
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	input.mi.dx = x;
	input.mi.dy = y;
	SendInput(1, &input, sizeof(input));
}
static auto move_to(double x, double y) -> void {
	double center_x = (ImGui::GetIO().DisplaySize.x / 2);
	double center_y = (ImGui::GetIO().DisplaySize.y / 2);

	double target_x = 0;
	double target_y = 0;
	if (x != 0.f)
	{
		if (x > center_x)
		{
			target_x = -(center_x - x);
			target_x /= item.Smooth;
			if (target_x + center_x > center_x * 2.f) target_x = 0.f;
		}

		if (x < center_x)
		{
			target_x = x - center_x;
			target_x /= item.Smooth;
			if (target_x + center_x < 0.f) target_x = 0.f;
		}
	}

	if (y != 0.f)
	{
		if (y > center_y)
		{
			target_y = -(center_y - y);
			target_y /= item.Smooth;
			if (target_y + center_y > center_y * 2.f) target_y = 0.f;
		}

		if (y < center_y)
		{
			target_y = y - center_y;
			target_y /= item.Smooth;
			if (target_y + center_y < 0.f) target_y = 0.f;
		}
	}

	SetMouseAbsPosition(static_cast<DWORD>(target_x), static_cast<DWORD>(target_y));
}

static auto move_toaaaaaa(double x, double y) -> void {
	double center_x = (ImGui::GetIO().DisplaySize.x / 2);
	double center_y = (ImGui::GetIO().DisplaySize.y / 2);

	double target_x = 0;
	double target_y = 0;
	if (x != 0.f)
	{
		if (x > center_x)
		{
			target_x = -(center_x - x);
			target_x /= item.Smooth;
			if (target_x + center_x > center_x * 2.f) target_x = 0.f;
		}

		if (x < center_x)
		{
			target_x = x - center_x;
			target_x /= item.Smooth;
			if (target_x + center_x < 0.f) target_x = 0.f;
		}
	}

	if (y != 0.f)
	{
		if (y > center_y)
		{
			target_y = -(center_y - y);
			target_y /= item.Smooth;
			if (target_y + center_y > center_y * 2.f) target_y = 0.f;
		}

		if (y < center_y)
		{
			target_y = y - center_y;
			target_y /= item.Smooth;
			if (target_y + center_y < 0.f) target_y = 0.f;
		}
	}

	float theNum = floor(target_x / item.Smooth);
	float result = theNum / 6.666666666666667f;

	float theNum1 = floor(target_y / item.Smooth);
	float resulte = theNum1 / 6.666666666666667f;
	float result1 = -(resulte);

	write<float>(PlayerController + 0x290 + 0x0, result1);
	write<float>(PlayerController + 0x290 + 0x4, result);
}

double GetCrossDistance(double x1, double y1, double z1, double x2, double y2, double z2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

#define PI 3.14159265358979323846f

typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;

class Color
{
public:
	RGBA red = { 255,0,0,255 };
	RGBA Magenta = { 255,0,255,255 };
	RGBA yellow = { 255,255,0,255 };
	RGBA grayblue = { 128,128,255,255 };
	RGBA green = { 128,224,0,255 };
	RGBA darkgreen = { 0,224,128,255 };
	RGBA brown = { 192,96,0,255 };
	RGBA pink = { 255,168,255,255 };
	RGBA DarkYellow = { 216,216,0,255 };
	RGBA SilverWhite = { 236,236,236,255 };
	RGBA purple = { 144,0,255,255 };
	RGBA Navy = { 88,48,224,255 };
	RGBA skyblue = { 0,136,255,255 };
	RGBA graygreen = { 128,160,128,255 };
	RGBA blue = { 0,96,192,255 };
	RGBA orange = { 255,128,0,255 };
	RGBA peachred = { 255,80,128,255 };
	RGBA reds = { 255,128,192,255 };
	RGBA darkgray = { 96,96,96,255 };
	RGBA Navys = { 0,0,128,255 };
	RGBA darkgreens = { 0,128,0,255 };
	RGBA darkblue = { 0,128,128,255 };
	RGBA redbrown = { 128,0,0,255 };
	RGBA purplered = { 128,0,128,255 };
	RGBA greens = { 25,255,25,140 };
	RGBA envy = { 0,255,255,255 };
	RGBA black = { 0,0,0,255 };
	RGBA neger = { 215, 240, 180, 255 };
	RGBA negernot = { 222, 180, 200, 255 };
	RGBA gray = { 128,128,128,255 };
	RGBA white = { 255,255,255,255 };
	RGBA blues = { 30,144,255,255 };
	RGBA lightblue = { 135,206,250,255 };
	RGBA Scarlet = { 220, 20, 60, 160 };
	RGBA white_ = { 255,255,255,200 };
	RGBA gray_ = { 128,128,128,200 };
	RGBA black_ = { 0,0,0,200 };
	RGBA red_ = { 255,0,0,200 };
	RGBA Magenta_ = { 255,0,255,200 };
	RGBA yellow_ = { 255,255,0,200 };
	RGBA grayblue_ = { 128,128,255,200 };
	RGBA green_ = { 128,224,0,200 };
	RGBA darkgreen_ = { 0,224,128,200 };
	RGBA brown_ = { 192,96,0,200 };
	RGBA pink_ = { 255,168,255,200 };
	RGBA darkyellow_ = { 216,216,0,200 };
	RGBA silverwhite_ = { 236,236,236,200 };
	RGBA purple_ = { 144,0,255,200 };
	RGBA Blue_ = { 88,48,224,200 };
	RGBA skyblue_ = { 0,136,255,200 };
	RGBA graygreen_ = { 128,160,128,200 };
	RGBA blue_ = { 0,96,192,200 };
	RGBA orange_ = { 255,128,0,200 };
	RGBA pinks_ = { 255,80,128,200 };
	RGBA Fuhong_ = { 255,128,192,200 };
	RGBA darkgray_ = { 96,96,96,200 };
	RGBA Navy_ = { 0,0,128,200 };
	RGBA darkgreens_ = { 0,128,0,200 };
	RGBA darkblue_ = { 0,128,128,200 };
	RGBA redbrown_ = { 128,0,0,200 };
	RGBA purplered_ = { 128,0,128,200 };
	RGBA greens_ = { 0,255,0,200 };
	RGBA envy_ = { 0,255,255,200 };

	RGBA glassblack = { 0, 0, 0, 160 };
	RGBA GlassBlue = { 65,105,225,80 };
	RGBA glassyellow = { 255,255,0,160 };
	RGBA glass = { 200,200,200,60 };

	RGBA filled = { 0, 0, 0, 150 };

	RGBA Plum = { 221,160,221,160 };

	RGBA rainbow() {

		static float x = 0, y = 0;
		static float r = 0, g = 0, b = 0;

		if (y >= 0.0f && y < 255.0f) {
			r = 255.0f;
			g = 0.0f;
			b = x;
		}
		else if (y >= 255.0f && y < 510.0f) {
			r = 255.0f - x;
			g = 0.0f;
			b = 255.0f;
		}
		else if (y >= 510.0f && y < 765.0f) {
			r = 0.0f;
			g = x;
			b = 255.0f;
		}
		else if (y >= 765.0f && y < 1020.0f) {
			r = 0.0f;
			g = 255.0f;
			b = 255.0f - x;
		}
		else if (y >= 1020.0f && y < 1275.0f) {
			r = x;
			g = 255.0f;
			b = 0.0f;
		}
		else if (y >= 1275.0f && y < 1530.0f) {
			r = 255.0f;
			g = 255.0f - x;
			b = 0.0f;
		}

		x += item.rainbow_speed; //increase this value to switch colors faster
		if (x >= 255.0f)
			x = 0.0f;

		y += item.rainbow_speed; //increase this value to switch colors faster
		if (y > 1530.0f)
			y = 0.0f;


		return RGBA{ (DWORD)r, (DWORD)g, (DWORD)b, 255 };
	}

};
Color Col;

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}
std::wstring MBytesToWString(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}

void DrawStrokeText(int x, int y, RGBA* color, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}
void DrawStrokeText2(int x, int y, RGBA* color, const std::string str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}
void DrawNewText(int x, int y, RGBA* color, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}
void DrawRect(int x, int y, int w, int h, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0, thickness);
}
void DrawFilledRect(int x, int y, int w, int h, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0);
}
void DrawLeftProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	int G = (255 * m_health / 100);
	int R = 255 - G;
	RGBA healthcol = { R, G, 0, 255 };

	DrawFilledRect(x - (w / 2) - 3, y, thick, (h)*m_health / 100, &healthcol);
}
void DrawRightProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	int G = (255 * m_health / 100);
	int R = 255 - G;
	RGBA healthcol = { R, G, 0, 255 };

	DrawFilledRect(x + (w / 2) - 25, y, thick, (h)*m_health / 100, &healthcol);
}
void DrawString(float fontSize, int x, int y, RGBA* color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 2;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), text.c_str());
}



void DrawCircleFilled(int x, int y, int radius, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}
void DrawCircle(int x, int y, int radius, RGBA* color, int segments)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), segments);
}
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, RGBA* color, float thickne)
{
	ImGui::GetOverlayDrawList()->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickne);
}
void DrawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}
void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}
void DrawCornerBox(int x, int y, int w, int h, int borderPx, RGBA* color)
{
	DrawLine(x + borderPx, y, w / 3, borderPx, color, 1); //top 
	DrawLine(x + w - w / 3 + borderPx, y, w / 3, borderPx, color, 1); //top 
	DrawLine(x, y, borderPx, h / 3, color, 1); //left 
	DrawLine(x, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color, 1); //left 
	DrawLine(x + borderPx, y + h + borderPx, w / 3, borderPx, color, 1); //bottom 
	DrawLine(x + w - w / 3 + borderPx, y + h + borderPx, w / 3, borderPx, color, 1); //bottom 
	DrawLine(x + w + borderPx, y, borderPx, h / 3, color, 1);//right 
	DrawLine(x + w + borderPx, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color, 1);//right 
}
void DrawNormalBox(int x, int y, int w, int h, int borderPx, RGBA* color)
{
	DrawFilledRect(x + borderPx, y, w, borderPx, color); //top 
	DrawFilledRect(x + w - w + borderPx, y, w, borderPx, color); //top 
	DrawFilledRect(x, y, borderPx, h, color); //left 
	DrawFilledRect(x, y + h - h + borderPx * 2, borderPx, h, color); //left 
	DrawFilledRect(x + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	DrawFilledRect(x + w - w + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	DrawFilledRect(x + w + borderPx, y, borderPx, h, color);//right 
	DrawFilledRect(x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color);//right 
}
void DrawLine2(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
{
	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;
	ImGui::GetOverlayDrawList()->AddLine(from, to, ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), thickness);
}
void DrawRectRainbow(int x, int y, int width, int height, float flSpeed, RGBA* color, float& flRainbow)
{
	ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
	}
}

typedef struct _FNlEntity {
	uint64_t Actor;
	int ID;
	uint64_t mesh;
}FNlEntity;

std::vector<FNlEntity> entityList;


class FTextData {
public:
	char pad_0x0000[0x30];  //0x0000
	wchar_t* Name;          //0x0028 
	__int32 Length;         //0x0030 

};

struct FText {
	FTextData* Data;
	char UnknownData[0x10];

	wchar_t* Get() const {
		if (Data) {
			return Data->Name;
		}

		return nullptr;
	}
};

RGBA getColorFromTier(BYTE Tier) {
	RGBA color = { 255.f, 255.f, 255.f, 1.f };

	if (Tier == 1)
		color = { 255, 255, 255, 1 };
	else if (Tier == 2)
		color = { 255, 0, 0, 1 };
	else if (Tier == 3)
		color = { 9, 63, 150, 1 };
	else if (Tier == 4)
		color = { 154, 18, 179, 1 };
	else if (Tier == 5)
		color = { 95, 85, 45, 1 };

	return color;
}

uint64_t CurrentActor;
Vector3 relativelocationA;

uintptr_t curactoridA;

Vector3 LootPos;

wchar_t* LootName;

uintptr_t Tier;

RGBA LootColor;

namespace RenderA
{
	struct color_keys
	{
		ImVec4 value;
		color_keys() { value.x = value.y = value.z = value.y = 0.0f; }
		color_keys(int r, int g, int b, int a = 255) { float sc = 1.0f / 255.0f; value.x = (float)r * sc; value.y = (float)g * sc; value.z = (float)b * sc; value.w = (float)a * sc; }
		color_keys(float r, float g, float b, float a = 1.0f) { value.x = r; value.y = g; value.z = b; value.w = a; }
		color_keys(const ImVec4& col) { value = col; }
	};
	color_keys* col_keys;

	struct vec_2
	{
		int x, y;
	};

	struct Colors
	{
		ImColor red = { 255, 0, 0, 255 };
		ImColor green = { 255, 0, 0, 255 };
		ImColor blue = { 0, 136, 255, 255 };
		ImColor aqua_blue = { 0, 255, 255, 255 };
		ImColor cyan = { 0, 210, 210, 255 };
		ImColor royal_purple = { 102, 0, 255, 255 };
		ImColor dark_pink = { 255, 0, 174, 255 };
		ImColor black = { 0, 0, 0, 255 };
		ImColor white = { 255, 255, 255, 255 };
		ImColor purple = { 255, 0, 255, 255 };
		ImColor yellow = { 255, 255, 0, 255 };
		ImColor orange = { 255, 140, 0, 255 };
		ImColor gold = { 234, 255, 0, 255 };
		ImColor royal_blue = { 0, 30, 255, 255 };
		ImColor dark_red = { 150, 5, 5, 255 };
		ImColor dark_green = { 5, 150, 5, 255 };
		ImColor dark_blue = { 100, 100, 255, 255 };
		ImColor navy_blue = { 0, 73, 168, 255 };
		ImColor light_gray = { 200, 200, 200, 255 };
		ImColor dark_gray = { 150, 150, 150, 255 };
	};
	Colors color;

	void Text(int posx, int posy, ImColor clr, const char* pText, ...)
	{
		va_list va_alist;
		char buf[1024] = { 0 };
		va_start(va_alist, pText);
		_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
		va_end(va_alist);
		std::string textA = WStringToUTF8(MBytesToWString(buf).c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), 14.f, ImVec2(posx, posy), ImColor(clr), textA.c_str());
	}

	void OutlinedText(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 1), ImColor(color.black), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx - 1, posy - 1), ImColor(color.black), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 1), ImColor(color.black), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx - 1, posy - 1), ImColor(color.black), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void ShadowText(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void Rect(int x, int y, int w, int h, ImColor color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0, thickness);
	}

	void RectFilledGradient(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void RectFilled(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void Tab(const char* v, float size_x, float size_y, static int tab_name, int tab_next)
	{
		if (ImGui::Button(v, ImVec2{ size_x, size_y })) tab_name = tab_next;
	}
}

void DrawLString(float fontSize, int x, int y, ImU32 color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 2;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), color, text.c_str());
}


std::string Names2;
std::string Names4;

Vector3 WeakspotPorn;

//void cache()
//{
//	while (true) {
//		
//	}
//}

typedef struct _LootEntity {
	ImDrawList* Renderer;
	std::string name;
	uintptr_t CurrentActor;
	Vector3 LocalRelativeLocation;
}LootEntity;
static std::vector<LootEntity> LootentityList;

static std::string RealGetNameFromFName(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = read<uint64_t>(sdk::module_base + 0xC562740 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
	uint16_t nameEntry = read<uint16_t>(NamePoolChunk);

	int nameLength = nameEntry >> 6;
	char buff[1024];
	if ((uint32_t)nameLength)
	{
		for (int x = 0; x < nameLength; ++x)
		{
			buff[x] = read<char>(NamePoolChunk + 4 + x);
		}
		char* v2 = buff; // rbx
		unsigned int v4 = nameLength;
		unsigned int v5; // eax
		__int64 result; // rax
		int v7; // ecx
		unsigned int v8; // kr00_4
		__int64 v9; // ecx

		v5 = 0;
		result = read<unsigned int>(sdk::module_base + 0xC42D698) >> 5;
		if (v4)
		{
			do
			{
				v7 = *v2++;
				v8 = result ^ (16 * v7) ^ (result ^ (v7 >> 4)) & 0xF;
				result = (unsigned int)(result + 4 * v5++);
				*(v2 - 1) = v8;
			} while (v5 < v4);
		}
		buff[nameLength] = '\0';
		return std::string(buff);
	}
	else
	{
		return "";
	}
}

static std::string GetNameFromFName(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = read<uint64_t>(sdk::module_base + 0xC562740 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED //gname
	if (read<uint16_t>(NamePoolChunk) < 64)
	{
		auto a1 = read<DWORD>(NamePoolChunk + 4);
		return RealGetNameFromFName(a1);
	}
	else
	{
		return RealGetNameFromFName(key);
	}
}

void drAWLOOOPP()
{
	while (true) {

		std::vector<FNlEntity> tmpList;
		std::vector<LootEntity> tmpListA;

		Uworld = read<DWORD_PTR>(sdk::module_base + 0xc525cd8);
		DWORD_PTR Gameinstance = read<DWORD_PTR>(Uworld + 0x190);
		DWORD_PTR LocalPlayers = read<DWORD_PTR>(Gameinstance + 0x38);

		Localplayer = read<DWORD_PTR>(LocalPlayers);
		PlayerController = read<DWORD_PTR>(Localplayer + 0x30);

		LocalPawn = read<DWORD_PTR>(PlayerController + 0x310);

		PlayerState = read<DWORD_PTR>(LocalPawn + 0x290);
		Rootcomp = read<DWORD_PTR>(LocalPawn + 0x188);

		relativelocation = read<DWORD_PTR>(Rootcomp + 0x128);

		if (LocalPawn != 0) {
			localplayerID = read<int>(LocalPawn + 0x18);
		}

		Persistentlevel = read<DWORD_PTR>(Uworld + 0x30);
		DWORD ActorCount = read<DWORD>(Persistentlevel + 0xA0);
		DWORD_PTR AActors = read<DWORD_PTR>(Persistentlevel + 0x98);

		for (int i = 0; i < read<DWORD>(Uworld + (0x148 + sizeof(PVOID))); ++i) {
			uintptr_t ItemLevels = read<uintptr_t>(Uworld + 0x148);
			if (!ItemLevels) return;

			uintptr_t ItemLevel = read<uintptr_t>(ItemLevels + (i * sizeof(uintptr_t)));
			if (!ItemLevel) return;

			for (int i = 0; i < read<DWORD>(ItemLevel + (0x98 + sizeof(PVOID))); ++i) {

				uintptr_t ItemsPawns = read<uintptr_t>(ItemLevel + 0x98);
				uintptr_t CurrentItemPawn = read<uintptr_t>(ItemsPawns + (i * sizeof(uintptr_t)));
				int currentitemid = read<int>(CurrentItemPawn + 0x18);
				auto CurrentItemPawnName = GetNameFromFName(currentitemid);

				if (strstr(CurrentItemPawnName.c_str(), ("FortPickupAthena")) || strstr(CurrentItemPawnName.c_str(), ("Tiered_Chest")) || strstr(CurrentItemPawnName.c_str(), ("Vehicle")) || strstr(CurrentItemPawnName.c_str(), ("Tiered_Ammo")) || strstr(CurrentItemPawnName.c_str(), ("Llama")))
				{
					LootEntity fnlEntity{ };
					fnlEntity.CurrentActor = CurrentItemPawn;
					fnlEntity.name = CurrentItemPawnName;
					tmpListA.push_back(fnlEntity);
				}

				if (strstr(CurrentItemPawnName.c_str(), ("WeakSpot")))
				{
					auto Arootcomp = read<uint64_t>(CurrentItemPawn + 0x188);
					auto Arelativelocation = read<Vector3>(Arootcomp + 0x128);
					WeakspotPorn = ProjectWorldToScreen(Arelativelocation);
					if (item.WeakspotAim && GetAsyncKeyState(item.aimkey)) {
						move_to(WeakspotPorn.x, WeakspotPorn.y);
					}
				}


				//auto gayset = read<float>(CurrentItemPawn + 0x3CE0);
				if (strstr(CurrentItemPawnName.c_str(), ("PlayerPawn"))) {
					FNlEntity fnlEntity{ };
					fnlEntity.Actor = CurrentItemPawn;
					fnlEntity.mesh = read<uint64_t>(CurrentItemPawn + 0x2F0);
					fnlEntity.ID = curactoridA;
					tmpList.push_back(fnlEntity);
				}
			}
		}

		entityList = tmpList;
		LootentityList = tmpListA;
	}
}

void AimAt(DWORD_PTR entity) {
	uint64_t currentactormesh = read<uint64_t>(entity + 0x2F0);
	auto rootHead = GetBoneWithRotation(currentactormesh, 98);
	Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);
	if (rootHeadOut.x != 0 || rootHeadOut.y != 0 || rootHeadOut.z != 0) {
		if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, rootHeadOut.z, Width / 2, Height / 2, Depth / 2) <= item.AimFOV * 1)) {
			move_to(rootHeadOut.x, rootHeadOut.y);
		}
	}
}

void AimAt2(uint64_t entity) {
	auto rootHead = GetBoneWithRotation(entity, 98);
	Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);
	if (rootHeadOut.x != 0 || rootHeadOut.y != 0 || rootHeadOut.z != 0) {
		if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, rootHeadOut.z, Width / 2, Height / 2, Depth / 2) <= item.AimFOV * 1)) {
			move_toaaaaaa(rootHeadOut.x, rootHeadOut.y);
		}
	}
}

void DrawCorneredBoxAAA(int X, int Y, int W, int H, const ImU32& color, int thickness) {
	float lineW = (W / 3);
	float lineH = (H / 3);

	//corners
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}

void DrawSkeleton(DWORD_PTR mesh, ImColor SkelColor)
{

	Vector3 vHeadBone = GetBoneWithRotation(mesh, 98);
	Vector3 vHip = GetBoneWithRotation(mesh, 2);
	Vector3 vHead = GetBoneWithRotation(mesh, 68);
	Vector3 vNeck = GetBoneWithRotation(mesh, 66);
	Vector3 vUpperArmLeft = GetBoneWithRotation(mesh, 93);
	Vector3 vUpperArmRight = GetBoneWithRotation(mesh, 9);
	Vector3 vLeftHand = GetBoneWithRotation(mesh, 62);
	Vector3 vRightHand = GetBoneWithRotation(mesh, 33);
	Vector3 vLeftHand1 = GetBoneWithRotation(mesh, 100);
	Vector3 vRightHand1 = GetBoneWithRotation(mesh, 99);
	Vector3 vRightThigh = GetBoneWithRotation(mesh, 69);
	Vector3 vLeftThigh = GetBoneWithRotation(mesh, 76);
	Vector3 vRightCalf = GetBoneWithRotation(mesh, 72);
	Vector3 vLeftCalf = GetBoneWithRotation(mesh, 79);
	Vector3 vLeftFoot = GetBoneWithRotation(mesh, 85);
	Vector3 vRightFoot = GetBoneWithRotation(mesh, 84);
	Vector3 vHeadBoneOut = ProjectWorldToScreen(vHeadBone);
	Vector3 vHipOut = ProjectWorldToScreen(vHip);
	Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
	Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft);
	Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight);
	Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand);
	Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand);
	Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1);
	Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1);
	Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh);
	Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh);
	Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf);
	Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf);
	Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot);
	Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);
	//ImGui::GetOverlayDrawList()->AddLine(ImVec2(vHeadBoneOut.x, vHeadBoneOut.y), ImVec2(vNeckOut.x, vNeckOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vHipOut.x, vHipOut.y), ImVec2(vNeckOut.x, vNeckOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vNeckOut.x, vNeckOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vNeckOut.x, vNeckOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vLeftHandOut1.x, vLeftHandOut1.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vRightHandOut1.x, vRightHandOut1.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vHipOut.x, vHipOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vHipOut.x, vHipOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftFootOut.x, vLeftFootOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightFootOut.x, vRightFootOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), SkelColor, 1.0f);
}

bool Headd = true;
bool Neck = false;
bool Chest = false;
ImDrawList* Rendererrr = ImGui::GetOverlayDrawList();

bool isVisible(DWORD_PTR mesh)
{
	if (!mesh)
		return false;
	float fLastSubmitTime = read<float>(mesh + 0x330);
	float fLastRenderTimeOnScreen = read<float>(mesh + 0x334);
	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
	return bVisible;
}
int CounterandPrevent;

void SetControlRotation(Vector3 NewRotation, bool bResetCamera = false)
{
	auto SetControlRotation_ = (*(void(__fastcall**)(uintptr_t Controller, Vector3 NewRotation, bool bResetCamera))(read<uint64_t>(PlayerController + 0x6F0)));
	SetControlRotation_(PlayerController, NewRotation, bResetCamera);
}

class FRotator
{
public:
	FRotator() : fPitch(0), fYaw(0), fRoll(0)
	{

	}

	FRotator(float _pitch, float _yaw, float _roll) : fPitch(_pitch), fYaw(_yaw), fRoll(_roll)
	{

	}

	double fPitch;
	double fYaw;
	double fRoll;
};

//static auto Box3D(Vector3 origin, Vector3 extends, RGBA* color, int thickness) -> void
//{
//	origin  = extends / 2.f;
//
//	Vector3 one = origin;
//	Vector3 two = origin; two.x += extends.x;
//	Vector3 three = origin; three.x += extends.x; three.y += extends.y;
//	Vector3 four = origin; four.y += extends.y;
//
//	Vector3 five = one; five.z += extends.z;
//	Vector3 six = two; six.z += extends.z;
//	Vector3 seven = three; seven.z += extends.z;
//	Vector3 eight = four; eight.z += extends.z;
//
//	Vector3 s1 = ProjectWorldToScreen(one);
//	Vector3 s2 = ProjectWorldToScreen(two);
//	Vector3 s3 = ProjectWorldToScreen(three);
//	Vector3 s4 = ProjectWorldToScreen(four);
//	Vector3 s5 = ProjectWorldToScreen(five);
//	Vector3 s6 = ProjectWorldToScreen(six);
//	Vector3 s7 = ProjectWorldToScreen(seven);
//	Vector3 s8 = ProjectWorldToScreen(eight);
//
//	if (!IsVec3Valid(s1))return;
//	if (!IsVec3Valid(s2))return;
//	if (!IsVec3Valid(s3))return;
//	if (!IsVec3Valid(s4))return;
//	if (!IsVec3Valid(s5))return;
//	if (!IsVec3Valid(s6))return;
//	if (!IsVec3Valid(s7))return;
//	if (!IsVec3Valid(s8))return;
//
//	DrawLine(s1.x, s1.y, s2.x, s2.y, color, thickness);
//	DrawLine(s2.x, s2.y, s3.x, s3.y, color, thickness);
//	DrawLine(s3.x, s3.y, s4.x, s4.y, color, thickness);
//	DrawLine(s4.x, s4.y, s1.x, s1.y, color, thickness);
//
//	DrawLine(s5.x, s5.y, s6.x, s6.y, color, thickness);
//	DrawLine(s6.x, s6.y, s7.x, s7.y, color, thickness);
//	DrawLine(s7.x, s7.y, s8.x, s8.y, color, thickness);
//	DrawLine(s8.x, s8.y, s5.x, s5.y, color, thickness);
//
//	DrawLine(s1.x, s1.y, s5.x, s5.y, color, thickness);
//	DrawLine(s2.x, s2.y, s6.x, s6.y, color, thickness);
//	DrawLine(s3.x, s3.y, s7.x, s7.y, color, thickness);
//	DrawLine(s4.x, s4.y, s8.x, s8.y, color, thickness);
//}

void drawlootloop() {

}

void GetKey() {
	if (item.hitboxpos == 0) {
		item.hitbox = 98;
	}
	else if (item.hitboxpos == 1) {
		item.hitbox = 66;
	}
	else if (item.hitboxpos == 2) {
		item.hitbox = 5;
	}

	else if (item.hitboxpos == 3) {
		item.hitbox = 2;
	}
}

static int Tab = 0;

void shortcurts()
{
	if (Key.IsKeyPushing(VK_F5))
	{
		if (menu_key == false)
		{
			menu_key = true;
		}
		else if (menu_key == true)
		{
			menu_key = false;
		}
		Sleep(200);
	}
}


float color_red = 1.;
float color_green = 0;
float color_blue = 0;
float color_random = 0.0;
float color_speed = -10.0;

void ColorChange()
{
	static float Color[3];
	static DWORD Tickcount = 0;
	static DWORD Tickcheck = 0;
	ImGui::ColorConvertRGBtoHSV(color_red, color_green, color_blue, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount >= 1)
	{
		if (Tickcheck != Tickcount)
		{
			Color[0] += 0.001f * color_speed;
			Tickcheck = Tickcount;
		}
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], color_red, color_green, color_blue);
}

ImGuiStyle* dst;
ImFont* icons = nullptr;
ImFont* big = nullptr;
ImFont* bigger = nullptr;
ImFont* g_font = nullptr;

bool TabA(const char* icon, const char* label, const ImVec2& size_arg, const bool selected)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	static float sizeplus = 0.f;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

	auto animcolor3 = ImColor(27, 27, 27, 255);
	auto animcolor2 = ImColor(3, 169, 252, 255);
	auto animcolor = ImColor(24, 24, 24, 255);

	window->DrawList->AddRectFilled({ bb.Min.x,bb.Max.y }, { bb.Max.x,bb.Min.y }, animcolor);

	if (selected)
		window->DrawList->AddRectFilled({ bb.Min.x,bb.Max.y }, { bb.Min.x + 2,bb.Min.y }, animcolor2);

	if (selected)
		window->DrawList->AddRectFilled({ bb.Min.x + 2,bb.Max.y }, { bb.Min.x + 148,bb.Min.y }, animcolor3);

	ImGui::PushFont(big);

	if (selected)
		window->DrawList->AddText({ bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2 }, ImColor(225 / 255.f, 225 / 255.f, 225 / 255.f, 255.f / 255.f), label);

	if (!selected)
		window->DrawList->AddText({ bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2 }, ImColor(75 / 255.f, 75 / 255.f, 75 / 255.f, 255.f / 255.f), label);


	ImGui::PopFont();

	return pressed;
}

bool SubTab(const char* label, const ImVec2& size_arg, const bool selected)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	static float sizeplus = 0.f;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

	auto animcolor = ImColor(3, 169, 252, 255);

	window->DrawList->AddRectFilled({ bb.Max.x,bb.Max.y }, { bb.Min.x,bb.Max.y - 2 }, ImGui::GetColorU32(ImGuiCol_Border));

	if (selected)
		window->DrawList->AddRectFilled({ bb.Max.x,bb.Max.y }, { bb.Min.x,bb.Max.y - 2 }, animcolor);

	if (selected) {
		window->DrawList->AddText({ bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2 }, ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 255.f / 255.f), label);
	}
	else {
		window->DrawList->AddText({ bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2 }, ImColor(200 / 255.f, 200 / 255.f, 200 / 255.f, 255.f / 255.f), label);
	}


	return pressed;
}

namespace L
{
	typedef struct
	{
		DWORD R;
		DWORD G;
		DWORD B;
		DWORD A;
	}RGBA;

	class RBGAColr
	{
	public:
		RGBA red = { 255,0,0,255 };
		RGBA Magenta = { 255,0,255,255 };
		RGBA yellow = { 255,255,0,255 };
		RGBA grayblue = { 128,128,255,255 };
		RGBA green = { 128,224,0,255 };
		RGBA darkgreen = { 0,224,128,255 };
		RGBA brown = { 192,96,0,255 };
		RGBA pink = { 255,168,255,255 };
		RGBA DarkYellow = { 216,216,0,255 };
		RGBA SilverWhite = { 236,236,236,255 };
		RGBA purple = { 144,0,255,255 };
		RGBA Navy = { 88,48,224,255 };
		RGBA skyblue = { 0,136,255,255 };
		RGBA graygreen = { 128,160,128,255 };
		RGBA blue = { 0,96,192,255 };
		RGBA orange = { 255,128,0,255 };
		RGBA peachred = { 255,80,128,255 };
		RGBA reds = { 255,128,192,255 };
		RGBA darkgray = { 96,96,96,255 };
		RGBA Navys = { 0,0,128,255 };
		RGBA darkgreens = { 0,128,0,255 };
		RGBA darkblue = { 0,128,128,255 };
		RGBA redbrown = { 128,0,0,255 };
		RGBA purplered = { 128,0,128,255 };
		RGBA greens = { 0,255,0,255 };
		RGBA envy = { 0,255,255,255 };
		RGBA black = { 0,0,0,255 };
		RGBA gray = { 128,128,128,255 };
		RGBA white = { 255,255,255,255 };
		RGBA blues = { 30,144,255,255 };
		RGBA lightblue = { 135,206,250,160 };
		RGBA Scarlet = { 220, 20, 60, 160 };
		RGBA white_ = { 255,255,255,200 };
		RGBA gray_ = { 128,128,128,200 };
		RGBA black_ = { 0,0,0,200 };
		RGBA red_ = { 255,0,0,200 };
		RGBA Magenta_ = { 255,0,255,200 };
		RGBA yellow_ = { 255,255,0,200 };
		RGBA grayblue_ = { 128,128,255,200 };
		RGBA green_ = { 128,224,0,200 };
		RGBA darkgreen_ = { 0,224,128,200 };
		RGBA brown_ = { 192,96,0,200 };
		RGBA pink_ = { 255,168,255,200 };
		RGBA darkyellow_ = { 216,216,0,200 };
		RGBA silverwhite_ = { 236,236,236,200 };
		RGBA purple_ = { 144,0,255,200 };
		RGBA Blue_ = { 88,48,224,200 };
		RGBA skyblue_ = { 0,136,255,200 };
		RGBA graygreen_ = { 128,160,128,200 };
		RGBA blue_ = { 0,96,192,200 };
		RGBA orange_ = { 255,128,0,200 };
		RGBA pinks_ = { 255,80,128,200 };
		RGBA Fuhong_ = { 255,128,192,200 };
		RGBA darkgray_ = { 96,96,96,200 };
		RGBA Navy_ = { 0,0,128,200 };
		RGBA darkgreens_ = { 0,128,0,200 };
		RGBA darkblue_ = { 0,128,128,200 };
		RGBA redbrown_ = { 128,0,0,200 };
		RGBA purplered_ = { 128,0,128,200 };
		RGBA greens_ = { 0,255,0,200 };
		RGBA envy_ = { 0,255,255,200 };
		RGBA glassblack = { 0, 0, 0, 160 };
		RGBA GlassBlue = { 65,105,225,80 };
		RGBA glassyellow = { 255,255,0,160 };
		RGBA glass = { 200,200,200,60 };
		RGBA Plum = { 221,160,221,160 };
		RGBA neongreen = { 0, 255, 229,0 };

	};
	RBGAColr Color;

	std::string string_To_UTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}
	// Drawings for custom menus + Toggle Button's / Checkboxes ETC
	void K2_text(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
	void K2_customtext(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 240)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 240)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
	void OutlinedRBGText(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 30 / 30.0)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 30 / 30.0)), utf_8_2.c_str());
		//ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
	void RegularText(int x, int y, RGBA* color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
	}
	void NeonRBGText(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y - 1), ImColor(64, 255, 0), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 1), ImColor(64, 255, 0), utf_8_2.c_str());
		//ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	}
	void BackgroundRBGOutline(int x, int y, int w, int h, ImColor color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0, thickness);
	}
	void BackgroundOutline(int x, int y, int w, int h, RGBA* color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0, thickness);
	}
	void TitleBar(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0);
	}
	void TitleBarGraident(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), (color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), (color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), (color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), (color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0), 0, 0);
	}
	void BackgroundLowOpacity(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 0.20f)), 0);
	}

	void BackgroundLowRounded(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 0.20f)), 6);
	}

	void BackgroundRBGGradient(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void K2_drawfilled(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void SelectedFilled(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 0.40f)), 0, 0);
	}

	void BackgroundFilled(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 1.00f)), 0);
	}

	void BackgroundFilledRound(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 1.00f)), 6);
	}

	void BackgroundGradient(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 1.00f)), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 1.00f)), 0, 0);
	}
	void RoundedRect(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImColor(color), 2);
	}
}

void CodeinButton(const char* str_id, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight() - 5;
	float width = height * 1.80f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.05f;
	if (g.ActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
	{
		float t_anim = ImSaturate(g.ActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImColor(0, 0, 0), t));
	else
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImColor(0, 0, 0), t));

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}


void DrawESP() {

	if (item.drawfov_circle) {
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(ScreenCenterX, ScreenCenterY), float(item.AimFOV), ImGui::GetColorU32({ item.DrawFOVCircle[0], item.DrawFOVCircle[1], item.DrawFOVCircle[2], 1.0f }), item.Shape, item.Thickness);
	}

	if (item.cross_hair) {
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 - 11, Height / 2), ImVec2(Width / 2 + 1, Height / 2), ImGui::ColorConvertFloat4ToU32(ImColor{ 255, 89, 167 }), 1.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 + 12, Height / 2), ImVec2(Width / 2 + 1, Height / 2), ImGui::ColorConvertFloat4ToU32(ImColor{ 255, 89, 167 }), 1.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 - 11), ImVec2(Width / 2, Height / 2), ImGui::ColorConvertFloat4ToU32(ImColor{ 255, 89, 167 }), 1.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 + 12), ImVec2(Width / 2, Height / 2), ImGui::ColorConvertFloat4ToU32(ImColor{ 255, 89, 167 }), 1.0f);
	}

	auto entityListCopy = entityList;
	float closestDistance = FLT_MAX;
	DWORD_PTR closestPawn = NULL;

	DWORD_PTR AActors = read<DWORD_PTR>(Ulevel + 0x98);


	for (unsigned long i = 0; i < entityListCopy.size(); ++i) {
		FNlEntity entity = entityListCopy[i];

		uint64_t CurrentActor = read<uint64_t>(AActors + i * 0x8);

		uint64_t CurActorRootComponent = read<uint64_t>(entity.Actor + 0x188);
		if (CurActorRootComponent == (uint64_t)nullptr || CurActorRootComponent == -1 || CurActorRootComponent == NULL)
			continue;

		Vector3 actorpos = read<Vector3>(CurActorRootComponent + 0x128);
		Vector3 actorposW2s = ProjectWorldToScreen(actorpos);

		DWORD64 otherPlayerState = read<uint64_t>(entity.Actor + 0x290);
		if (otherPlayerState == (uint64_t)nullptr || otherPlayerState == -1 || otherPlayerState == NULL)
			continue;

		localactorpos = read<Vector3>(Rootcomp + 0x128);

		Vector3 bone66 = GetBoneWithRotation(entity.mesh, 66);
		Vector3 bone0 = GetBoneWithRotation(entity.mesh, 0);

		Vector3 top = ProjectWorldToScreen(bone66);
		Vector3 chest = ProjectWorldToScreen(bone66);
		Vector3 aimbotspot = ProjectWorldToScreen(bone66);
		Vector3 bottom = ProjectWorldToScreen(bone0);

		uint64_t currentactormesh = read<uint64_t>(CurrentActor + 0x2F0);
		Vector3 rootOut = GetBoneWithRotation(currentactormesh, 0);
		Vector3 Out = ProjectWorldToScreen(rootOut);

		Vector3 Headposw2s;

		Vector3 Headpos = GetBoneWithRotation(currentactormesh, 66);
		Headposw2s = ProjectWorldToScreen(Headpos);
		Vector3 Head = ProjectWorldToScreen(Vector3(bone66.x, bone66.y, bone66.z + 15));

		float distance = localactorpos.Distance(bone66) / 100.f;
		float BoxHeight = (float)(Head.y - bottom.y);
		float CornerHeight = abs(Head.y - bottom.y);
		Vector3 vRootBone = GetBoneWithRotation(currentactormesh, 0);
		Vector3 vRootBoneOut = ProjectWorldToScreen(vRootBone);
		float CornerWidth = BoxHeight * 0.50;

		int MyTeamId = read<int>(PlayerState + 0x1010);
		int ActorTeamId = read<int>(otherPlayerState + 0x1010);
		int curactorid = read<int>(CurrentActor + 0x18);


		auto SkelColor = ImColor(255, 0, 0);
		auto Name = ImColor(250, 0, 0);



		if (MyTeamId != ActorTeamId) {
			if (item.Esp_line) {
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 1, Height / 1), ImVec2(chest.x, chest.y), ImGui::GetColorU32({ item.LineESP[0], item.LineESP[1], item.LineESP[2], 1.0f }), item.Thickness);

			}

			if (item.Esp_box_fill) {
				ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(Head.x - (CornerWidth / 4), Head.y), ImVec2(bottom.x + (CornerWidth / 4), bottom.y), ImGui::GetColorU32({ 0.00f, 0.00f, 0.00f, 0.50f }), 0, item.Thickness);
				ImGui::GetOverlayDrawList()->AddRect(ImVec2(Head.x - (CornerWidth / 4), Head.y), ImVec2(bottom.x + (CornerWidth / 4), bottom.y), ImGui::GetColorU32({ item.Espbox[0], item.Espbox[1], item.Espbox[2], 1.0f }), 0, item.Thickness);
				ImGui::GetOverlayDrawList()->AddRect(ImVec2(Head.x - (CornerWidth / 4), Head.y), ImVec2(bottom.x + (CornerWidth / 4), bottom.y), ImGui::GetColorU32({ 0.00f, 0.00f, 0.00f, 1.00f }), 0, item.Thickness);
			}


			if (item.Esp_box) {

				ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(Head.x - (CornerWidth / 4), Head.y), ImVec2(bottom.x + (CornerWidth / 4), bottom.y), ImGui::GetColorU32({ 0.00f, 0.00f, 0.00f, 0.50f }), 0, item.Thickness);
				ImGui::GetOverlayDrawList()->AddRect(ImVec2(Head.x - (CornerWidth / 4), Head.y), ImVec2(bottom.x + (CornerWidth / 4), bottom.y), ImGui::GetColorU32({ item.Espbox[0], item.Espbox[1], item.Espbox[2], 1.0f }), 0, item.Thickness);
				ImGui::GetOverlayDrawList()->AddRect(ImVec2(Head.x - (CornerWidth / 4), Head.y), ImVec2(bottom.x + (CornerWidth / 4), bottom.y), ImGui::GetColorU32({ 0.00f, 0.00f, 0.00f, 1.00f }), 0, item.Thickness);
			}

			if (item.skeleton) {
				DrawSkeleton(entity.mesh, SkelColor);
			}

			if (item.WEAPON)
			{
				auto ftextsize = read<const char*>(otherPlayerState + 0x388 + 0x30 + 0x8);
				ImVec2 DistanceTextSize = ImGui::CalcTextSize(ftextsize);
				ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), 16.f, ImVec2(Head.x - DistanceTextSize.x / 2, Head.y + 35 + DistanceTextSize.y * 3), ImGui::ColorConvertFloat4ToU32(Name), ftextsize);
			}

			if (item.tpos) {
				uintptr_t ES = read<uintptr_t>(CurrentActor + 0x2F0);
				if (!ES) continue;

				uintptr_t mesh = read<uintptr_t>(LocalPawn + 0x2F0);
				write<int>(mesh + 0x9aa, 1);
				write<int>(ES + 0x9aa, 1);
			}

			if (item.FovChanger)
			{
				uint64_t CamManager = read<uint64_t>(PlayerController + 0x338);
				write<float>(CamManager + 0x294, item.FovValue); // + 0x4
			}


			if (item.FPAimbot) {

				item.Aimbot = true;
				if (GetAsyncKeyState(VK_RBUTTON)) {
					if (LocalPawn) {
						uintptr_t Mesh = read<uintptr_t>(LocalPawn + 0x2F0);
						write<Vector3>(Mesh + 0x158, Vector3(2000, -2000, 2000)); //Class Engine.SceneComponent -> RelativeScale3D -> 0x134
					}
				}
				else {
					uintptr_t Mesh = read<uintptr_t>(LocalPawn + 0x2F0);
					write<Vector3>(Mesh + 0x158, Vector3(0, 0, -87)); //Class Engine.SceneComponent -> RelativeScale3D -> 0x134
				}

			}

			if (item.RapidFire)
			{
				float a = 0;
				float b = 0;
				uintptr_t CurrentWeapon = *(uintptr_t*)(LocalPawn + 0x790); //CurrentWeapon Offset
				if (CurrentWeapon) {
					a = *(float*)(CurrentWeapon + 0xA60); //LastFireTime Offset
					b = *(float*)(CurrentWeapon + 0xA64); //LastFireTimeVerified Offset
					*(float*)(CurrentWeapon + 0xA70) = a + b - 0.0003; //LastFireTime Offset
				}
			}

			if (item.Animation)
			{
				uintptr_t CurrentWeapon = *(uintptr_t*)(LocalPawn + 0x790); //CurrentWeapon Offset
				if (CurrentWeapon) {
					*(bool*)(CurrentWeapon + 0x313) = true; //bDisableEquipAnimation Offset (update this yourself)
				}
			}

			if (item.playergay)
			{
				write<bool>(LocalPawn + 0x4525, true);
			}

			if (item.instant_res) {
				write<float>(LocalPawn + 0x3EE8, 0.2f); //ReviveFromDBNOTime Offset
			}

			if (item.Distance_Esp) {
				char dist[64];
				sprintf_s(dist, "[%.fM]", distance);
				ImGui::GetOverlayDrawList()->AddText(ImVec2(bottom.x - 15, bottom.y), ImGui::GetColorU32({ 0.28f, 0.56f, 1.00f, 1.00f }), dist);
			}



			if (item.freezeplayer) {
				write<float>(LocalPawn + 0xC8, 10.f);
				write<float>(LocalPawn + 0xCC, 0.1f);
			}

			if (item.airstuck) {
				if (GetAsyncKeyState(VK_F4)) { //Alt Keybind
					write<float>(LocalPawn + 0x64, 0); //CustomTimeDilation Offset
				}
				else {
					write<float>(LocalPawn + 0x64, 1); //CustomTimeDilation Offset
				}
			}

			if (item.glider) {
				if (item.glider && GetAsyncKeyState(VK_SHIFT))
				{
					write<float>(LocalPawn + 0x1946, 0.02f); //bIsParachuteOpen Offset
					write<float>(LocalPawn + 0x1945, 0.02f); //bIsSkydiving  Offset
				}
			}


			if (item.Aimbot) {
				auto dx = aimbotspot.x - (Width / 2);
				auto dy = aimbotspot.y - (Height / 2);
				auto dz = aimbotspot.z - (Depth / 2);
				auto dist = sqrtf(dx * dx + dy * dy + dz * dz) / 50.0f;
				if (dist < item.AimFOV && dist < closestDistance) {
					closestDistance = dist;
					closestPawn = entity.Actor;
				}

			}
		}
	}

	if (item.Aimbot) {
		if (closestPawn != 0) {
			if (item.Aimbot && closestPawn && GetAsyncKeyState(item.aimkey) < 0) {
				AimAt(closestPawn);

			}
			else {
				isaimbotting = false;
				AimAt2(closestPawn);
			}
		}
	}
}

void render() {

	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = nullptr;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	char dist[200];
	sprintf_s(dist, " BigC [V4.0.0]");
	ImGui::GetOverlayDrawList()->AddText(ImVec2(Width / 2.20, Height / 16), ImGui::GetColorU32({ color.DarkRed[0], color.DarkRed[1], color.DarkRed[2], 4.0f }), dist);

	DrawESP();
	shortcurts();

	if (menu_key)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImGui::GetIO().MousePos, ImVec2(ImGui::GetIO().MousePos.x + 5.f, ImGui::GetIO().MousePos.y + 5.f), ImColor(235, 0, 0, 255));
		static int kugay = 0;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(1.00f, 0.00f, 0.00f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.41f, 0.00f, 0.03f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.48f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);

		ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5, 0.5);
		ImGui::GetStyle().FramePadding = ImVec2(4.0, 2.0);
		ImGui::GetStyle().WindowRounding = 0;
		ImGui::GetStyle().GrabMinSize = 12;
		ImGui::GetStyle().ScrollbarSize = 14;
		ImGui::GetStyle().ScrollbarRounding = 0;
		ImGui::GetStyle().FrameRounding = 4.0f;



		ImGui::SetNextWindowSize({ 370.f,605.f });


		ImGui::PushFont(gay);

		if (ImGui::Begin(" BigC [F5]", 0, ImGuiWindowFlags_NoResize))
		{


			if (ImGui::CollapsingHeader("Aimbot"))
			{
				ImGui::Text("Aimbot");
				ImGui::Text("");
				ImGui::Checkbox("Memory", &item.Aimbot);
				//ImGui::Checkbox("Silent", &item.PerfectSilent);
				ImGui::Checkbox("Aim Fov Circle", &item.drawfov_circle);
				ImGui::Checkbox("Crosshair", &item.cross_hair);
				std::string FOVAMT = "Field Of View: " + std::to_string(item.AimFOV);
				std::string SMOOTHAMT = "Aim Smoothing: " + std::to_string(item.Smooth);

				ImGui::Text("");
				ImGui::Text(FOVAMT.c_str());
				ImGui::SliderFloaAAAAAAAAAt("fov", &item.AimFOV, 0, 1000, "%.3f", 2);
				ImGui::Text(SMOOTHAMT.c_str());
				ImGui::SliderFloaAAAAAAAAAt("smoothing", &item.Smooth, 0, 25, "%.3f", 2);
			}
			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Visuals"))
			{
				ImGui::Text("Visuals");
				ImGui::Text("");
				ImGui::Checkbox("Skeleton", &item.skeleton);
				ImGui::Checkbox("Box", &item.Esp_box);
				ImGui::Checkbox("Distance", &item.Distance_Esp);
			}
			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Exploits"))
			{
				ImGui::Text("Exploits [Detection Risk]");
				ImGui::Text("");
				ImGui::Checkbox("Invsibility", &item.FPAimbot);
				ImGui::Checkbox("PlayerFly [Repairing...]", &item.playerfly);
				ImGui::Checkbox("Aim While Jumping", &item.playergay);
				ImGui::Checkbox("Instant Revive", &item.instant_res);
				ImGui::Checkbox("RapidFire", &item.RapidFire);
			}

			ImGui::End();
		}
		ImGui::PopFont();

	}

	float radarWidth = 230;
	float PosDx = item.RadarPosX;
	float PosDy = item.RadarPosY;

	if (item.shield_esp)
	{
		ImVec2 midRadar = ImVec2(PosDx + (radarWidth / 2), PosDy + (radarWidth / 2));
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(PosDx, PosDy), ImVec2(PosDx + radarWidth, PosDy + radarWidth), IM_COL32(25, 25, 25, 255));
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(PosDx, PosDy), ImVec2(PosDx + radarWidth, PosDy + radarWidth), IM_COL32(0, 0, 0, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(PosDx + (radarWidth / 2), PosDy + (radarWidth / 2)), ImVec2(PosDx, PosDy), IM_COL32(255, 255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(PosDx + (radarWidth / 2), PosDy + (radarWidth / 2)), ImVec2(PosDx + radarWidth, PosDy), IM_COL32(255, 255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(midRadar.x - (radarWidth / 2), midRadar.y), ImVec2(midRadar.x + (radarWidth / 2), midRadar.y), IM_COL32(255, 255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(midRadar.x, midRadar.y - (radarWidth / 2)), ImVec2(midRadar.x, midRadar.y + (radarWidth / 2)), IM_COL32(255, 255, 255, 255));
	}

	ImGui::EndFrame();
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (p_Device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}
	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&p_Params);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}


WPARAM MainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();
		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{

			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyWnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();



	CleanuoD3D();
	DestroyWindow(MyWnd);

	return Message.wParam;
}
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		CleanuoD3D();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void CleanuoD3D()
{
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}
	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}
void SetWindowToTarget()
{
	while (true)
	{
		GameWnd = get_process_wnd(sdk::process_id);
		if (GameWnd)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(GameWnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(GameWnd, GWL_STYLE);
			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			ScreenCenterX = Width / 2;
			ScreenCenterY = Height / 2;
			MoveWindow(MyWnd, GameRect.left, GameRect.top, Width, Height, true);
		}
	}
}

bool IsProcessRunning(const wchar_t* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!wcsicmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}

int main()
{
	SetConsoleTitleA("Windows IP Over USB PC Service");

	system("cls");
	system("color a");
//	system("start http://webmat.store");
	printf("Press Enter when you are in the match.");
//	system("pause >nul 2>&1");
	system("cls");
	printf("\n\n	Welcome to BigC");
	printf("\n	Made by anticheese#5680 ~ Love you Guys<3");
	printf("\n	Injected.");

	PIDManager _PIDManager;

	sdk::process_id = _PIDManager.GetAowProcId();

	Controller = new DriverController(sdk::process_id);

	sdk::module_base = GetBaseAddress();

	// printf(("\n	Fortnite Base Address: 0x%llX\n"), sdk::module_base);
	// printf(("	Fortnite Process Id: 0x%llX\n"), sdk::process_id);

	SetupWindow();
	DirectXInit(MyWnd);

	std::thread(drAWLOOOPP).detach();

	while (1)
	{
		MainLoop();
		Sleep(5);
	}

	return 0;
	printf(("Failed!\n"));
	system(("pause"));
	return 1;
}