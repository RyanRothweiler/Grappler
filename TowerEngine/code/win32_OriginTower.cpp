
#include "OriginTower.h"

bool GlobalRunning = true;
screen_buffer ScreenBuffer;
int64 ElapsedFrameCount;
int64 PerfCountFrequency;

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

int32
DigitCount(int64 *Input)
{
	char NumChar[MAX_PATH] = {};
	char *NumCharPointer = NumChar;

	int32 Count = 0;
	int64 tmp = *Input;
	while (tmp != 0)
	{
		tmp = (tmp - *NumCharPointer) / 10;
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
		tmp = (tmp - *NumCharPointer) / 10;
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

real64
CheckStickDeadzone(short Value, SHORT DeadZoneThreshold)
{
	real64 Result = 0;

	if (Value < -DeadZoneThreshold)
	{
		Result = (real64)(Value + DeadZoneThreshold) / (32768.0f - DeadZoneThreshold);
	}
	else if (Value > DeadZoneThreshold)
	{
		// this explicit number is pulled from my ass.
		Result = (real64)(Value  + DeadZoneThreshold) / (47467.0f - DeadZoneThreshold);
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

struct win32_game_code
{
	HMODULE GameCodeDLL;
	game_update_and_render *UpdateAndRender;

	bool32 IsValid;
};

win32_game_code
LoadGameCode()
{
	win32_game_code Result = {};

	CopyFile("OriginTower.dll", "OriginTower_temp.dll", FALSE);
	Result.GameCodeDLL = LoadLibraryA("OriginTower_temp.dll");
	if (Result.GameCodeDLL)
	{
		Result.UpdateAndRender = (game_update_and_render *)GetProcAddress(Result.GameCodeDLL, "GameUpdateAndRender");
	}

	// NOTE this is the wrong way to set is valid. we don't actually know it is valid.
	Result.IsValid = true;

	return (Result);
}

void
UnloadGameCode(win32_game_code *GameCode)
{
	if (GameCode->GameCodeDLL)
	{
		FreeLibrary(GameCode->GameCodeDLL);
	}

	GameCode->IsValid = false;
	GameCode->UpdateAndRender = GameUpdateAndRenderStub;
}


LRESULT CALLBACK
WindowProcedure(HWND WindowHandle, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// test
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
	return (Result);
}

inline LARGE_INTEGER
GetWallClock()
{
	LARGE_INTEGER Count;
	QueryPerformanceCounter(&Count);
	return (Count);
}

int CALLBACK
WinMain(HINSTANCE Instance,	HINSTANCE PrevInstance,	LPSTR CommandLine, int ShowCode)
{
	WNDCLASS WindowClass = {};
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProcedure;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "OriginTowerWindowClass";

	if (RegisterClass(&WindowClass))
	{
		HWND WindowHandle =
		    CreateWindowEx(
		        0,
		        WindowClass.lpszClassName,
		        "Origin Tower",
		        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
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


			LARGE_INTEGER FrequencyLong;
			QueryPerformanceFrequency(&FrequencyLong);
			PerfCountFrequency = FrequencyLong.QuadPart;

			// Probably need to get this from hardware instead of pulling a number out of my ass
			int32 MonitorUpdateHz = 60;
			int32 GameUpdateHz = 60;
			real64 TargetSecondsElapsedPerFrame = 1.0f / (real64)GameUpdateHz;

			game_input GameInput = {};

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
			Assert(ScreenBuffer.ScreenBuffer !=  NULL);


			#if INTERNAL
			LPVOID BaseAddress = (LPVOID)Terrabytes((uint64)2);
			#else
			LPVOID BaseAddress = 0;
			#endif
			game_memory GameMemory = {};
			GameMemory.PermanentStorageSize = Megabytes(64);
			GameMemory.TransientStorageSize = Gigabytes((uint64)4);
			uint64 TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;

			GameMemory.PermanentStorage = VirtualAlloc(BaseAddress, TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			GameMemory.TransientStorage = (uint8 *)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize;

			LARGE_INTEGER PreviousFrameCount = GetWallClock();


			win32_game_code GameCode = LoadGameCode();
			uint32 LoadCounter = 0;

			while (GlobalRunning)
			{
				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					switch (Message.message)
					{
						default:
						{
							TranslateMessage(&Message);
							DispatchMessage(&Message);
						} break;
					}
				}

				LoadCounter++;
				if (LoadCounter > 60)
				{
					UnloadGameCode(&GameCode);
					GameCode = LoadGameCode();
					LoadCounter = 0;
				}

				DWORD dwResult;
				for (DWORD ControllerIndex = 0;
				     ControllerIndex < XUSER_MAX_COUNT;
				     ControllerIndex++)
				{
					XINPUT_STATE ControllerState;
					ZeroMemory(&ControllerState, sizeof(XINPUT_STATE));

					dwResult = XInputGetState(ControllerIndex, &ControllerState);

					if (dwResult == ERROR_SUCCESS)
					{
						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

						GameInput.AButtonDown = (Pad->wButtons & XINPUT_GAMEPAD_A);
						GameInput.BButtonDown = (Pad->wButtons & XINPUT_GAMEPAD_B);
						GameInput.XButtonDown = (Pad->wButtons & XINPUT_GAMEPAD_X);
						GameInput.YButtonDown = (Pad->wButtons & XINPUT_GAMEPAD_Y);

						GameInput.DUp = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						GameInput.DDown = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						GameInput.DLeft = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						GameInput.DRight = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

						GameInput.LeftStick.X = ClampValue(-0.9f, 0.9f, CheckStickDeadzone(Pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
						GameInput.LeftStick.Y = ClampValue(-0.9f, 0.9f, CheckStickDeadzone(Pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) * -1;

					}
					else
					{
						// Controller is not connected
					}
				}

				GameCode.UpdateAndRender(&GameMemory, &GameInput, &ScreenBuffer);

				DrawPixels(WindowHandle);

				LARGE_INTEGER WorkFrameCount = GetWallClock();
				ElapsedFrameCount = WorkFrameCount.QuadPart - PreviousFrameCount.QuadPart;

				real64 SecondsElapsedForWork = (real64)ElapsedFrameCount / (real64)PerfCountFrequency;
				real64 SecondsElapsedForFrame = SecondsElapsedForWork;
				while (SecondsElapsedForFrame < TargetSecondsElapsedPerFrame)
				{
					LARGE_INTEGER NewWorkFrameCount = GetWallClock();
					SecondsElapsedForFrame = (((real64)NewWorkFrameCount.QuadPart - PreviousFrameCount.QuadPart) /
					                          (real64)PerfCountFrequency);
				}

				WorkFrameCount = GetWallClock();

				ElapsedFrameCount = WorkFrameCount.QuadPart - PreviousFrameCount.QuadPart;
				int64 MSThisFrame = (1000 * ElapsedFrameCount) / PerfCountFrequency;
				int64 FPS = PerfCountFrequency / ElapsedFrameCount;
				char charFPS[MAX_PATH] = {};
				ConcatIntChar(FPS, " FPS", charFPS);
				DebugLine(charFPS);

				PreviousFrameCount = WorkFrameCount;
			}
		}
	}
}