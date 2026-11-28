using System.Diagnostics;
using System.Threading;

namespace Shooting;

public class Watch
{
	private long startTime { get; set; }

	public void Start()
	{
		startTime = Stopwatch.GetTimestamp();
	}

	public float GetDuration()
	{
		if (startTime == 0L)
		{
			return 0f;
		}
		return 1000f * (float)(Stopwatch.GetTimestamp() - startTime) / (float)Stopwatch.Frequency;
	}

	public void IncreaseOneFrame()
	{
		startTime += Stopwatch.Frequency / 60;
	}

	public void SleepTo(float Time)
	{
		Time -= GetDuration();
		if (Time > 0f)
		{
			Thread.Sleep((int)Time);
		}
	}
}
