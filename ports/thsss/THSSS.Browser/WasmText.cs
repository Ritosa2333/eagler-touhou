using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Thsss.Browser
{
	internal static unsafe class WasmText
	{
		public static byte* Alloc(string text)
		{
			if (text == null)
			{
				text = "";
			}
			var count = Encoding.UTF8.GetByteCount(text);
			var buffer = (byte*)NativeMemory.Alloc((nuint)(count + 1));
			fixed (char* chars = text)
			{
				Encoding.UTF8.GetBytes(chars, text.Length, buffer, count);
			}
			buffer[count] = 0;
			return buffer;
		}

		public static void Free(byte* buffer)
		{
			NativeMemory.Free(buffer);
		}

		public static string Read(byte* text)
		{
			if (text == null)
			{
				return "";
			}
			return Encoding.UTF8.GetString(MemoryMarshal.CreateReadOnlySpanFromNullTerminated(text));
		}

		public static string CopyFromJs(delegate*<byte*, int, void> copy, delegate*<int> length)
		{
			var size = length();
			if (size <= 1)
			{
				return "";
			}
			var buffer = (byte*)NativeMemory.Alloc((nuint)size);
			try
			{
				copy(buffer, size);
				return Encoding.UTF8.GetString(MemoryMarshal.CreateReadOnlySpanFromNullTerminated(buffer));
			}
			finally
			{
				NativeMemory.Free(buffer);
			}
		}

		public static byte[] CopyBytes(delegate*<byte*, int, void> copy, int length)
		{
			if (length <= 0)
			{
				return Array.Empty<byte>();
			}
			var bytes = new byte[length];
			fixed (byte* buffer = bytes)
			{
				copy(buffer, length);
			}
			return bytes;
		}
	}
}
