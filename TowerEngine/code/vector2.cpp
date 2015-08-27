#include "Vector2.h"

vector2 operator+(vector2 A, vector2 B)
{
	vector2 Output;
	Output.X = A.X + B.X;
	Output.Y = A.Y + B.Y;
	return (Output);
}

vector2 operator+(real64 B, vector2 A)
{
	vector2 Output;
	Output.X = A.X + B;
	Output.Y = A.Y + B;
	return (Output);
}

vector2 operator-(vector2 A, vector2 B)
{
	vector2 Output;
	Output.X = A.X - B.X;
	Output.Y = A.Y - B.Y;
	return (Output);
}

vector2 operator/(vector2 A, int B)
{
	vector2 Output;
	Output.X = A.X / B;
	Output.Y = A.Y / B;
	return (Output);
}

vector2 operator*(vector2 A, real64 B)
{
	vector2 Output;
	Output.X = A.X * B;
	Output.Y = A.Y * B;
	return (Output);
}

vector2 operator*(vector2 A, float B)
{
	vector2 Output;
	Output.X = A.X * B;
	Output.Y = A.Y * B;
	return (Output);
}

vector2 operator*(float B, vector2 A)
{
	vector2 Output;
	Output.X = A.X * B;
	Output.Y = A.Y * B;
	return (Output);
}

vector2 operator*(vector2 A, int64 B)
{
	vector2 Output;
	Output.X = A.X * B;
	Output.Y = A.Y * B;
	return (Output);
}

vector2 operator*(vector2 A, vector2 B)
{
	vector2 Output;
	Output.X = A.X * B.X;
	Output.Y = A.Y * B.Y;
	return (Output);
}

bool32 operator>(vector2 A, int32 B)
{
	if (A.X > B &&
	    A.Y > B)
	{
		return (true);
	}
	return (false);
}

real64
Vector2Length(vector2 A)
{
	return (SquareRoot((A.X * A.X) + (A.Y * A.Y)));
}

vector2
Vector2Normalize(vector2 Input)
{
	vector2 Output = {};

	real64 Length = Vector2Length(Input);
	Output.X = Input.X / Length;
	Output.Y = Input.Y / Length;

	return (Output);
}

real64
Vector2Distance(vector2 A, vector2 B)
{
	real64 Output = 0;
	real64 ASideLength = Abs(A.X - B.X);
	real64 BSideLength = Abs(A.Y - B.Y);
	Output = SquareRoot((ASideLength * ASideLength) + (BSideLength * BSideLength));
	return (Abs(Output));
}


const vector2 VECTOR2_ZERO = vector2{0, 0};