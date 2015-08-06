
#include "OriginTower.h"

static platform_read_file *PlatformReadFile;

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
	active_entity *EntityHit = {};

	if (Entity->CanCollide)
	{
		for (int EntityIndex = 0;
		     EntityIndex < GameState->WorldEntityCount;
		     EntityIndex++)
		{
			if (GameState->WorldEntities[EntityIndex] != Entity)
			{
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
					EntityHit = GameState->WorldEntities[EntityIndex];
				}
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
		real64 WidthSum = (EntityHit->Width / 2) + (Entity->Width / 2);

		vector2 NewPos = NewTestPos;
		vector2 NewVelocity = (InputForce * 0.9f) + Entity->Velocity;
		
		if (Entity->Position.X > (EntityHit->Position.X + WidthSum))
		{
			NewVelocity.X = 0;
			NewPos.X = EntityHit->Position.X + WidthSum + 0.1f;
		}
		if (Entity->Position.X < (EntityHit->Position.X - WidthSum))
		{
			NewVelocity.X = 0;
			NewPos.X = EntityHit->Position.X - WidthSum - 0.1f;
		}
		if (Entity->Position.Y > (EntityHit->Position.Y + WidthSum))
		{
			NewVelocity.Y = 0;
			NewPos.Y = EntityHit->Position.Y + WidthSum + 0.1f;
		}
		if (Entity->Position.Y < (EntityHit->Position.Y - WidthSum))
		{
			NewVelocity.Y = 0;
			NewPos.Y = EntityHit->Position.Y - WidthSum - 0.1f;
		}

		Entity->Position = NewPos;
		Entity->Velocity = NewVelocity;
	}
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
GLLoadBMP(char *FilePath)
{
	loaded_image Result = {};

	read_file_result FileResult = PlatformReadFile(FilePath);
	uint32 *BitmapPixels = {};
	if (FileResult.ContentsSize != 0)
	{
		bmp_header *Header = (bmp_header *)FileResult.Contents;
		Result.Width = Header->Width;
		Result.Height = Header->Height;

		// NOTE this number offset here is pulled from my ass. The offset in the image doesn't seem to work.
		BitmapPixels = ((uint32 *)FileResult.Contents + 35);
	}

	uint32 *Source = (uint32 *)BitmapPixels;
	for (uint32 PixelIndex = 0;
	     PixelIndex < (Result.Width * Result.Height);
	     ++PixelIndex)
	{
		uint8 *Pixel = (uint8 *)Source;
		uint8 Bit2 = *Pixel++; // A
		uint8 Bit3 = *Pixel++; // R
		uint8 Bit0 = *Pixel++; // G
		uint8 Bit1 = *Pixel++; // B

		*Source++ = (Bit0 << 24) | (Bit1 << 16) | (Bit2 << 8) | (Bit3 << 0);
	}

	glGenTextures(1, &Result.GLTexture);
	glBindTexture(GL_TEXTURE_2D, Result.GLTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	             Result.Width, Result.Height,
	             0, GL_RGBA, GL_UNSIGNED_BYTE, BitmapPixels);

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

	GameState->BackgroundImage = GLLoadBMP("../assets/Background.bmp");

	// NOTE this line is necessary to initialize the DebugOuput var of GameState. It must be initialized to something.
	DebugLine("Loaded", GameState);
}

gl_square
MakeSquare(vector2 Pos, int32 SideLength, color Color)
{
	gl_square Result = {};

	Result.Color = Color;

	int32 HalfSide = SideLength / 2;
	Result.TopLeft = vector2{Pos.X - HalfSide, Pos.Y - HalfSide};
	Result.TopRight = vector2{Pos.X + HalfSide, Pos.Y - HalfSide};
	Result.BottomLeft = vector2{Pos.X - HalfSide, Pos.Y + HalfSide};
	Result.BottomRight = vector2{Pos.X + HalfSide, Pos.Y + HalfSide};

	return (Result);
}

void
UpdateSquare(active_entity *Entity)
{
	int32 HalfSide = Entity->Width / 2;
	Entity->GraphicSquare.TopLeft = vector2{Entity->Position.X - HalfSide, Entity->Position.Y - HalfSide};
	Entity->GraphicSquare.TopRight = vector2{Entity->Position.X + HalfSide, Entity->Position.Y - HalfSide};
	Entity->GraphicSquare.BottomLeft = vector2{Entity->Position.X - HalfSide, Entity->Position.Y + HalfSide};
	Entity->GraphicSquare.BottomRight = vector2{Entity->Position.X + HalfSide, Entity->Position.Y + HalfSide};
}

extern "C" GAME_LOOP(GameLoop)
{
	PlatformReadFile = Memory->PlatformReadFile;

	Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
	game_state *GameState = (game_state *)Memory->PermanentStorage;
	if (!Memory->IsInitialized)
	{
		GameState->PrintFPS = true;

		LoadAssets(GameState);

		// NOTE maybe this method overwrites memory??
		uint16 PosCount = 0;
		uint16 XCount = 10;
		uint16 YCount = 10;
		GameState->BackgroundPositionsCount = XCount * YCount;
		for (uint16 BackgroundX = 0;
		     BackgroundX < XCount;
		     BackgroundX++)
		{
			for (uint16 BackgroundY = 0;
			     BackgroundY < YCount;
			     BackgroundY++)
			{
				GameState->BackgroundPositions[PosCount] = vector2{BackgroundX * GameState->BackgroundImage.Width,
				                                                   BackgroundY * GameState->BackgroundImage.Height};
				PosCount++;
			}
		}

		GameState->Player.Entity.Position.X = WindowInfo->Width / 2;
		GameState->Player.Entity.Position.Y = WindowInfo->Height / 2;
		GameState->Player.Entity.Width = 50;
		GameState->Player.Entity.CanCollide = true;
		GameState->Player.Entity.MovementSpeed = 3;
		GameState->Player.Entity.GraphicSquare = MakeSquare(GameState->Player.Entity.Position, GameState->Player.Entity.Width, COLOR_BLUE);
		GameState->GLSquares[0] = &GameState->Player.Entity.GraphicSquare;

		GameState->Enemy.Position.X = 700.0f;
		GameState->Enemy.Position.Y = 700.0f;
		GameState->Enemy.Width = 100;
		GameState->Enemy.CanCollide = true;
		GameState->Enemy.MovementSpeed = 1;
		GameState->Enemy.GraphicSquare = MakeSquare(GameState->Enemy.Position, GameState->Enemy.Width, COLOR_GREEN);
		GameState->GLSquares[1] = &GameState->Enemy.GraphicSquare;

		GameState->WorldEntityCount = 2;
		GameState->SquareCount = GameState->WorldEntityCount;
		GameState->WorldEntities[0] = &GameState->Enemy;
		GameState->WorldEntities[1] = &GameState->Player.Entity;

		GameState->WorldCenter = vector2{0, 0};
		GameState->CamCenter = vector2{WindowInfo->Width / 2, WindowInfo->Height / 2};

		AudioBuffer->RunningSampleIndex = 0;

		Memory->IsInitialized = true;

		// NOTE this line is necessary to initialize the DebugOuput var of GameState. It must be initialized to something.
		DebugLine("Initialized", GameState);
	}

	player *Player = &GameState->Player;
	active_entity *Enemy = &GameState->Enemy;

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


	vector2 DirectionPos = {};
	if (GameInput->YButton.IsDown)
	{
		DirectionPos = Enemy->Position - Player->Entity.Position;
		DirectionPos = -1 * NormalizeVector2(DirectionPos);
	}
	VectorForceEntity(Enemy, DirectionPos, GameState);

	vector2 PrevPlayerPos = Player->Entity.Position;
	VectorForceEntity(&Player->Entity, NormalizeVector2(GameInput->LeftStick), GameState);

	vector2 PlayerCamDifference = Player->Entity.Position - GameState->WorldCenter;
	GameState->WorldCenter = GameState->WorldCenter + (PlayerCamDifference * 0.08f);


	for (int EntityIndex = 0;
	     EntityIndex < GameState->WorldEntityCount;
	     EntityIndex++)
	{
		UpdateSquare(GameState->WorldEntities[EntityIndex]);
	}
}

extern "C" GAME_LOAD_ASSETS(GameLoadAssets)
{
	game_state *GameState = (game_state *)Memory->PermanentStorage;
	LoadAssets(GameState);
}