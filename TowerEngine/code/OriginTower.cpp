
#include "OriginTower.h"

void
DrawSquare(int32 XPos, int32 YPos, uint32 squareSize, color Color,
           screen_buffer ScreenBuffer)
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

void
VectorForceEntity(active_entity *Entity, vector2 InputForce)
{
	InputForce = (InputForce * Entity->MovementSpeed) + (-0.25f * Entity->DeltaPosition);
	// NOTE these 0.9f here should actually be the previous elapsed frame time. Maybe do that at some point
	Entity->Position = (0.5f * InputForce * SquareInt((int64)(0.9f))) + (Entity->DeltaPosition * 0.9f) + Entity->Position;
	Entity->DeltaPosition = (InputForce * 0.9f) + Entity->DeltaPosition;
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
	game_state *GameState = (game_state *)Memory->PermanentStorage;
	if (!Memory->IsInitialized)
	{
		GameState->Player.Entity.Position.X = (real64)(ScreenBuffer->Width / 3);
		GameState->Player.Entity.Position.Y = (real64)(ScreenBuffer->Height / 3);
		GameState->Player.Entity.Width = 50;
		GameState->Player.Entity.Color = COLORRED;
		GameState->Player.Entity.MovementSpeed = 3;

		GameState->Enemy.Position.X = (real64)(ScreenBuffer->Width / 2);
		GameState->Enemy.Position.Y = (real64)(ScreenBuffer->Height / 2);
		GameState->Enemy.Width = 50;
		GameState->Enemy.Color = COLORGREEN;
		GameState->Enemy.MovementSpeed = 1;

		Memory->IsInitialized = true;
	}

	player *Player = &GameState->Player;
	active_entity *Enemy = &GameState->Enemy;


	DrawSquare((uint32)Enemy->Position.X, (uint32)Enemy->Position.Y, Enemy->Width, ScreenBuffer->BackgroundColor, *ScreenBuffer);
	DrawSquare((uint32)Player->Entity.Position.X, (uint32)Player->Entity.Position.Y, Player->Entity.Width, ScreenBuffer->BackgroundColor, *ScreenBuffer);

	vector2 DirectionPos = {};
	if (GameInput->YButton.IsDown)
	{
		DirectionPos = Enemy->Position - Player->Entity.Position;
		DirectionPos = -1 * NormalizeVector2(DirectionPos);
	}
	VectorForceEntity(Enemy, DirectionPos);

	VectorForceEntity(&Player->Entity, NormalizeVector2(GameInput->LeftStick));

	DrawSquare((uint32)Player->Entity.Position.X, (uint32)Player->Entity.Position.Y, Player->Entity.Width, Player->Entity.Color, *ScreenBuffer);
	DrawSquare((uint32)Enemy->Position.X, (uint32)Enemy->Position.Y, Enemy->Width, Enemy->Color, *ScreenBuffer);

}