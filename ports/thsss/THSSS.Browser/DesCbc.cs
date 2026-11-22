using System;
using System.Text;

namespace Shooting
{
	public static class DesCbc
	{
		public static string EncryptToHex(string text, string keyText, string ivText)
		{
			var plain = Ansi.GBK.GetBytes(text ?? "");
			var cipher = Transform(plain, Encoding.ASCII.GetBytes(keyText), Encoding.ASCII.GetBytes(ivText), true);
			var hexChars = new char[cipher.Length * 2];
			for (var i = 0; i < cipher.Length; i++)
			{
				hexChars[i * 2] = Hex[cipher[i] >> 4];
				hexChars[i * 2 + 1] = Hex[cipher[i] & 15];
			}
			return new string(hexChars);
		}

		public static string DecryptFromHex(string hex, string keyText, string ivText)
		{
			var cipher = new byte[hex.Length / 2];
			for (var i = 0; i < cipher.Length; i++)
			{
				cipher[i] = (byte)((Nibble(hex[i * 2]) << 4) | Nibble(hex[i * 2 + 1]));
			}
			var plain = Transform(cipher, Encoding.ASCII.GetBytes(keyText), Encoding.ASCII.GetBytes(ivText), false);
			return Ansi.GBK.GetString(plain);
		}

		private static int Nibble(char c)
		{
			if (c >= '0' && c <= '9')
			{
				return c - '0';
			}
			if (c >= 'A' && c <= 'F')
			{
				return c - 'A' + 10;
			}
			if (c >= 'a' && c <= 'f')
			{
				return c - 'a' + 10;
			}
			return 0;
		}

		private static byte[] Transform(byte[] input, byte[] key, byte[] iv, bool encrypt)
		{
			var subkeys = BuildSubkeys(key);
			if (encrypt)
			{
				var pad = 8 - (input.Length % 8);
				var padded = new byte[input.Length + pad];
				Buffer.BlockCopy(input, 0, padded, 0, input.Length);
				for (var i = input.Length; i < padded.Length; i++)
				{
					padded[i] = (byte)pad;
				}
				var output = new byte[padded.Length];
				var previous = Read64(iv);
				for (var offset = 0; offset < padded.Length; offset += 8)
				{
					var enc = CryptBlock(Read64At(padded, offset) ^ previous, subkeys, true);
					Write64At(output, offset, enc);
					previous = enc;
				}
				return output;
			}
			var plain = new byte[input.Length];
			var prev = Read64(iv);
			for (var offset = 0; offset < input.Length; offset += 8)
			{
				var block = Read64At(input, offset);
				var dec = CryptBlock(block, subkeys, false) ^ prev;
				Write64At(plain, offset, dec);
				prev = block;
			}
			int n = plain[plain.Length - 1];
			if (n < 1 || n > 8)
			{
				return plain;
			}
			var trimmed = new byte[plain.Length - n];
			Buffer.BlockCopy(plain, 0, trimmed, 0, trimmed.Length);
			return trimmed;
		}

		private static ulong[] BuildSubkeys(byte[] key)
		{
			var k = Read64(key);
			var cd = Permute(k, MapPc1, 64);
			var sub = new ulong[16];
			for (var round = 0; round < 16; round++)
			{
				cd = Rotate28(cd, Shifts[round]);
				sub[round] = Permute(cd, MapPc2, 56);
			}
			return sub;
		}

		private static ulong CryptBlock(ulong block, ulong[] subkeys, bool encrypt)
		{
			var ip = Permute(block, MapIp, 64);
			var left = (uint)(ip >> 32);
			var right = (uint)(ip & 0xffffffffu);
			for (var round = 0; round < 16; round++)
			{
				var next = left ^ F(right, subkeys[encrypt ? round : 15 - round]);
				left = right;
				right = next;
			}
			var pre = ((ulong)right << 32) | left;
			return Permute(pre, MapFp, 64);
		}

		private static uint F(uint right, ulong subkey)
		{
			var expanded = Permute(right, MapE, 32);
			var mixed = expanded ^ subkey;
			uint s = 0;
			for (var i = 0; i < 8; i++)
			{
				var six = (int)((mixed >> (42 - 6 * i)) & 0x3f);
				var row = ((six & 0x20) >> 4) | (six & 1);
				var col = (six >> 1) & 0x0f;
				s = (s << 4) | (uint)S[i, row * 16 + col];
			}
			return (uint)Permute(s, MapP, 32);
		}

		private static ulong Permute(ulong value, ulong[][] map, int inBits)
		{
			var shift = inBits - 8;
			var result = map[0][(int)((value >> shift) & 255)];
			for (var b = 1; b < map.Length; b++)
			{
				shift -= 8;
				result |= map[b][(int)((value >> shift) & 255)];
			}
			return result;
		}

		private static ulong[][] BuildMap(int[] table, int inBits, int outBits)
		{
			var map = new ulong[inBits / 8][];
			for (var b = 0; b < map.Length; b++)
			{
				map[b] = new ulong[256];
				for (var v = 0; v < 256; v++)
				{
					ulong acc = 0;
					for (var outi = 0; outi < outBits; outi++)
					{
						var src = table[outi] - 1;
						if (src >> 3 != b)
						{
							continue;
						}
						if (((v >> (7 - (src & 7))) & 1) != 0)
						{
							acc |= 1ul << (outBits - 1 - outi);
						}
					}
					map[b][v] = acc;
				}
			}
			return map;
		}

		private static ulong Rotate28(ulong cd, int shift)
		{
			var c = (cd >> 28) & 0x0fffffff;
			var d = cd & 0x0fffffff;
			c = ((c << shift) | (c >> (28 - shift))) & 0x0fffffff;
			d = ((d << shift) | (d >> (28 - shift))) & 0x0fffffff;
			return (c << 28) | d;
		}

		private static ulong Read64(byte[] bytes)
		{
			return Read64At(bytes, 0);
		}

		private static ulong Read64At(byte[] bytes, int offset)
		{
			ulong value = 0;
			for (var i = 0; i < 8; i++)
			{
				value = (value << 8) | bytes[offset + i];
			}
			return value;
		}

		private static void Write64At(byte[] bytes, int offset, ulong value)
		{
			for (var i = 7; i >= 0; i--)
			{
				bytes[offset + i] = (byte)(value & 0xff);
				value >>= 8;
			}
		}

		private static readonly char[] Hex = "0123456789ABCDEF".ToCharArray();

		private static readonly ulong[][] MapIp;

		private static readonly ulong[][] MapFp;

		private static readonly ulong[][] MapE;

		private static readonly ulong[][] MapP;

		private static readonly ulong[][] MapPc1;

		private static readonly ulong[][] MapPc2;

		static DesCbc()
		{
			MapIp = BuildMap(Ip, 64, 64);
			MapFp = BuildMap(Fp, 64, 64);
			MapE = BuildMap(E, 32, 48);
			MapP = BuildMap(P, 32, 32);
			MapPc1 = BuildMap(Pc1, 64, 56);
			MapPc2 = BuildMap(Pc2, 56, 48);
		}

		private static readonly int[] Shifts = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

		private static readonly int[] Ip = {
			58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
			62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
			57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
			61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
		};

		private static readonly int[] Fp = {
			40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
			38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
			36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
			34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25
		};

		private static readonly int[] E = {
			32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11,
			12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21,
			22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
		};

		private static readonly int[] P = {
			16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10,
			2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25
		};

		private static readonly int[] Pc1 = {
			57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
			10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
			63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
			14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
		};

		private static readonly int[] Pc2 = {
			14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
			23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
			41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
			44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
		};

		private static readonly int[,] S = {
			{
				14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
				0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
				4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
				15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
			},
			{
				15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
				3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
				0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
				13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9
			},
			{
				10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
				13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
				13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
				1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12
			},
			{
				7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
				13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
				10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
				3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14
			},
			{
				2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
				14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
				4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
				11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3
			},
			{
				12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
				10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
				9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
				4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13
			},
			{
				4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
				13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
				1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
				6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12
			},
			{
				13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
				1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
				7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
				2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
			}
		};
	}
}
