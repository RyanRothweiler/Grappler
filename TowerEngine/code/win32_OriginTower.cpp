bool PRINTFPS = false;

#include "OriginTower.h"


bool GlobalRunning = true;
screen_buffer ScreenBuffer;
int64 ElapsedFrameCount;
int64 PerfCountFrequency;
LPDIRECTSOUNDBUFFER SoundSecondaryBuffer;

struct win32_game_code
{
	HMODULE GameCodeDLL;
	game_update_and_render *GameLoop;

	bool32 IsValid;
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
FillPixels(screen_buffer *ScreenBuffer)
{
	uint8 *Row = (uint8 *)ScreenBuffer->ScreenBuffer;
	for (uint32 Y = 0;
	     Y < ScreenBuffer->Height;
	     ++Y)
	{
		uint8 *Pixel = (uint8 *)Row;
		for (uint32 X = 0;
		     X < ScreenBuffer->Width;
		     ++X)
		{
			// B
			*Pixel = ScreenBuffer->BackgroundColor.B;
			++Pixel;

			// G
			*Pixel = ScreenBuffer->BackgroundColor.G;
			++Pixel;

			// R
			*Pixel = ScreenBuffer->BackgroundColor.R;
			++Pixel;

			// A?
			*Pixel = ScreenBuffer->BackgroundColor.A;
			++Pixel;

		}
		uint32 Pitch = ScreenBuffer->Width * ScreenBuffer->BytesPerPixel;
		Row += Pitch;
	}
}

void
ProcessButtonInput(input_button *ButtonProcessing, bool32 NewState)
{
	if (NewState)
	{
		if (ButtonProcessing->IsDown)
		{
			ButtonProcessing->OnDown = false;
		}
		else
		{
			ButtonProcessing->IsDown = true;
			ButtonProcessing->OnDown = true;
			ButtonProcessing->IsUp = false;
			ButtonProcessing->OnUp = false;
		}

	}
	else
	{
		if (ButtonProcessing->IsUp)
		{
			ButtonProcessing->OnUp = false;
		}
		else
		{
			ButtonProcessing->IsUp = true;
			ButtonProcessing->OnUp = true;
			ButtonProcessing->IsDown = false;
			ButtonProcessing->OnDown = false;
		}
	}
}

void
ProcessTriggerInput(input_button *Trigger, int32 TriggerValue)
{
	if (TriggerValue > 200)
	{
		ProcessButtonInput(Trigger, true);
	}
	else
	{
		ProcessButtonInput(Trigger, false);
	}
}


void
UpdateScreenSize(HWND WindowHandle)
{
	RECT WindowRect;
	GetWindowRect(WindowHandle, &WindowRect);
	ScreenBuffer.Width = WindowRect.right - WindowRect.left;
	ScreenBuffer.Height = WindowRect.bottom - WindowRect.top;

	uint32 MemSize = ScreenBuffer.Width * ScreenBuffer.Height * ScreenBuffer.BytesPerPixel;
	ScreenBuffer.ScreenBuffer =  VirtualAlloc(0, MemSize, MEM_COMMIT, PAGE_READWRITE);
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

win32_game_code
LoadGameCode()
{
	win32_game_code Result = {};

	CopyFile("OriginTower.dll", "OriginTower_temp.dll", FALSE);
	Result.GameCodeDLL = LoadLibraryA("OriginTower_temp.dll");
	if (Result.GameCodeDLL)
	{
		Result.GameLoop = (game_update_and_render *)GetProcAddress(Result.GameCodeDLL, "GameLoop");
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
	GameCode->GameLoop = GameLoopStub;
}

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

void
LoadDirectSound(HWND WindowHandle, win32_audio_output *SoundOutput)
{
	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
	if (DSoundLibrary)
	{
		direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

		LPDIRECTSOUND DirectSound;
		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
		{
			WAVEFORMATEX WaveFormat = {};
			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormat.nChannels = 2;
			WaveFormat.nSamplesPerSec = SoundOutput->SamplesPerSecond;
			WaveFormat.wBitsPerSample = 16;
			WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
			WaveFormat.cbSize = 0;

			if (SUCCEEDED(DirectSound->SetCooperativeLevel(WindowHandle, DSSCL_PRIORITY)))
			{
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
				{
					if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat)))
					{
						// NOTE now the format has been set
					}
					else
					{
						// diagnostics
					}
				}
				else
				{
					// diagnostics
				}
			}
			else
			{
				// diagnostics
			}

			//secondary buffer stuff

			DSBUFFERDESC BufferDescription = {};
			BufferDescription.dwSize = sizeof(BufferDescription);
			BufferDescription.dwFlags = 0;
			BufferDescription.dwBufferBytes = SoundOutput->SecondaryBufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;

			if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &SoundSecondaryBuffer, 0)))
			{

			}
			else
			{
				// diagnostics
			}
		}
	}
	else
	{
		// diagnostics
	}
}

void
FillSoundOutput(game_audio_output_buffer *GameAudio, win32_audio_output *SoundOutput,
                DWORD ByteToLock, DWORD BytesToWrite,
                wave_file *TESTWAVFILE)
{
	VOID *Region1;
	DWORD Region1Size;
	VOID *Region2;
	DWORD Region2Size;

	// if (SUCCEEDED(SoundSecondaryBuffer->Lock(ByteToLock, BytesToWrite,
	//               &Region1, &Region1Size,
	//               &Region2,  &Region2Size,
	//               0)))
	// {
	// 	DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
	// 	int16 *DestSample = (int16 *)Region1;
	// 	int16 *SourceSamples = GameAudio->Samples;
	// 	for (DWORD SampleIndex = 0;
	// 	     SampleIndex < Region1SampleCount;
	// 	     ++SampleIndex)
	// 	{
	// 		*DestSample++ = *SourceSamples++;
	// 		*DestSample++ = *SourceSamples++;
	// 		++SoundOutput->RunningSampleIndex;
	// 	}

	// 	DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
	// 	DestSample = (int16 *)Region2;
	// 	for (DWORD SampleIndex = 0;
	// 	     SampleIndex < Region2SampleCount;
	// 	     ++SampleIndex)
	// 	{
	// 		*DestSample++ = *SourceSamples++;
	// 		*DestSample++ = *SourceSamples++;
	// 		++SoundOutput->RunningSampleIndex;
	// 	}

	// 	SoundSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
	// }

	if (SUCCEEDED(SoundSecondaryBuffer->Lock(ByteToLock, BytesToWrite,
	              &Region1, &Region1Size,
	              &Region2,  &Region2Size,
	              0)))
	{
		DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
		int16 *DestSample = (int16 *)Region1;
		int16 *SourceSamples = (int16 *)TESTWAVFILE->Data;
		for (DWORD SampleIndex = 0;
		     SampleIndex < Region1SampleCount;
		     ++SampleIndex)
		{
			*DestSample++ = *SourceSamples++;
			*DestSample++ = *SourceSamples++;
			++SoundOutput->RunningSampleIndex;
		}

		DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
		DestSample = (int16 *)Region2;
		for (DWORD SampleIndex = 0;
		     SampleIndex < Region2SampleCount;
		     ++SampleIndex)
		{
			*DestSample++ = *SourceSamples++;
			*DestSample++ = *SourceSamples++;
			++SoundOutput->RunningSampleIndex;
		}

		SoundSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
	}
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

inline real32
GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
	real32 Result = ((real32)(End.QuadPart - Start.QuadPart) / (real32)PerfCountFrequency);
	return (Result);
}

void
SaveSate(char *FileName, game_memory *GameMemory)
{
	DebugLine("Saving State");

	HANDLE FileHandle = CreateFileA(FileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	// NOTE if totalsize if greater than 4gb then we must write a for loop to loop over the write file.
	DWORD BytesWritten = {};
	bool32 Success = WriteFile(FileHandle, GameMemory->GameMemoryBlock, (DWORD)GameMemory->TotalSize, &BytesWritten, 0);

	Assert(Success)
	CloseHandle(FileHandle);

	DebugLine("Save Complete");
}

void
LoadState(char *FileName, game_memory *GameMemory)
{
	HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ,  0, 0, OPEN_EXISTING, 0, 0);
	DWORD BytesRead;
	bool32 Success = ReadFile(FileHandle, GameMemory->GameMemoryBlock, (DWORD)GameMemory->TotalSize, &BytesRead, 0);

	if (!Success)
	{
		DebugLine("Loading State File Failed");
	}
	CloseHandle(FileHandle);
}

struct read_file_result
{
	uint32 ContentsSize;
	void *Contents;
};

read_file_result
GetFile(char *FileName)
{
	read_file_result Result = {};

	HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER FileSize;
		if (GetFileSizeEx(FileHandle, &FileSize))
		{
			uint32 FileSize32 = (uint32)FileSize.QuadPart;
			Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (Result.Contents)
			{
				DWORD BytesRead;
				if (ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0) &&
				    (FileSize32 == BytesRead))
				{
					// File read successfully
					Result.ContentsSize = FileSize32;
				}
				else
				{
					VirtualFree(Result.Contents, 0, MEM_RELEASE);
					Result.Contents = 0;
				}
			}
			else
			{

			}
		}
		else
		{

		}

		CloseHandle(FileHandle);
	}
	else
	{

	}

	return (Result);
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

			LARGE_INTEGER FlipWallClock = GetWallClock();

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

			FillPixels(&ScreenBuffer);
			Assert(ScreenBuffer.ScreenBuffer !=  NULL);


			#if INTERNAL
			LPVOID BaseAddress = (LPVOID)Terrabytes((uint64)2);
			#else
			LPVOID BaseAddress = 0;
			#endif
			game_memory GameMemory = {};
			GameMemory.PermanentStorageSize = Megabytes(64);
			GameMemory.TransientStorageSize = Megabytes((uint64)1);
			GameMemory.TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;

			GameMemory.GameMemoryBlock = VirtualAlloc(BaseAddress, GameMemory.TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			GameMemory.PermanentStorage = GameMemory.GameMemoryBlock;
			GameMemory.TransientStorage = (uint8 *)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize;



			read_file_result Result = GetFile("../assets/audio/testNote.wav");
			wave_file_header *WaveHeader = (wave_file_header *)Result.Contents;
			wave_file WaveFile = {};
			WaveFile.Header = *WaveHeader;
			WaveFile.Data = (uint32 *)((uint8 *)Result.Contents + 44);


			LARGE_INTEGER PreviousFrameCount = GetWallClock();

			win32_audio_output SoundOutput = {};
			SoundOutput.SamplesPerSecond = 48000;
			SoundOutput.ToneHz = 256;
			SoundOutput.RunningSampleIndex = 0;
			SoundOutput.SquareWavePeriod = SoundOutput.SamplesPerSecond / SoundOutput.ToneHz;
			SoundOutput.HalfSquareWavePeriod = SoundOutput.SquareWavePeriod / 2;
			SoundOutput.BytesPerSample = sizeof(int16) * 2;
			SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;
			SoundOutput.ToneVolume = 0.1f;
			bool SoundIsValid = false;

			int16 *AudioSamplesMemory = (int16 *)VirtualAlloc(0, SoundOutput.SecondaryBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			LoadDirectSound(WindowHandle, &SoundOutput);
			SoundSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

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

						ProcessButtonInput(&GameInput.AButton, Pad->wButtons & XINPUT_GAMEPAD_A);
						ProcessButtonInput(&GameInput.BButton, Pad->wButtons & XINPUT_GAMEPAD_B);
						ProcessButtonInput(&GameInput.XButton, Pad->wButtons & XINPUT_GAMEPAD_X);
						ProcessButtonInput(&GameInput.YButton, Pad->wButtons & XINPUT_GAMEPAD_Y);

						ProcessButtonInput(&GameInput.DUp, Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						ProcessButtonInput(&GameInput.DDown, Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						ProcessButtonInput(&GameInput.DLeft, Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						ProcessButtonInput(&GameInput.DRight, Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

						ProcessButtonInput(&GameInput.R1, Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						ProcessButtonInput(&GameInput.L1, Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						ProcessTriggerInput(&GameInput.R2, Pad->bRightTrigger);
						ProcessTriggerInput(&GameInput.L2, Pad->bLeftTrigger);

						ProcessButtonInput(&GameInput.Start, Pad->wButtons & XINPUT_GAMEPAD_START);
						ProcessButtonInput(&GameInput.Select, Pad->wButtons & XINPUT_GAMEPAD_BACK);

						GameInput.LeftStick.X = ClampValue(-0.9f, 0.9f, CheckStickDeadzone(Pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
						GameInput.LeftStick.Y = ClampValue(-0.9f, 0.9f, CheckStickDeadzone(Pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) * -1;

					}
					else
					{
						// Controller is not connected
					}
				}


				char *Slot1Name = "StateSlot1.ss";
				char *Slot2Name = "StateSlot2.ss";
				char *Slot3Name = "StateSlot3.ss";
				char *Slot4Name = "StateSlot4.ss";
				if (GameInput.R1.OnDown && GameInput.Select.IsDown)
				{
					SaveSate(Slot1Name, &GameMemory);
				}
				if (GameInput.R1.OnDown && !GameInput.Select.IsDown)
				{
					LoadState(Slot1Name, &GameMemory);
					FillPixels(&ScreenBuffer);
				}

				if (GameInput.L1.OnDown && GameInput.Select.IsDown)
				{
					SaveSate(Slot2Name, &GameMemory);
				}
				if (GameInput.L1.OnDown && !GameInput.Select.IsDown)
				{
					LoadState(Slot2Name, &GameMemory);
					FillPixels(&ScreenBuffer);
				}

				if (GameInput.R2.OnDown && GameInput.Select.IsDown)
				{
					SaveSate(Slot3Name, &GameMemory);
				}
				if (GameInput.R2.OnDown && !GameInput.Select.IsDown)
				{
					LoadState(Slot3Name, &GameMemory);
					FillPixels(&ScreenBuffer);
				}

				if (GameInput.L2.OnDown && GameInput.Select.IsDown)
				{
					SaveSate(Slot4Name, &GameMemory);
				}
				if (GameInput.L2.OnDown && !GameInput.Select.IsDown)
				{
					LoadState(Slot4Name, &GameMemory);
					FillPixels(&ScreenBuffer);
				}

				LARGE_INTEGER AudioWallClock = GetWallClock();
				real32 FromBeginToAudioSeconds = GetSecondsElapsed(FlipWallClock, AudioWallClock);

				DWORD PlayCursor;
				DWORD WriteCursor;
				game_audio_output_buffer GameAudio = {};
				DWORD BytesToWrite = 0;
				DWORD ByteToLock = 0;
				if (SoundSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor) == DS_OK)
				{
					if (!SoundIsValid)
					{
						SoundOutput.RunningSampleIndex = WriteCursor / SoundOutput.BytesPerSample;
						SoundIsValid = true;
					}

					ByteToLock = (SoundOutput.RunningSampleIndex * SoundOutput.BytesPerSample) %
					             SoundOutput.SecondaryBufferSize;


					DWORD ExpectedSoundBytesPerFrame = (int)(((real32)(SoundOutput.SamplesPerSecond *
					                                   SoundOutput.BytesPerSample)) /  GameUpdateHz);
					real64 SecondsLeftUntilFlip = (TargetSecondsElapsedPerFrame - FromBeginToAudioSeconds);
					DWORD ExpectedBytesUntilFlip = (DWORD)((SecondsLeftUntilFlip / TargetSecondsElapsedPerFrame
					                                       ) * (real32)ExpectedSoundBytesPerFrame);

					DWORD ExpectedFrameBoundaryByte = PlayCursor + ExpectedBytesUntilFlip;

					DWORD SafeWriteCursor = WriteCursor;
					if (SafeWriteCursor < PlayCursor)
					{
						SafeWriteCursor += SoundOutput.SecondaryBufferSize;
					}
					Assert(SafeWriteCursor >= PlayCursor);
					SafeWriteCursor += SoundOutput.SafetyBytes;

					bool32 AudioCardIsLowLatency = (SafeWriteCursor < ExpectedFrameBoundaryByte);

					DWORD TargetCursor = 0;
					if (AudioCardIsLowLatency)
					{
						TargetCursor = (ExpectedFrameBoundaryByte + ExpectedSoundBytesPerFrame);
					}
					else
					{
						TargetCursor = (WriteCursor + ExpectedSoundBytesPerFrame + SoundOutput.SafetyBytes);
					}
					TargetCursor = TargetCursor % SoundOutput.SecondaryBufferSize;

					if (ByteToLock > TargetCursor)
					{
						BytesToWrite = (SoundOutput.SecondaryBufferSize - ByteToLock) + TargetCursor;
					}
					else
					{
						BytesToWrite = TargetCursor - ByteToLock;
					}

					GameAudio.SamplesPerSecond = SoundOutput.SamplesPerSecond;
					GameAudio.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
					GameAudio.Samples = AudioSamplesMemory;
				}

				GameCode.GameLoop(&GameMemory, &GameInput, &ScreenBuffer, &GameAudio);
				FillSoundOutput(&GameAudio, &SoundOutput, ByteToLock, BytesToWrite, &WaveFile);

				game_state *GameStateFromMemory = (game_state *)GameMemory.PermanentStorage; \
				char *EmptyChar = "";
				if (GameStateFromMemory->DebugOutput != EmptyChar)
				{
					DebugLine(GameStateFromMemory->DebugOutput);
					GameStateFromMemory->DebugOutput = EmptyChar;
				}

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
				if (PRINTFPS)
				{
					DebugLine(charFPS);
				}

				PreviousFrameCount = WorkFrameCount;
				GameMemory.ElapsedCycles = PreviousFrameCount.QuadPart;

				FlipWallClock = GetWallClock();
			}
		}
	}
}