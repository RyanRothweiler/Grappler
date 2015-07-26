
#include "OriginTower.h"

static platform_read_file *PlatformReadFile;

void
DrawSquare(int32 XPos, int32 YPos, uint32 squareSize, color Color,
           screen_buffer ScreenBuffer)
{
	int32 MinX = XPos - (squareSize / 2);
	int32 MinY = YPos - (squareSize / 2);
	int32 MaxX = XPos + (squareSize / 2);
	int32 MaxY = YPos + (squareSize / 2);

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

int64
Lerp(int64 a, int64 b, real32 t)
{
	return (int64)(a + t * (b - a));

}

// NOTE right now collision detection only works for squares
void
VectorForceEntity(active_entity *Entity, vector2 InputForce, game_state *GameState)
{
	InputForce = (InputForce * Entity->MovementSpeed) + (-0.25f * Entity->Velocity);
	// NOTE these 0.9f here should actually be the previous elapsed frame time. Maybe do that at some point
	vector2 NewTestPos = (0.5f * InputForce * SquareInt((int64)(0.9f))) + (Entity->Velocity * 0.9f) + Entity->Position;

	bool32 CollisionDetected = false;
	for (int EntityIndex = 0;
	     EntityIndex < GameState->WorldEntityCount;
	     EntityIndex++)
	{
		if (GameState->WorldEntities[EntityIndex] != Entity)
		{
			// real64 WidthAdding = Entity->Width;
			real64 WidthAdding = Entity->Width;
			vector2 EntityTopLeft =
			{
				GameState->WorldEntities[EntityIndex]->Position.X - ((GameState->WorldEntities[EntityIndex]->Width + WidthAdding) / 2),
				GameState->WorldEntities[EntityIndex]->Position.Y - ((GameState->WorldEntities[EntityIndex]->Width + WidthAdding) / 2)
			};
			vector2 EntityBottomRight =
			{
				GameState->WorldEntities[EntityIndex]->Position.X + ((GameState->WorldEntities[EntityIndex]->Width + WidthAdding) / 2),
				GameState->WorldEntities[EntityIndex]->Position.Y + ((GameState->WorldEntities[EntityIndex]->Width + WidthAdding) / 2)
			};

			if (NewTestPos.X > EntityTopLeft.X &&
			    NewTestPos.X < EntityBottomRight.X &&
			    NewTestPos.Y > EntityTopLeft.Y &&
			    NewTestPos.Y < EntityBottomRight.Y)
			{
				CollisionDetected = true;
			}
		}
	}

	if (!CollisionDetected)
	{
		Entity->Position = NewTestPos;
		Entity->Velocity = (InputForce * 0.9f) + Entity->Velocity;
	}
	else
	{
		Entity->Velocity = vector2{0, 0};
	}
}

// NOTE doesn't really work the best. Need to create some debug overlay system.
void
VisualizeDirectionVector(vector2 Vector, active_entity *Entity, screen_buffer ScreenBuffer)
{
	DrawSquare((int32)(Entity->Position.X + Vector.X), (int32)(Entity->Position.Y + Vector.Y), 5, COLORRED, ScreenBuffer);
}

// NOTE this debug line only outputs after the end of the GameLoop.
void
DebugLine(char *Output, game_state *GameState)
{
	GameState->DebugOutput = Output;
}

inline riff_iterator
ParseChunkAt(void *At, void *Stop)
{
	riff_iterator Iter;

	Iter.At = (uint8 *)At;
	Iter.Stop = (uint8 *)Stop;

	return (Iter);
}

inline riff_iterator
NextChunk(riff_iterator Iter)
{
	wave_chunk *Chunk = (wave_chunk *)Iter.At;
	uint32 Size = (Chunk->Size + 1) & ~1;
	Iter.At += sizeof(wave_chunk) + Size;

	return (Iter);
}

inline bool32
IsValid(riff_iterator Iter)
{
	bool32 Result = (Iter.At < Iter.Stop);
	return (Result);
}

inline void *
GetChunkData(riff_iterator Iter)
{
	void *Result = (Iter.At + sizeof(wave_chunk));
	return (Result);
}

inline uint32
GetType(riff_iterator Iter)
{
	wave_chunk *Chunk = (wave_chunk *)Iter.At;
	uint32 Result = Chunk->ID;

	return (Result);
}

uint32
GetChunkDataSize(riff_iterator Iter)
{
	wave_chunk *Chunk = (wave_chunk *)Iter.At;
	uint32 Result = Chunk->Size;

	return (Result);
}

loaded_sound
LoadWave(char *FilePath)
{
	loaded_sound Result = {};

	//NOTE this is the loading the wave file code. Maybe all this should be pulled into the game layer
	read_file_result WaveResult = PlatformReadFile(FilePath);
	wave_header *WaveHeader = (wave_header *)WaveResult.Contents;
	Assert(WaveHeader->RiffID == WAVE_ChunkID_RIFF);
	Assert(WaveHeader->WaveID == WAVE_ChunkID_WAVE);

	uint32 ChannelCount = 0;
	uint32 SampleDataSize = 0;
	int16 *SampleData = 0;
	for (riff_iterator Iter = ParseChunkAt(WaveHeader + 1, (uint8 *)(WaveHeader + 1) + WaveHeader->Size - 4);
	     IsValid(Iter);
	     Iter = NextChunk(Iter))
	{
		switch (GetType(Iter))
		{
			case WAVE_ChunkID_fmt:
			{
				wave_fmt *fmt = (wave_fmt *)GetChunkData(Iter);

				// NOTE Assert that this file is in a supported format
				// Using PCM format;
				Assert(fmt->Format == 1);
				Assert(fmt->NumSamplesPerSecond == 48000);
				Assert(fmt->BitsPerSample == 16);
				Assert(fmt->BlockAlign == (sizeof(int16) * fmt->NumberOfChannels));
				ChannelCount = fmt->NumberOfChannels;
			} break;
			case WAVE_ChunkID_data:
			{
				SampleData = (int16 *)GetChunkData(Iter);
				SampleDataSize = GetChunkDataSize(Iter);
			} break;
		}
	}

	Result.ChannelCount = ChannelCount;
	Result.SampleCount = SampleDataSize / (ChannelCount * sizeof(int16));
	if (ChannelCount == 1)
	{
		Result.Samples[0] = SampleData;
		Result.Samples[1] = 0;
	}
	else if (ChannelCount == 2)
	{

		Result.Samples[0] = SampleData;
		Result.Samples[1] = SampleData + Result.SampleCount;

		for (uint32 SampleIndex = 0;
		     SampleIndex < Result.SampleCount;
		     ++SampleIndex)
		{
			int16 Source = SampleData[2 * SampleIndex];
			SampleData[2 * SampleIndex] = SampleData[SampleIndex];
			SampleData[SampleIndex] = Source;
		}
	}
	else
	{
		Assert(!"Invalid Channel Count");
	}

	// TODO this only loads the left channel. MAYBE load the right channel too
	Result.ChannelCount = 1;

	return (Result);
}

loaded_image
LoadBMP(char *FilePath)
{
	loaded_image Result = {};

	//NOTE this is the loading the wave file code. Maybe all this should be pulled into the game layer
	read_file_result FileResult = PlatformReadFile(FilePath);
	bmp_header *Header = (bmp_header *)FileResult.Contents;

	Assert(0);

	return (Result);
}

void
LoadAssets(game_state *GameState)
{
	// NOTE currently sound is forced at...
	// Mono, 16 bit, 48000.
	// anything else won't work. need to implement stereo sound at some point.
	GameState->TestNoteSampleIndex = 0;
	GameState->TestNote = LoadWave("../assets/testNote.wav");

	GameState->TestImage = LoadBMP("../assets/TestImage.bmp");

	// NOTE this line is necessary to initialize the DebugOuput var of GameState. It must be initialized to something.
	DebugLine("Loaded", GameState);
}

extern "C" GAME_LOOP(GameLoop)
{
	PlatformReadFile = Memory->PlatformReadFile;

	Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
	game_state *GameState = (game_state *)Memory->PermanentStorage;
	if (!Memory->IsInitialized)
	{
		GameState->PrintFPS = false;

		GameState->Player.Entity.Position.X = (real64)(ScreenBuffer->Width / 2);
		GameState->Player.Entity.Position.Y = (real64)(ScreenBuffer->Height / 2);
		GameState->Player.Entity.Width = 50;
		GameState->Player.Entity.Color = COLORRED;
		GameState->Player.Entity.MovementSpeed = 3;

		GameState->Enemy.Position.X = (real64)(ScreenBuffer->Width / 3);
		GameState->Enemy.Position.Y = (real64)(ScreenBuffer->Height / 3);
		GameState->Enemy.Width = 50;
		GameState->Enemy.Color = COLORGREEN;
		GameState->Enemy.MovementSpeed = 1;

		GameState->WorldEntityCount = 2;
		GameState->WorldEntities[0] = &GameState->Enemy;
		GameState->WorldEntities[1] = &GameState->Player.Entity;

		GameState->CameraFollowCoefficient = 0.0f;
		GameState->Camera.Position = GameState->Player.Entity.Position;
		GameState->Camera.MovementSpeed = 3;
		GameState->Camera.Width = 0;

		LoadAssets(GameState);

		AudioBuffer->RunningSampleIndex = 0;

		Memory->IsInitialized = true;

		// NOTE this line is necessary to initialize the DebugOuput var of GameState. It must be initialized to something.
		DebugLine("Initialized", GameState);
	}

	player *Player = &GameState->Player;
	active_entity *Enemy = &GameState->Enemy;
	active_entity *Camera = &GameState->Camera;

	if (GameInput->BButton.OnDown)
	{
		GameState->TestNoteSampleIndex = 0;
	}

	// NOTE Sound doesn't work well at all when fps drops below 60
	int16 *SampleOut = AudioBuffer->Samples;
	for (int SampleIndex = 0;
	     SampleIndex < AudioBuffer->SampleCount;
	     ++SampleIndex)
	{
		int16 SampleValue = 0;

		uint32 SampleValueIndex = (GameState->TestNoteSampleIndex + SampleIndex);
		if (SampleValueIndex < GameState->TestNote.SampleCount)
		{
			SampleValue = GameState->TestNote.Samples[0][SampleValueIndex];
		}
		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;
	}
	GameState->TestNoteSampleIndex += AudioBuffer->SampleCount;


	for (int EntityIndex = 0;
	     EntityIndex < GameState->WorldEntityCount;
	     EntityIndex++)
	{
		active_entity *EntityAbout = GameState->WorldEntities[EntityIndex];
		DrawSquare((uint32)EntityAbout->Position.X, (uint32)EntityAbout->Position.Y,
		           EntityAbout->Width, ScreenBuffer->BackgroundColor, *ScreenBuffer);
	}

	vector2 DirectionPos = {};
	if (GameInput->YButton.IsDown)
	{
		DirectionPos = Enemy->Position - Player->Entity.Position;
		DirectionPos = -1 * NormalizeVector2(DirectionPos);
	}
	VectorForceEntity(Enemy, DirectionPos, GameState);

	vector2 PrevPlayerPos = Player->Entity.Position;
	VectorForceEntity(&Player->Entity, NormalizeVector2(GameInput->LeftStick), GameState);
	vector2 DeltaPlayerPos = Player->Entity.Position - PrevPlayerPos;

	vector2 PrevCamPos = Camera->Position;
	// real64 PlayerCameraDist = Vector2Distance(Player->Entity.Position, Camera->Position);
	// GameState->CameraFollowCoefficient = 1.0f;
	// if (PlayerCameraDist > 200)
	// {
	// 	GameState->CameraFollowCoefficient = 1.0f;
	// }
	// GameState->CameraFollowCoefficient = ClampValue(0.0f, 1.0f, GameState->CameraFollowCoefficient);
	// VectorForceEntity(Camera, NormalizeVector2(GameInput->LeftStick) * GameState->CameraFollowCoefficient, GameState);
	vector2 DeltaCamPos = Camera->Position - PrevCamPos;


	for (int EntityIndex = 0;
	     EntityIndex < GameState->WorldEntityCount;
	     EntityIndex++)
	{
		active_entity *EntityAbout = GameState->WorldEntities[EntityIndex];
		EntityAbout->Position = EntityAbout->Position - DeltaCamPos;
	}

	for (int EntityIndex = 0;
	     EntityIndex < GameState->WorldEntityCount;
	     EntityIndex++)
	{
		active_entity *EntityAbout = GameState->WorldEntities[EntityIndex];
		DrawSquare((uint32)EntityAbout->Position.X, (uint32)EntityAbout->Position.Y,
		           EntityAbout->Width, EntityAbout->Color, *ScreenBuffer);
	}
}

extern "C" GAME_LOAD_ASSETS(GameLoadAssets)
{
	game_state *GameState = (game_state *)Memory->PermanentStorage;
	LoadAssets(GameState);
}