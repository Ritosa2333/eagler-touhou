using System;
using System.IO;
using System.Text;
using SlimDX.Multimedia;

namespace Shooting;

internal class xWMAStream : Stream
{
	protected Stream dataStream;

	protected long dataOffset;

	protected int dataSize;

	protected int[] decodedPacketCumulativeBytes;

	protected WaveFormat format;

	public const ushort WAVE_FORMAT_xWMA = 353;

	public const ushort WAVE_FORMAT_EXTENSIBLE = 65534;

	public int[] DecodedPacketCumulativeBytes => decodedPacketCumulativeBytes;

	public WaveFormat Format => format;

	public override bool CanRead => true;

	public override bool CanWrite => false;

	public override bool CanSeek => true;

	public override long Position
	{
		get
		{
			return dataStream.Position - dataOffset;
		}
		set
		{
			Seek(value, SeekOrigin.Begin);
		}
	}

	public override long Length => dataSize;

	public xWMAStream(Stream stream)
	{
		dataStream = stream;
		analyzeXWMAFileStream();
	}

	public xWMAStream(string path)
	{
		if (!File.Exists(path))
		{
			throw new FileNotFoundException(path + " Not Found");
		}
		dataStream = new FileStream(path, FileMode.Open);
		analyzeXWMAFileStream();
	}

	private void analyzeXWMAFileStream()
	{
		//IL_0131: Unknown result type (might be due to invalid IL or missing references)
		//IL_013b: Expected Obj, but got Unknown
		//IL_021d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0224: Expected Obj, but got Unknown
		byte[] array = new byte[4];
		dataStream.Seek(0L, SeekOrigin.Begin);
		dataStream.Read(array, 0, 4);
		if (Encoding.ASCII.GetString(array) != "RIFF")
		{
			throw new InvalidDataException("Invalid xWMA file.");
		}
		dataStream.Read(array, 0, 4);
		int num = BitConverter.ToInt32(array, 0);
		if (dataStream.Length != num + 8)
		{
			throw new InvalidDataException("Invalid Format");
		}
		dataStream.Read(array, 0, 4);
		if (Encoding.ASCII.GetString(array) != "XWMA")
		{
			throw new InvalidDataException("Invalid xWMA file.");
		}
		long num2 = 0L;
		for (num2 = dataStream.Read(array, 0, 4); num2 > 3; num2 = dataStream.Read(array, 0, 4))
		{
			string text = Encoding.ASCII.GetString(array);
			dataStream.Read(array, 0, 4);
			num = BitConverter.ToInt32(array, 0);
			switch (text)
			{
			case "fmt ":
			{
				dataStream.Read(array, 0, 2);
				ushort num4 = BitConverter.ToUInt16(array, 0);
				format = new WaveFormat();
				dataStream.Read(array, 0, 2);
				format.Channels = (short)BitConverter.ToUInt16(array, 0);
				dataStream.Read(array, 0, 4);
				format.SamplesPerSecond = BitConverter.ToInt32(array, 0);
				dataStream.Read(array, 0, 4);
				format.AverageBytesPerSecond = BitConverter.ToInt32(array, 0);
				dataStream.Read(array, 0, 2);
				format.BlockAlignment = BitConverter.ToInt16(array, 0);
				dataStream.Read(array, 0, 2);
				format.BitsPerSample = BitConverter.ToInt16(array, 0);
				switch (num4)
				{
				case 353:
					dataStream.Seek(num - 16, SeekOrigin.Current);
					format.FormatTag = (WaveFormatTag)353;
					break;
				case 65534:
				{
					WaveFormatExtensible val = new WaveFormatExtensible();
					((WaveFormat)val).Channels = format.Channels;
					((WaveFormat)val).SamplesPerSecond = format.SamplesPerSecond;
					((WaveFormat)val).AverageBytesPerSecond = format.AverageBytesPerSecond;
					((WaveFormat)val).BlockAlignment = format.BlockAlignment;
					((WaveFormat)val).BitsPerSample = format.BitsPerSample;
					dataStream.Read(array, 0, 2);
					num = BitConverter.ToInt16(array, 0);
					if (num != 22)
					{
						throw new InvalidDataException("Invalid Format.");
					}
					dataStream.Read(array, 0, 2);
					val.ValidBitsPerSample = BitConverter.ToInt16(array, 0);
					dataStream.Read(array, 0, 4);
					val.ChannelMask = (Speakers)BitConverter.ToUInt32(array, 0);
					byte[] array2 = new byte[16];
					dataStream.Read(array2, 0, 16);
					val.SubFormat = new Guid(array2);
					((WaveFormat)val).FormatTag = (WaveFormatTag)65534;
					format = (WaveFormat)(object)val;
					break;
				}
				default:
					throw new InvalidDataException("Invalid xWMA file.");
				}
				break;
			}
			case "dpds":
			{
				if (num % 4 != 0)
				{
					throw new InvalidDataException("Invalid Format.");
				}
				int num3 = num / 4;
				decodedPacketCumulativeBytes = new int[num3];
				for (int i = 0; i < num3; i++)
				{
					dataStream.Read(array, 0, 4);
					decodedPacketCumulativeBytes[i] = BitConverter.ToInt32(array, 0);
				}
				break;
			}
			case "data":
				dataOffset = dataStream.Position;
				dataSize = num;
				dataStream.Seek(num, SeekOrigin.Current);
				break;
			default:
				dataStream.Seek(num, SeekOrigin.Current);
				break;
			}
		}
		dataStream.Seek(dataOffset, SeekOrigin.Begin);
	}

	public override int Read(byte[] buffer, int offset, int count)
	{
		return dataStream.Read(buffer, offset, count);
	}

	public override void Write(byte[] buffer, int offset, int count)
	{
		throw new NotSupportedException("xWMA Stream objects cannot be written to.");
	}

	public override void SetLength(long value)
	{
		throw new NotSupportedException("xWMA Stream objects cannot be resized.");
	}

	public override long Seek(long offset, SeekOrigin origin)
	{
		long offset2 = 0L;
		switch (origin)
		{
		case SeekOrigin.Begin:
			offset2 = dataOffset + offset;
			break;
		case SeekOrigin.Current:
			offset2 = offset;
			break;
		case SeekOrigin.End:
			offset2 = dataOffset;
			break;
		}
		return dataStream.Seek(offset2, origin) - dataOffset;
	}

	public override void Flush()
	{
		throw new NotSupportedException("xWMA Stream objects cannot be flushed.");
	}

	public override int ReadByte()
	{
		if (dataStream.Position > dataOffset + dataSize)
		{
			return -1;
		}
		return dataStream.ReadByte();
	}

	public override void WriteByte(byte value)
	{
		throw new NotSupportedException("xWMA Stream objects cannot be written to.");
	}

	public override void Close()
	{
		dataStream.Close();
	}

	protected override void Dispose(bool disposing)
	{
		dataStream.Dispose();
	}
}
