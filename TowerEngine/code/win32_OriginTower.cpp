#include <windows.h>
#include <xinput.h>

#include "win32_OriginTower.h"

#define Assert(Expression) if (!(Expression)) {*(int *)0 = 0;}

typedef _int8 int8;
typedef _int16 int16;
typedef _int32 int32;
typedef _int64 int64;

typedef unsigned _int8 uint8;
typedef unsigned _int16 uint16;
typedef unsigned _int32 uint32;
typedef unsigned _int64 uint64;

HWND WindowHandle;
bool GlobalRunning = true;
void *PixelData = {};
DWORD WindowWidth = {};
DWORD WindowHeight = {};

struct controller
{
	bool aButton;
	bool bButton;
	bool xButton;
	bool yButton;
};

int32
StringLength(char *String)
{
	int Count = 0;
	while (*String++)
	{
		++Count;
	}
	return (Count);
}

void
ConcatStrings(char *SourceA, char *SourceB, char *Destination)
{
	int32 SourceALength = StringLength(SourceA);
	int32 SourceBLength = StringLength(SourceB);	

	for (int32 Index = 0;
	     Index < SourceALength;
	     Index++)
	{
		*Destination++ = *SourceA++;
	}

	for (int32 Index = 0;
	     Index < SourceBLength;
	     Index++)
	{
		*Destination++ = *SourceB++;
	}

	*Destination++ = 0;
}

void 
UpdateScreenSize()
{
	RECT WindowRect;
	GetWindowRect(WindowHandle, &WindowRect);
	WindowWidth = WindowRect.right - WindowRect.left;
	WindowHeight = WindowRect.bottom - WindowRect.top;
}

void 
DebugLine(char *output)
{
	char finalOutput[MAX_PATH];
	ConcatStrings(output, "\n", finalOutput);
	OutputDebugString(finalOutput);
}

void
DrawPixels(HWND WindowHandle)
{
	HDC DeviceContext = GetDC(WindowHandle);

	BITMAPINFO BitMapInfo = {};
	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biWidth = WindowWidth;
	BitMapInfo.bmiHeader.biHeight = -1 * (int32)WindowHeight;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 32;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;

	StretchDIBits(DeviceContext,
	              0, 0, WindowWidth, WindowHeight,
	              0, 0, WindowWidth, WindowHeight,
	              PixelData, &BitMapInfo,
	              DIB_RGB_COLORS, 
	              SRCCOPY);

	ReleaseDC(WindowHandle, DeviceContext);
}

LRESULT CALLBACK 
WindowProcedure(HWND WindowHandle, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result;

	switch (uMsg)
	{
		case WM_PAINT:
		{
		} break;

		case WM_SIZE:
		{
			UpdateScreenSize();
			DrawPixels(WindowHandle);
		} break;

		case WM_CLOSE:
		{
			GlobalRunning = false;
		} break;

		default:
		{
		} break;
	}

	Result = DefWindowProc(WindowHandle, uMsg, wParam, lParam);
	return(Result);
}

int CALLBACK 
WinMain(HINSTANCE Instance,	HINSTANCE PrevInstance,	LPSTR CommandLine, int ShowCode)
{
	WNDCLASS WindowClass = {};
	WindowClass.style = CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProcedure;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "OriginTowerWindowClass";

	if(RegisterClass(&WindowClass))
	{
		WindowHandle = 
		CreateWindowEx(
		               0,
		               WindowClass.lpszClassName,
		               "Origin Tower",
		               WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		               CW_USEDEFAULT,
		               CW_USEDEFAULT,
		               CW_USEDEFAULT,
		               CW_USEDEFAULT,
		               0,
		               0,
		               Instance,
		               0);
		if (WindowHandle)
		{

			controller Controller;



			UpdateScreenSize();

			uint32 BytesPerPixel = 4;
			uint32 Pitch = WindowWidth * BytesPerPixel;

			uint32 MemSize = WindowWidth * WindowHeight * BytesPerPixel;

			PixelData =  VirtualAlloc(0,
			                          MemSize, 
			                          MEM_COMMIT, PAGE_READWRITE);

			if (PixelData == NULL)
			{
				Assert(0);
			}

			uint8 *Row = (uint8 *)PixelData;
			for (uint32 Y = 0;
			     Y < WindowHeight;
			     ++Y)
			{
				uint8 *Pixel = (uint8 *)Row;
				for (uint32 X = 0;
				     X < WindowWidth;
				     ++X)
				{
					// B
					*Pixel = 100;
					++Pixel;

					// G
					*Pixel = 100;
					++Pixel;

					// R
					*Pixel = 255;
					++Pixel;

					// A?
					*Pixel = 2;
					++Pixel;

				}
				Row += Pitch;
			}



			while (GlobalRunning)
			{
				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					switch(Message.message)
					{
						default:
						{
							TranslateMessage(&Message);
							DispatchMessage(&Message);
						} break;
					}

					for (DWORD i = 0; 
					     i < XUSER_MAX_COUNT; 
					     i++)
					{
						XINPUT_STATE state;
						ZeroMemory(&state, sizeof(XINPUT_STATE));
						DWORD dwResult = XInputGetState(i, &state);

						if(dwResult == ERROR_SUCCESS)
						{
							WORD wButtons = state.Gamepad.wButtons;

							Controller.aButton = (wButtons & XINPUT_GAMEPAD_A) != 0;
							Controller.bButton = (wButtons & XINPUT_GAMEPAD_B) != 0;
							Controller.xButton = (wButtons & XINPUT_GAMEPAD_X) != 0;
							Controller.yButton = (wButtons & XINPUT_GAMEPAD_Y) != 0;

							WORD bButton = wButtons & 0x2000;
						}
					}

					DrawPixels(WindowHandle);
				}
			}
		}
	}
}