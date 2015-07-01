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

typedef int32 bool32;


struct color
{
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 A;
};

struct screen_buffer
{
	DWORD Width;
	DWORD Height;

	uint32 BytesPerPixel = 4;
	void *ScreenBuffer;
	
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

	float LeftStickX;
	float LeftStickY;

};

struct player
{
	float PosX;
	float PosY;
	uint16 Width;
	color Color;
};


bool GlobalRunning = true;
screen_buffer ScreenBuffer;


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

// This is super wonky. Don't really need to put the prev number into a char array. Just hold the last loops number
int32
DigitCount(int64 *Input)
{
	char NumChar[MAX_PATH] = {};
	char *NumCharPointer = NumChar;

	int32 Count = 0;
	int64 tmp = *Input;
	while (tmp != 0)
	{
		tmp = (tmp - *NumCharPointer)/10;
		Count++;
	}

	return (Count);
}

void
IntToCharArray(int64 *Input, char *Output)
{
	int64 tmp = *Input;
	char *NumCharPointer = Output + DigitCount(&tmp) - 1;

	while (tmp != 0)
	{
		*NumCharPointer-- = '0' + (tmp % 10);
		tmp = (tmp - *NumCharPointer)/10;
	}
}


void
ConcatCharArrays(char *SourceA, char *SourceB, char *Destination)
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
DebugLine(int64 *Output)
{
	char NumChar[MAX_PATH] = {};
	IntToCharArray(Output, NumChar);

	char FinalOutput[MAX_PATH] = {};
	ConcatCharArrays(NumChar, "\n", FinalOutput);
	OutputDebugString(FinalOutput);
}

void 
DebugLine(int64 Output)
{
	DebugLine(&Output);
}

void 
DebugLine(char *Output)
{
	char FinalOutput[MAX_PATH] = {};
	ConcatCharArrays(Output, "\n", FinalOutput);
	OutputDebugString(FinalOutput);
}

void
ConcatIntChar(int64 IntInput, char *CharInput, 
              char *CharOutput)
{
	char IntInputAsChar[MAX_PATH] = {};
	IntToCharArray(&IntInput, IntInputAsChar);
	ConcatCharArrays(IntInputAsChar, CharInput, CharOutput);
}

void
ConcatIntChar(char *CharInput, int64 IntInput, 
              char *CharOutput)
{
	char IntInputAsChar[MAX_PATH] = {};
	IntToCharArray(&IntInput, IntInputAsChar);
	ConcatCharArrays(IntInputAsChar, CharInput, CharOutput);
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
UpdateScreenSize(HWND WindowHandle)
{
	RECT WindowRect;
	GetWindowRect(WindowHandle, &WindowRect);
	ScreenBuffer.Width = WindowRect.right - WindowRect.left;
	ScreenBuffer.Height = WindowRect.bottom - WindowRect.top;
}

void
DrawPixels(HWND WindowHandle)
{
	HDC DeviceContext = GetDC(WindowHandle);

	BITMAPINFO BitMapInfo = {};
	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biWidth = ScreenBuffer.Width;
	BitMapInfo.bmiHeader.biHeight = -1 * (int32)ScreenBuffer.Height;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 32;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;

	StretchDIBits(DeviceContext,
	              0, 0, ScreenBuffer.Width, ScreenBuffer.Height,
	              0, 0, ScreenBuffer.Width, ScreenBuffer.Height,
	              ScreenBuffer.ScreenBuffer, &BitMapInfo,
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
			UpdateScreenSize(WindowHandle);
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
DrawSquare(int32 XPos, int32 YPos, uint32 squareSize, color Color)
{
	int32 MinX = XPos;
	int32 MinY = YPos;
	int32 MaxX = XPos + squareSize;
	int32 MaxY = YPos + squareSize;

	if (MinX < 0)
	{
		MinX = 0;
	}
	if (MinY < 0)
	{
		MinY = 0;
	}
	if (MaxX > (int32)ScreenBuffer.Width)
	{
		MaxX = (int32)ScreenBuffer.Width;
	}
	if (MaxY > (int32)ScreenBuffer.Height)
	{
		MaxY = (int32)ScreenBuffer.Height;
	}

	uint32 Pitch = ScreenBuffer.Width * ScreenBuffer.BytesPerPixel;
	uint8 *Row = (uint8 *)ScreenBuffer.ScreenBuffer + ((MinX * ScreenBuffer.BytesPerPixel) + (MinY * Pitch));
	for (int32 Y = MinY;
	     Y < MaxY;
	     ++Y)
	{
		uint8 *Pixel = (uint8 *)Row;
		for (int32 X = MinX;
		     X < MaxX;
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

			controller Controller = {};

			ScreenBuffer = {};
			ScreenBuffer.BackgroundColor.R = 0;
			ScreenBuffer.BackgroundColor.G = 0;
			ScreenBuffer.BackgroundColor.B = 0;
			UpdateScreenSize(WindowHandle);

			uint32 MemSize = ScreenBuffer.Width * ScreenBuffer.Height * ScreenBuffer.BytesPerPixel;
			ScreenBuffer.ScreenBuffer =  VirtualAlloc(0, MemSize, MEM_COMMIT, PAGE_READWRITE);
			uint8 *Row = (uint8 *)ScreenBuffer.ScreenBuffer;
			for (uint32 Y = 0;
			     Y < ScreenBuffer.Height;
			     ++Y)
			{
				uint8 *Pixel = (uint8 *)Row;
				for (uint32 X = 0;
				     X < ScreenBuffer.Width;
				     ++X)
				{	
					// B
					*Pixel = ScreenBuffer.BackgroundColor.B;
					++Pixel;

					// G
					*Pixel = ScreenBuffer.BackgroundColor.G;
					++Pixel;

					// R
					*Pixel = ScreenBuffer.BackgroundColor.R;
					++Pixel;

					// A?
					*Pixel = ScreenBuffer.BackgroundColor.A;
					++Pixel;

				}
				uint32 Pitch = ScreenBuffer.Width * ScreenBuffer.BytesPerPixel;
				Row += Pitch;
			}

			if (ScreenBuffer.ScreenBuffer == NULL)
			{
				DebugLine("Pixel Data no initialized correctly.");
				Assert(0);
			}


			player Player = {};
			Player.PosX = (float)(ScreenBuffer.Width / 2);
			Player.PosY = (float)(ScreenBuffer.Height / 2);
			Player.Width = 50;
			Player.Color.R = 255;
			Player.Color.G = 100;
			Player.Color.B = 100;


			LARGE_INTEGER PreviousFrameCount;
			QueryPerformanceCounter(&PreviousFrameCount);

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

						Controller.LeftStickX = CheckStickDeadzone(Pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
						Controller.LeftStickY = CheckStickDeadzone(Pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

					}
					else
					{
            				// Controller is not connected 
					}
				}


				DrawSquare((uint32)Player.PosX, (uint32)Player.PosY, Player.Width, ScreenBuffer.BackgroundColor);
				Player.PosX += Controller.LeftStickX;
				Player.PosY -= Controller.LeftStickY;
				DrawSquare((uint32)Player.PosX, (uint32)Player.PosY, Player.Width, Player.Color);

				DrawPixels(WindowHandle);

				LARGE_INTEGER NewFrameCount;
				QueryPerformanceCounter(&NewFrameCount);
				int64 ElapsedFrameCount = NewFrameCount.QuadPart - PreviousFrameCount.QuadPart;
				PreviousFrameCount = NewFrameCount;
			}
		}
	}
}