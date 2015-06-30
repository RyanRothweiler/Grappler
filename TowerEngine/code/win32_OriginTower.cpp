#include <windows.h>
#include <xinput.h>

#include "win32_OriginTower.h"

#define Assert(Expression) if (!(Expression)) {*(int *)0 = 0;}

// #define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
// #define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
// #define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30

typedef _int8 int8;
typedef _int16 int16;
typedef _int32 int32;
typedef _int64 int64;

typedef unsigned _int8 uint8;
typedef unsigned _int16 uint16;
typedef unsigned _int32 uint32;
typedef unsigned _int64 uint64;

typedef int32 bool32;


struct color
{
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 A;
};

struct window_screen
{
	DWORD Width;
	DWORD Height;

	uint32 BytesPerPixel = 4;
	void *PixelData;
	
	color BackgroundColor;
};

struct controller
{
	bool32 AButtonDown;
	bool32 BButtonDown;
	bool32 XButtonDown;
	bool32 YButtonDown;

	bool32 DUp;
	bool32 DRight;
	bool32 DLeft;
	bool32 DDown;
};

struct Vector2
{

};


HWND WindowHandle;
bool GlobalRunning = true;
window_screen WindowScreen;

float leftX;
float leftY;

float test;

float PlayerX;
float PlayerY;
int32 PlayerWidth;

float 
SquareRoot(float num)
{
	float i=0;
	float x1,x2;

	while( (i*i) <= num )
	{
		i+=0.1f;
	}
	x1=i;
	for(int j=0;j<10;j++)
	{
		x2=num;
		x2/=x1;
		x2+=x1;
		x2/=2;
		x1=x2;
	}
	return x2;
}

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

float
CheckStickDeadzone(short Value, SHORT DeadZoneThreshold)
{
	float Result = 0;

	if (Value < -DeadZoneThreshold)
	{
		Result = (float)(Value + DeadZoneThreshold) / (32768.0f - DeadZoneThreshold);
	}
	else if (Value > DeadZoneThreshold)
	{
		Result = (float)(Value  + DeadZoneThreshold) / (32767.0f - DeadZoneThreshold);
	}

	return (Result);
}

void 
UpdateScreenSize()
{
	RECT WindowRect;
	GetWindowRect(WindowHandle, &WindowRect);
	WindowScreen.Width = WindowRect.right - WindowRect.left;
	WindowScreen.Height = WindowRect.bottom - WindowRect.top;
}

void
DrawPixels(HWND WindowHandle)
{
	HDC DeviceContext = GetDC(WindowHandle);

	BITMAPINFO BitMapInfo = {};
	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biWidth = WindowScreen.Width;
	BitMapInfo.bmiHeader.biHeight = -1 * (int32)WindowScreen.Height;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 32;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;

	StretchDIBits(DeviceContext,
	              0, 0, WindowScreen.Width, WindowScreen.Height,
	              0, 0, WindowScreen.Width, WindowScreen.Height,
	              WindowScreen.PixelData, &BitMapInfo,
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

void
FillPixels(color Color)
{
	uint8 *Row = (uint8 *)WindowScreen.PixelData;
	for (uint32 Y = 0;
	     Y < WindowScreen.Height;
	     ++Y)
	{
		uint8 *Pixel = (uint8 *)Row;
		for (uint32 X = 0;
		     X < WindowScreen.Width;
		     ++X)
		{
					// B
			*Pixel = Color.B;
			++Pixel;

					// G
			*Pixel = Color.G;
			++Pixel;

					// R
			*Pixel = Color.R;
			++Pixel;

					// A?
			*Pixel = Color.A;
			++Pixel;

		}
		uint32 Pitch = WindowScreen.Width * WindowScreen.BytesPerPixel;
		Row += Pitch;
	}
}

void 
DrawSquare(uint32 XPos, uint32 YPos, uint32 Width, color color)
{
	uint32 Pitch = WindowScreen.Width * WindowScreen.BytesPerPixel;
	uint8 *Row = (uint8 *)WindowScreen.PixelData + ((XPos * WindowScreen.BytesPerPixel) + (YPos * Pitch));
	for (uint32 Y = 0;
	     Y < Width;
	     ++Y)
	{
		uint8 *Pixel = (uint8 *)Row;
		for (uint32 X = 0;
		     X < Width;
		     ++X)
		{
					// B
			*Pixel = color.B;
			++Pixel;

					// G
			*Pixel = color.G;
			++Pixel;

					// R
			*Pixel = color.R;
			++Pixel;

					// A?
			*Pixel = color.A;
			++Pixel;

		}
		Row += Pitch;
	}
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

			controller Controller = {};

			WindowScreen = {};
			WindowScreen.BackgroundColor.R = 0;
			WindowScreen.BackgroundColor.G = 0;
			WindowScreen.BackgroundColor.B = 0;
			UpdateScreenSize();

			uint32 MemSize = WindowScreen.Width * WindowScreen.Height * WindowScreen.BytesPerPixel;
			WindowScreen.PixelData =  VirtualAlloc(0, MemSize, MEM_COMMIT, PAGE_READWRITE);
			FillPixels(WindowScreen.BackgroundColor);

			if (WindowScreen.PixelData == NULL)
			{
				DebugLine("Pixel Data no initialized correctly.");
				Assert(0);
			}

			PlayerX = 10;
			PlayerY = 10;
			PlayerWidth = 10;

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

				DWORD dwResult;    
				for (DWORD ControllerIndex = 0; 
				     ControllerIndex < XUSER_MAX_COUNT; 
				     ControllerIndex++)
				{
					XINPUT_STATE ControllerState;
					ZeroMemory(&ControllerState, sizeof(XINPUT_STATE));

					dwResult = XInputGetState(ControllerIndex, &ControllerState);

					if(dwResult == ERROR_SUCCESS)
					{
						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

						Controller.AButtonDown = (Pad->wButtons & XINPUT_GAMEPAD_A);
						Controller.BButtonDown = (Pad->wButtons & XINPUT_GAMEPAD_B);
						Controller.XButtonDown = (Pad->wButtons & XINPUT_GAMEPAD_X);
						Controller.YButtonDown = (Pad->wButtons & XINPUT_GAMEPAD_Y);

						Controller.DUp = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						Controller.DDown = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						Controller.DLeft = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						Controller.DRight = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

						leftX = CheckStickDeadzone(Pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
						leftY = CheckStickDeadzone(Pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

					}
					else
					{
            				// Controller is not connected 
					}
				}


				if (Controller.DUp)
				{
					DebugLine("test");
					// Assert(0);
				}

				// float magnitude = SquareRoot(leftX * leftX + leftY * leftY);
				// float normalizedLX = leftX / magnitude;
				// float normalizedLY = leftY / magnitude;

				// float normalizedMagnitude = 0;

				// if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				// {
				// 	if (magnitude > 32767)
				// 	{ 
				// 		magnitude = 32767;
				// 	}
				// 	magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
				// 	normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				// }
				// else
				// {
				// 	magnitude = 0.0;
				// 	normalizedMagnitude = 0.0;
				// }

				PlayerX += leftX;
				PlayerY -= leftY;

				// if (Controller.DDown)
				// {
				// 	PlayerY++;
				// }
				// if (Controller.DRight)
				// {
				// 	PlayerX++;
				// }
				// if (Controller.DLeft)
				// {
				// 	PlayerX--;
				// }

				color PlayerColor;
				PlayerColor.R = 255;
				PlayerColor.G = 100;
				PlayerColor.B = 100;
				DrawSquare((uint32)PlayerX - 5, (uint32)PlayerY - 5, PlayerWidth * 2, WindowScreen.BackgroundColor);
				DrawSquare((uint32)PlayerX, (uint32)PlayerY, PlayerWidth, PlayerColor);

				DrawPixels(WindowHandle);
			}
		}
	}
}