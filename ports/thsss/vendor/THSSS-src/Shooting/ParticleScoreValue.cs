using System;
using System.Drawing;

namespace Shooting;

internal class ParticleScoreValue : BaseEffect
{
	private char[] ScoreCharArray { get; set; }

	public ParticleScoreValue(StageDataPackage StageData, PointF OriginalPosition, long ScoreValue)
		: base(StageData, null, new PointF(0f, 0f), 1f, -1.5707963705062866)
	{
		LifeTime = 50;
		Active = false;
		base.OriginalPosition = OriginalPosition;
		ScoreCharArray = ScoreValue.ToString().ToCharArray();
	}

	public override void Ctrl()
	{
		Angle = 0.0 - Direction + Math.PI / 2.0;
		base.Ctrl();
		if (Time > 0)
		{
			TransparentValueF -= 255 / LifeTime;
		}
	}

	public override void Show()
	{
		int num = TextureObjectDictionary["Score_+"].Width;
		for (int i = 0; i < ScoreCharArray.Length; i++)
		{
			num += TextureObjectDictionary[ScoreCharArray[i].ToString()].Width;
		}
		float num2 = 0.6f;
		PointF position = new PointF(OriginalPosition.X - (float)num * num2 / 2f, OriginalPosition.Y);
		SpriteMain.Draw2D(TextureObjectDictionary["Score_+"], num2, 0f, position, Color.FromArgb(TransparentValue, ColorValue));
		position = new PointF(position.X + (float)TextureObjectDictionary["Score_+"].Width * num2 / 2f, position.Y);
		for (int j = 0; j < ScoreCharArray.Length; j++)
		{
			position = new PointF(position.X + (float)TextureObjectDictionary["Score_" + ScoreCharArray[j]].Width * num2 / 2f, position.Y);
			SpriteMain.Draw2D(TextureObjectDictionary["Score_" + ScoreCharArray[j]], num2, 0f, position, Color.FromArgb(TransparentValue, ColorValue));
			position = new PointF(position.X + (float)TextureObjectDictionary["Score_" + ScoreCharArray[j]].Width * num2 / 2f, position.Y);
		}
	}
}
