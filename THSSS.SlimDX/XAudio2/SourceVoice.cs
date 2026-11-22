using System;
using System.IO;
using SlimDX.Audio;
using SlimDX.Multimedia;

namespace SlimDX.XAudio2
{
	public class SourceVoice : ComObject
	{
		private float volume = 1f;

		private int id;

		private int bufferId;

		private int channels = 2;

		private float left = 1f;

		private float right = 1f;

		public float Volume
		{
			get
			{
				return this.volume;
			}
			set
			{
				this.volume = value;
				if (this.id != 0 && AudioHost.Api != null)
				{
					AudioHost.Api.SetVolume(this.id, value);
				}
			}
		}

		public VoiceState State
		{
			get
			{
				var voiceState = default(VoiceState);
				if (this.id != 0 && AudioHost.Api != null)
				{
					voiceState.SamplesPlayed = AudioHost.Api.SamplesPlayed(this.id);
				}
				return voiceState;
			}
		}

		public VoiceDetails VoiceDetails
		{
			get
			{
				if (this.bufferId != 0 && AudioHost.Api != null)
				{
					var num = AudioHost.Api.Channels(this.bufferId);
					if (num > 0)
					{
						this.channels = num;
					}
				}
				return new VoiceDetails
				{
					InputChannels = this.channels
				};
			}
		}

		public FilterParameters FilterParameters
		{
			get
			{
				return this.filter;
			}
			set
			{
				this.filter = value;
				if (this.id != 0 && AudioHost.Api != null)
				{
					AudioHost.Api.SetFilter(this.id, this.filter.Frequency > 0f, this.filter.Frequency);
				}
			}
		}

		private FilterParameters filter;

		public SourceVoice(XAudio2 device, WaveFormat format)
		{
			if (format != null && format.Channels > 0)
			{
				this.channels = format.Channels;
			}
			if (AudioHost.Api != null)
			{
				this.id = AudioHost.Api.CreateVoice();
				AudioHost.Api.SetVolume(this.id, this.volume);
			}
		}

		public SourceVoice(XAudio2 device, WaveFormat format, VoiceFlags flags)
			: this(device, format)
		{
		}

		public void Bind(string path)
		{
			path = ResolveAudioPath(path);
			this.ReadWavInfo(path);
			if (AudioHost.Api == null)
			{
				return;
			}
			this.bufferId = AudioHost.Api.Prepare(path ?? "");
		}

		/// <summary>
		/// Game code still requests .wav paths from the original assembly.
		/// Packed browser assets ship as .ogg; prefer that when present.
		/// </summary>
		internal static string ResolveAudioPath(string path)
		{
			if (string.IsNullOrEmpty(path))
			{
				return path;
			}
			if (path.EndsWith(".wav", StringComparison.OrdinalIgnoreCase))
			{
				var ogg = path.Substring(0, path.Length - 4) + ".ogg";
				if (File.Exists(ogg) || !File.Exists(path))
				{
					return ogg;
				}
			}
			return path;
		}

		public void Start(PlayFlags flags)
		{
			if (this.id != 0 && AudioHost.Api != null)
			{
				AudioHost.Api.Start(this.id);
			}
		}

		public void Stop()
		{
			if (this.id != 0 && AudioHost.Api != null)
			{
				AudioHost.Api.Stop(this.id);
			}
		}

		public void FlushSourceBuffers()
		{
		}

		public void SubmitSourceBuffer(AudioBuffer buffer)
		{
			this.Submit(buffer);
		}

		public void SubmitSourceBuffer(AudioBuffer buffer, int[] decodedPacketCumulativeBytes)
		{
			this.Submit(buffer);
		}

		public void SetOutputMatrix(int inputChannels, int outputChannels, float[] matrix)
		{
			var num = 1f;
			var num2 = 1f;
			if (matrix != null && matrix.Length >= 4)
			{
				num = matrix[0] + matrix[2];
				num2 = matrix[1] + matrix[3];
			}
			else if (matrix != null && matrix.Length == 3)
			{
				num = matrix[1];
				num2 = matrix[2];
			}
			else if (matrix != null && matrix.Length == 2)
			{
				num = matrix[0];
				num2 = matrix[1];
			}
			else if (matrix != null && matrix.Length == 1)
			{
				num = matrix[0];
				num2 = matrix[0];
			}
			this.left = num;
			this.right = num2;
			if (this.id != 0 && AudioHost.Api != null)
			{
				AudioHost.Api.SetPan(this.id, num, num2);
			}
		}

		protected override void Dispose(bool disposing)
		{
			if (this.id != 0 && AudioHost.Api != null)
			{
				AudioHost.Api.DestroyVoice(this.id);
				this.id = 0;
			}
		}

		private void Submit(AudioBuffer buffer)
		{
			if (buffer == null || this.id == 0 || this.bufferId == 0 || AudioHost.Api == null)
			{
				return;
			}
			AudioHost.Api.Submit(this.id, this.bufferId, buffer.PlayBegin, buffer.PlayLength, buffer.LoopCount, buffer.LoopBegin, buffer.LoopLength);
		}

		private void ReadWavInfo(string path)
		{
			try
			{
				if (string.IsNullOrEmpty(path) || !File.Exists(path))
				{
					return;
				}
				using (var fileStream = File.OpenRead(path))
				{
					var array = new byte[44];
					if (fileStream.Read(array, 0, array.Length) < 44)
					{
						return;
					}
					int num = BitConverter.ToInt16(array, 22);
					if (num >= 1 && num <= 8)
					{
						this.channels = num;
					}
				}
			}
			catch
			{
			}
		}
	}
}
