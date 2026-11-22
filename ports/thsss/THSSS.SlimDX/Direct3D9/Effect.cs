using System.Collections.Generic;
using System.IO;

namespace SlimDX.Direct3D9
{
	public class Effect : ComObject
	{
		public static Effect Active { get; private set; }

		private readonly Dictionary<string, float> floats = new Dictionary<string, float>();

		private readonly Dictionary<string, int> ints = new Dictionary<string, int>();

		public string Name { get; private set; }

		public string Technique { get; set; }

		public static Effect FromFile(Device device, string fileName, ShaderFlags flags)
		{
			var path = (fileName ?? "").Replace('\\', '/');
			if (!File.Exists(path))
			{
				throw new FileNotFoundException("Effect file not found.", fileName);
			}
			using (File.OpenRead(path))
			{
			}
			return new Effect
			{
				Name = Path.GetFileNameWithoutExtension(path)
			};
		}

		public int Begin()
		{
			return 1;
		}

		public void BeginPass(int pass)
		{
			if (Sprite.Current != null)
			{
				Sprite.Current.FlushPending();
			}
			Active = this;
		}

		public void EndPass()
		{
			if (Sprite.Current != null)
			{
				Sprite.Current.FlushPending();
			}
			if (Active == this)
			{
				Active = null;
			}
		}

		public void End()
		{
			if (Active == this && Sprite.Current != null)
			{
				Sprite.Current.FlushPending();
			}
			if (Active == this)
			{
				Active = null;
			}
		}

		public void SetValue<T>(string name, T value) where T : struct
		{
			if (value is float number)
			{
				this.floats[name] = number;
			}
			else if (value is int integer)
			{
				this.ints[name] = integer;
			}
		}

		public float Float(string name)
		{
			float value;
			return this.floats.TryGetValue(name, out value) ? value : 0f;
		}

		public int Int(string name)
		{
			int value;
			return this.ints.TryGetValue(name, out value) ? value : 0;
		}

		public void OnLostDevice()
		{
		}

		public void OnResetDevice()
		{
		}
	}
}
