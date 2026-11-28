using System.Collections.Generic;

namespace SlimDX.DirectInput
{
	public static class KeyboardStateMap
	{
		public static readonly HashSet<Key> Down = new HashSet<Key>();
	}

	public class KeyboardState
	{
		public bool IsPressed(Key key)
		{
			return KeyboardStateMap.Down.Contains(key);
		}

		public bool IsReleased(Key key)
		{
			return !this.IsPressed(key);
		}
	}
}
