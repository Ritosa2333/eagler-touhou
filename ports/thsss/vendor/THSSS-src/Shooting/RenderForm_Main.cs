using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using SlimDX.Windows;

namespace Shooting;

public class RenderForm_Main : RenderForm
{
	public Panel panel1;

	private IContainer components;

	public RenderForm_Main()
	{
		//IL_0022: Unknown result type (might be due to invalid IL or missing references)
		//IL_002c: Expected Obj, but got Unknown
		InitializeComponent();
		((Control)this).BackColor = Color.Black;
		if (Game_Main.useUnrealFullScreen)
		{
			panel1 = new Panel();
			((Control)this).Controls.Add((Control)(object)panel1);
			((Control)panel1).Size = new Size(((Form)this).ClientSize.Height * 4 / 3, ((Form)this).ClientSize.Height);
			((Control)panel1).Location = new Point(((Form)this).ClientSize.Width / 2 - ((Control)panel1).Width / 2, 0);
			((Control)panel1).Name = "panel1";
			((Control)panel1).TabIndex = 0;
		}
	}

	private void RenderForm_Main_SizeChanged(object sender, EventArgs e)
	{
		if (panel1 != null)
		{
			if (!Game_Main.stretchScreen)
			{
				((Control)panel1).Size = new Size(((Form)this).ClientSize.Height * 4 / 3, ((Form)this).ClientSize.Height);
				((Control)panel1).Location = new Point(((Form)this).ClientSize.Width / 2 - ((Control)panel1).Width / 2, 0);
			}
			else
			{
				((Control)panel1).Size = new Size(((Form)this).ClientSize.Width, ((Form)this).ClientSize.Height);
				((Control)panel1).Location = new Point(0, 0);
			}
		}
	}

	protected override void Dispose(bool disposing)
	{
		if (disposing && components != null)
		{
			components.Dispose();
		}
		((Form)this).Dispose(disposing);
	}

	private void InitializeComponent()
	{
		//IL_0057: Unknown result type (might be due to invalid IL or missing references)
		//IL_0061: Expected Obj, but got Unknown
		ComponentResourceManager componentResourceManager = new ComponentResourceManager(typeof(RenderForm_Main));
		((Control)this).SuspendLayout();
		((ContainerControl)this).AutoScaleMode = (AutoScaleMode)0;
		((Control)this).BackColor = Color.Black;
		((Form)this).ClientSize = new Size(640, 480);
		((Control)this).DoubleBuffered = true;
		((Form)this).FormBorderStyle = (FormBorderStyle)3;
		((Form)this).Icon = (Icon)componentResourceManager.GetObject("$this.Icon");
		((Form)this).MaximizeBox = false;
		((Control)this).ImeMode = (ImeMode)2;
		((Control)this).Name = "RenderForm_Main";
		((Form)this).StartPosition = (FormStartPosition)1;
		((Control)this).Text = "东方夏夜祭 ～ Shining Shooting Star. ver 1.01";
		((Control)this).SizeChanged += RenderForm_Main_SizeChanged;
		((Control)this).ResumeLayout(false);
	}
}
