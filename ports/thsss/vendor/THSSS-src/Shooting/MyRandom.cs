using System;

namespace Shooting;

public class MyRandom : Random
{
	public int ranValue { get; private set; }

	public MyRandom()
	{
	}

	public MyRandom(int Seed)
		: base(Seed)
	{
	}

	public override int Next()
	{
		ranValue = base.Next();
		return ranValue;
	}

	public override int Next(int maxValue)
	{
		ranValue = base.Next(maxValue);
		return ranValue;
	}

	public override int Next(int minValue, int maxValue)
	{
		ranValue = base.Next(minValue, maxValue);
		return ranValue;
	}

	public override double NextDouble()
	{
		double num = base.NextDouble();
		ranValue = (int)(num * 100.0);
		return num;
	}

	public override void NextBytes(byte[] buffer)
	{
		ranValue = 0;
		base.NextBytes(buffer);
	}

	public double NextPMDouble()
	{
		return 2.0 * NextDouble() - 1.0;
	}
}
