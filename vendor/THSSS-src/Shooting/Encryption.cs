using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;

namespace Shooting;

internal class Encryption
{
	public static string Encrypt(string pToEncrypt)
	{
		string s = "SeiweLL0";
		string s2 = "0SeiweLL";
		DESCryptoServiceProvider dESCryptoServiceProvider = new DESCryptoServiceProvider();
		byte[] bytes = Encoding.Default.GetBytes(pToEncrypt);
		dESCryptoServiceProvider.Key = Encoding.ASCII.GetBytes(s);
		dESCryptoServiceProvider.IV = Encoding.ASCII.GetBytes(s2);
		MemoryStream memoryStream = new MemoryStream();
		CryptoStream cryptoStream = new CryptoStream(memoryStream, dESCryptoServiceProvider.CreateEncryptor(), CryptoStreamMode.Write);
		cryptoStream.Write(bytes, 0, bytes.Length);
		cryptoStream.FlushFinalBlock();
		StringBuilder stringBuilder = new StringBuilder();
		byte[] array = memoryStream.ToArray();
		foreach (byte b in array)
		{
			stringBuilder.AppendFormat("{0:X2}", b);
		}
		return stringBuilder.ToString();
	}

	public static string Decrypt(string pToDecrypt)
	{
		string s = "SeiweLL0";
		string s2 = "0SeiweLL";
		DESCryptoServiceProvider dESCryptoServiceProvider = new DESCryptoServiceProvider();
		byte[] array = new byte[pToDecrypt.Length / 2];
		for (int i = 0; i < pToDecrypt.Length / 2; i++)
		{
			int num = Convert.ToInt32(pToDecrypt.Substring(i * 2, 2), 16);
			array[i] = (byte)num;
		}
		dESCryptoServiceProvider.Key = Encoding.ASCII.GetBytes(s);
		dESCryptoServiceProvider.IV = Encoding.ASCII.GetBytes(s2);
		MemoryStream memoryStream = new MemoryStream();
		CryptoStream cryptoStream = new CryptoStream(memoryStream, dESCryptoServiceProvider.CreateDecryptor(), CryptoStreamMode.Write);
		cryptoStream.Write(array, 0, array.Length);
		cryptoStream.FlushFinalBlock();
		new StringBuilder();
		return Encoding.Default.GetString(memoryStream.ToArray());
	}
}
