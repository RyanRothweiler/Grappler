int64
SquareInt(int64 input)
{
	return (input * input);
}

real64
SquareReal(real64 input)
{
	return (input * input);
}

real64
ClampValue(real64 Bottom, real64 Top, real64 Value)
{
	if (Value <= Bottom)
	{
		return (Bottom);
	}
	if (Value >= Top)
	{
		return (Top);
	}
	return (Value);
}


real64
SquareRoot(real64 num)
{
	real64 i = 0;
	real64 x1, x2;

	while ( (i * i) <= num )
	{
		i += 0.1f;
	}
	x1 = i;
	for (int j = 0; j < 10; j++)
	{
		x2 = num;
		x2 /= x1;
		x2 += x1;
		x2 /= 2;
		x1 = x2;
	}
	return x2;
}
