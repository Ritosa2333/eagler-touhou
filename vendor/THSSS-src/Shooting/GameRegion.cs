using System.Drawing;
using System.Windows.Forms;

namespace Shooting;

internal class GameRegion : UserControl
{
	private void InitializeComponent()
	{
		((Control)this).SuspendLayout();
		((ContainerControl)this).AutoScaleMode = (AutoScaleMode)0;
		((Control)this).Name = "GameRegion";
		((Control)this).Size = new Size(229, 148);
		((Control)this).ResumeLayout(false);
	}
}
