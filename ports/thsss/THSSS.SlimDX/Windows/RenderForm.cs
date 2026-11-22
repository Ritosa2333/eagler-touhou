using System.Windows.Forms;

namespace SlimDX.Windows
{
	public class RenderForm : Form
	{
		public RenderForm()
		{
			this.ClientSize = new System.Drawing.Size(640, 480);
		}

		public RenderForm(string text)
			: this()
		{
			this.Text = text;
		}
	}
}
