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


extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
	game_state *GameState = (game_state *)Memory->PermanentStorage;
	if (!Memory->IsInitialized)
	{
		GameState->Wall.Position.X = 20;
		GameState->Wall.Position.Y = 20;
		GameState->Wall.Width = 10;
		// GameState->Wall.Color = COLOR_WHITE;

		GameState->Player.Position.X = (real64)(ScreenBuffer->Width / 2);
		GameState->Player.Position.Y = (real64)(ScreenBuffer->Height / 2);
		GameState->Player.Width = 50;
		GameState->Player.Color.R = 255;
		GameState->Player.Color.G = 100;
		GameState->Player.Color.B = 100;
		GameState->Player.Speed = 3;

		Memory->IsInitialized = true;
	}

	player *Player = &GameState->Player;
	wall *Wall = &GameState->Wall;

	// GameState->Player.Color.R = 0;
	// GameState->Player.Width = 1;

	DrawSquare((uint32)Player->Position.X, (uint32)Player->Position.Y, Player->Width, ScreenBuffer->BackgroundColor, *ScreenBuffer);
	vector2 InputForce = NormalizeVector2(GameInput->LeftStick) * Player->Speed;
	InputForce = InputForce + (-0.25f * Player->DeltaPlayerPosition);
	// NOTE these 0.9f here should actually be the previous elapsed frame time. Maybe do that at some point
	Player->Position = (0.5f * InputForce * SquareInt((int64)(0.9f))) + (Player->DeltaPlayerPosition * 0.9f) + Player->Position;
	Player->DeltaPlayerPosition = (InputForce * 0.9f) + Player->DeltaPlayerPosition;
	DrawSquare((uint32)Player->Position.X, (uint32)Player->Position.Y, Player->Width, Player->Color, *ScreenBuffer);

	DrawSquare((uint32)Wall->Position.X, (uint32)Wall->Position.Y, Wall->Width, Wall->Color, *ScreenBuffer);

	if (GameInput->DUp)
	{
		Assert(0);
	}
}