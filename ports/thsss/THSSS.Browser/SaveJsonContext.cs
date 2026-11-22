using System.Collections.Generic;
using System.Text.Json.Serialization;

namespace Thsss.Browser
{
	[JsonSerializable(typeof(Dictionary<string, string>))]
	internal partial class SaveJsonContext : JsonSerializerContext
	{
	}
}
