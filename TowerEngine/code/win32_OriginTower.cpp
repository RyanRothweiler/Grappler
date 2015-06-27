#include <windows.h>
#include <xinput.h>

#include "win32_OriginTower.h"

#define Assert(Expression) if (!(Expression)) {*(int *)0 = 0;}

typedef _int8 int8;
typedef _int16 int16;
typedef _int32 int32;
typedef _int64 int64;

bool GlobalRunning = true;


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
DebugLine(char *output)
{
	char finalOutput[MAX_PATH];
	ConcatStrings(output, "\n", finalOutput);
	OutputDebugString(finalOutput);
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
		HWND WindowHandle = 
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

					HDC DeviceContext = GetDC(WindowHandle);

					RECT WindowRect;
					GetWindowRect(WindowHandle, &WindowRect);
					DWORD WindowWidth = WindowRect.right;
					DWORD WindowHeight = WindowRect.bottom;

					void *PixelData;
					BITMAPINFO *BitMapInfo;
					StretchDIBits(DeviceContext,
					              0, 0, WindowWidth, WindowHeight,
					              0, 0, WindowWidth, WindowHeight,
					              PixelData, BitMapInfo,
					              DIB_RGB_COLORS, 
					              WHITENESS);

					ReleaseDC(WindowHandle, DeviceContext);
				}
			}
		}
	}
}