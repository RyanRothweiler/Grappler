
#include <windows.h>
#include <xinput.h>
#include <dsound.h>


#define internal static

#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)
#define Terrabytes(value) (Megabytes(value) * 1024)



/*
	SLOW:
		0 - No slow code allowed
		1 - Slow code allowed

	INTernAL:
		0 - Build on dev machine
		1 - Build on release machine
*/


#if SLOW
	#define Assert(Expression) if (!(Expression)) {*(int *)0 = 0;}
#else
	#define Assert()
#endif


typedef _int8 int8;
typedef _int16 int16;
typedef _int32 int32;
typedef _int64 int64;

typedef unsigned _int8 uint8;
typedef unsigned _int16 uint16;
typedef unsigned _int32 uint32;
typedef unsigned _int64 uint64;

typedef int32 bool32;

typedef float real32;
typedef double real64;

#include "Math.cpp"
#include "vector2.cpp"
#include "Color.h"


struct wave_file_header
{
	DWORD ChunkID;
	DWORD ChunkSize;
	DWORD Format;
	DWORD SubChunkID;
	DWORD SubChunk1Size;
	WORD AudioFormat;
	WORD NumChannels;
	DWORD SampleRate;
	DWORD ByteRate;
	WORD BlockAlign;
	WORD BitsPerSample;
	DWORD SubChunk2ID;
	DWORD SubChunk2Siz;
	DWORD DataSize;
};

struct wave_file
{
	wave_file_header Header;
	void *Data;
};

struct win32_audio_output
{
	int SamplesPerSecond;
	int ToneHz;
	int SquareWavePeriod;
	int BytesPerSample;
	int SecondaryBufferSize;
	real32 ToneVolume;
	int HalfSquareWavePeriod;
	uint32 RunningSampleIndex;
	DWORD SafetyBytes;
};

struct game_audio_output_buffer
{
	// NOTE this running sample index is only for creating the sinwave. We don't need this otherwise. Remove this eventually.
	uint32 RunningSampleIndex;

	int SamplesPerSecond;
	int SampleCount;
	int16 *Samples;
};

struct screen_buffer
{
	DWORD Width;
	DWORD Height;

	uint32 BytesPerPixel = 4;
	void *ScreenBuffer;

	color BackgroundColor;
};

struct input_button
{
	bool32 OnDown;
	bool32 OnUp;
	bool32 IsDown;
	bool32 IsUp;
};

struct game_input
{
	input_button AButton;
	input_button BButton;
	input_button XButton;
	input_button YButton;

	input_button DUp;
	input_button DRight;
	input_button DLeft;
	input_button DDown;

	input_button R1;
	input_button R2;
	input_button L1;
	input_button L2;

	input_button Select;
	input_button Start;

	vector2 LeftStick;
	bool32 LeftStickButton;

	bool32 RightStickButton;
};

struct active_entity
{
	vector2 Position;
	vector2 Velocity;

	uint16 Width;
	color Color;
	real32 MovementSpeed;
};

struct player
{
	active_entity Entity;
};

struct game_state
{
	int32 WorldEntityCount;
	active_entity *WorldEntities[2];

	player Player;
	active_entity Enemy;

	real64 CameraFollowCoefficient;
	active_entity Camera;

	char *DebugOutput = "";

	wave_file WaveToneFile;

};

struct game_memory
{
	bool32 IsInitialized;

	uint64 PermanentStorageSize;
	void *PermanentStorage; // NOTE Required to be cleared to 0 on startup / allocation
	uint64 TransientStorageSize;
	void *TransientStorage;

	uint64 TotalSize;
	void *GameMemoryBlock;

	int64 ElapsedCycles;
};


#define GAME_LOOP(name) void name(game_memory *Memory, game_input *GameInput, screen_buffer *ScreenBuffer, game_audio_output_buffer *AudioBuffer)
typedef GAME_LOOP(game_update_and_render);
GAME_LOOP(GameLoopStub)
{
}