#include <windows.h>

#include "win32_OriginTower.h"

bool GlobalRunning = true;

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result;

	switch (uMsg)
	{
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

	Result = DefWindowProc(hwnd, uMsg, wParam, lParam);
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
		HWND Window = 
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
		if (Window)
		{
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
				}
			}
		}
	}
}