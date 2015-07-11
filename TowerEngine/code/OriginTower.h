
#include <windows.h>
#include <xinput.h>


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

struct active_entity
{
	vector2 Position;
	vector2 DeltaPosition;

	uint16 Width;
	color Color;
	real32 MovementSpeed;
};

struct player
{
	active_entity Entity;
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

struct game_state
{
	player Player;
	active_entity Enemy;

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


#define GAME_UPDATE_AND_RENDER(name) void name(game_memory *Memory, game_input *GameInput, screen_buffer *ScreenBuffer)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
}