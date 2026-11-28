namespace Shooting.Planes.Story;

internal class Conversation
{
	public string Text;

	public bool CharLAcitve;

	public bool CharRAcitve;

	public string CharL;

	public string CharR;

	public string CharL2;

	public bool CharL2Acitve;

	public int StringCount;

	public Conversation(string Text, bool CharLAcitve, bool CharRAcitve, string CharL, string CharR)
		: this(Text, CharLAcitve, CharRAcitve, CharL, CharR, 255)
	{
	}

	public Conversation(string Text, bool CharLAcitve, string CharL, bool Char2LAcitve, string CharL2, bool CharRAcitve, string CharR)
		: this(Text, CharLAcitve, CharRAcitve, CharL, CharR, 255)
	{
		this.CharL2 = CharL2;
		CharL2Acitve = Char2LAcitve;
	}

	public Conversation(string Text, bool CharLAcitve, bool CharRAcitve, string CharL, string CharR, int StringCount)
	{
		this.StringCount = StringCount;
		if (Text.Length > StringCount)
		{
			Text = Text.Insert(StringCount, "\r\n");
		}
		if (Text.Length > StringCount * 2)
		{
			Text = Text.Insert(StringCount * 2, "\r\n");
		}
		if (Text.Length > StringCount * 3)
		{
			Text = Text.Insert(StringCount * 3, "\r\n");
		}
		this.Text = Text;
		this.CharLAcitve = CharLAcitve;
		this.CharRAcitve = CharRAcitve;
		this.CharL = CharL;
		this.CharR = CharR;
	}
}
