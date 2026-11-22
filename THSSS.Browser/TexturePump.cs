using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using SlimDX.Direct3D9;
using Shooting;

namespace Thsss.Browser
{
	public static class TexturePump
	{
		private const int MaxInFlight = 2;

		private static readonly List<ImageJob> jobs = new List<ImageJob>();

		public static void Advance(GlobalDataPackage data)
		{
			if (data == null || data.LoadingList == null)
			{
				return;
			}
			for (var i = 0; i < jobs.Count; i++)
			{
				var state = ThsssJs.ImageState(jobs[i].Id);
				if (state == 1)
				{
					var glId = ThsssJs.FinishImage(jobs[i].Id);
					var width = ThsssJs.ImageWidth(jobs[i].Id);
					var height = ThsssJs.ImageHeight(jobs[i].Id);
					if (glId != 0 && width > 0 && height > 0)
					{
						try
						{
							data.RegisterLoadedTexture(jobs[i].Path, Texture.Wrap(glId, width, height));
						}
						catch (Exception ex)
						{
							Console.WriteLine(jobs[i].Path + " " + ex.Message);
						}
					}
					data.LoadingList.Remove(jobs[i].Path);
					jobs.RemoveAt(i);
					break;
				}
				if (state < 0)
				{
					Console.WriteLine("Image decode failed " + jobs[i].Path);
					data.LoadingList.Remove(jobs[i].Path);
					jobs.RemoveAt(i);
					break;
				}
			}
			if (jobs.Count >= MaxInFlight)
			{
				return;
			}
			var start = Stopwatch.GetTimestamp();
			var started = 0;
			for (var n = 0; n < data.LoadingList.Count && jobs.Count < MaxInFlight && started < 3; n++)
			{
				if (started > 0 && Stopwatch.GetElapsedTime(start).TotalMilliseconds > 4.0)
				{
					break;
				}
				var path = data.LoadingList[n];
				if (HasJob(path))
				{
					continue;
				}
				if (!ContentLoader.IsFileReady(path))
				{
					continue;
				}
				if (!File.Exists(path))
				{
					if (!ContentLoader.BackgroundIdle)
					{
						continue;
					}
					data.LoadingList.RemoveAt(n);
					n--;
					continue;
				}
				var bytes = File.ReadAllBytes(path);
				jobs.Add(new ImageJob
				{
					Path = path,
					Id = ThsssJs.BeginImage(bytes)
				});
				started++;
			}
		}

		private static bool HasJob(string path)
		{
			for (var i = 0; i < jobs.Count; i++)
			{
				if (string.Equals(jobs[i].Path, path, StringComparison.OrdinalIgnoreCase))
				{
					return true;
				}
			}
			return false;
		}

		private sealed class ImageJob
		{
			public string Path;

			public int Id;
		}
	}
}
