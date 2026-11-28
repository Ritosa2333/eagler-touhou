using System.CodeDom.Compiler;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.Resources;
using System.Runtime.CompilerServices;

namespace Shooting.Properties;

[GeneratedCode("System.Resources.Tools.StronglyTypedResourceBuilder", "17.0.0.0")]
[DebuggerNonUserCode]
[CompilerGenerated]
internal class Resources
{
	private static ResourceManager resourceMan;

	private static CultureInfo resourceCulture;

	[EditorBrowsable(EditorBrowsableState.Advanced)]
	internal static ResourceManager ResourceManager
	{
		get
		{
			if (resourceMan == null)
			{
				resourceMan = new ResourceManager("Shooting.Properties.Resources", typeof(Resources).Assembly);
			}
			return resourceMan;
		}
	}

	[EditorBrowsable(EditorBrowsableState.Advanced)]
	internal static CultureInfo Culture
	{
		get
		{
			return resourceCulture;
		}
		set
		{
			resourceCulture = value;
		}
	}

	internal static Bitmap background
	{
		get
		{
			//IL_0014: Unknown result type (might be due to invalid IL or missing references)
			//IL_001a: Expected Obj, but got Unknown
			return (Bitmap)ResourceManager.GetObject("background", resourceCulture);
		}
	}

	internal static Bitmap FiveStar
	{
		get
		{
			//IL_0014: Unknown result type (might be due to invalid IL or missing references)
			//IL_001a: Expected Obj, but got Unknown
			return (Bitmap)ResourceManager.GetObject("FiveStar", resourceCulture);
		}
	}

	internal static Bitmap Menu_Start
	{
		get
		{
			//IL_0014: Unknown result type (might be due to invalid IL or missing references)
			//IL_001a: Expected Obj, but got Unknown
			return (Bitmap)ResourceManager.GetObject("Menu_Start", resourceCulture);
		}
	}

	internal static Bitmap MyPlane
	{
		get
		{
			//IL_0014: Unknown result type (might be due to invalid IL or missing references)
			//IL_001a: Expected Obj, but got Unknown
			return (Bitmap)ResourceManager.GetObject("MyPlane", resourceCulture);
		}
	}

	internal static Bitmap Player
	{
		get
		{
			//IL_0014: Unknown result type (might be due to invalid IL or missing references)
			//IL_001a: Expected Obj, but got Unknown
			return (Bitmap)ResourceManager.GetObject("Player", resourceCulture);
		}
	}

	internal static Bitmap Power
	{
		get
		{
			//IL_0014: Unknown result type (might be due to invalid IL or missing references)
			//IL_001a: Expected Obj, but got Unknown
			return (Bitmap)ResourceManager.GetObject("Power", resourceCulture);
		}
	}

	internal static Bitmap Score
	{
		get
		{
			//IL_0014: Unknown result type (might be due to invalid IL or missing references)
			//IL_001a: Expected Obj, but got Unknown
			return (Bitmap)ResourceManager.GetObject("Score", resourceCulture);
		}
	}

	internal static Bitmap Spell
	{
		get
		{
			//IL_0014: Unknown result type (might be due to invalid IL or missing references)
			//IL_001a: Expected Obj, but got Unknown
			return (Bitmap)ResourceManager.GetObject("Spell", resourceCulture);
		}
	}

	internal static Bitmap 菜单背景
	{
		get
		{
			//IL_0014: Unknown result type (might be due to invalid IL or missing references)
			//IL_001a: Expected Obj, but got Unknown
			return (Bitmap)ResourceManager.GetObject("菜单背景", resourceCulture);
		}
	}

	internal Resources()
	{
	}
}
