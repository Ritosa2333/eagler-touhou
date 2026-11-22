using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;

namespace Config
{
	public partial class Form_Config : Form
	{
		public Form_Config()
		{
			this.InitializeComponent();
			base.Opacity = 0.0;
			this.LoadingTimer.Interval = 5;
			this.LoadingTimer.Tick += this.LoadingTimer_Tick;
			this.LoadingTimer.Enabled = true;
			try
			{
				INI_RW ini_RW = new INI_RW(".\\Setting.INI");
				if (ini_RW.ExistINIFile())
				{
					this.label_left.Text = ini_RW.IniReadValue("Key", "Left");
					this.label_right.Text = ini_RW.IniReadValue("Key", "Right");
					this.label_up.Text = ini_RW.IniReadValue("Key", "Up");
					this.label_down.Text = ini_RW.IniReadValue("Key", "Down");
					this.label_slow.Text = ini_RW.IniReadValue("Key", "Slow");
					this.label_shoot.Text = ini_RW.IniReadValue("Key", "Shoot");
					this.label_bomb.Text = ini_RW.IniReadValue("Key", "Bomb");
					this.checkBox_VerticalSync.Checked = ini_RW.IniReadValue("DirectX", "VerticalSync") == "1";
					this.checkBox_fullWindow.Checked = ini_RW.IniReadValue("DirectX", "FullWindow") == "1";
					this.radioButton_Circle1.Checked = ini_RW.IniReadValue("Mode", "PresentInterval") == "1";
					this.radioButton_Circle2.Checked = ini_RW.IniReadValue("Mode", "PresentInterval") == "2";
					this.radioButton_Circle3.Checked = ini_RW.IniReadValue("Mode", "PresentInterval") == "3";
					this.checkBox_useDirectInput.Checked = ini_RW.IniReadValue("DirectInput", "UseDirectInput") == "1";
					this.comboBox_Skip.SelectedIndex = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Skip"));
					this.comboBox_Pause.SelectedIndex = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Pause"));
					this.comboBox_Slow.SelectedIndex = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Slow"));
					this.comboBox_Shoot.SelectedIndex = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Shoot"));
					this.comboBox_Bomb.SelectedIndex = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Bomb"));
					this.comboBox_Font.SelectedItem = ini_RW.IniReadValue("Game", "FontType");
				}
				this.checkBox_useDirectInput_CheckedChanged(this, null);
			}
			catch
			{
				MessageBox.Show("读取配置文件失败，悲剧啊！", "Setting Load Error");
			}
		}

		private void LoadingTimer_Tick(object sender, EventArgs e)
		{
			if (this.Loading == 0)
			{
				base.Location = new Point(base.Location.X, base.Location.Y - 100);
				this.Loading++;
				return;
			}
			if (this.Loading < 50)
			{
				base.Opacity += 0.02;
				base.Location = new Point(base.Location.X, base.Location.Y + 1);
				this.Loading++;
				return;
			}
			this.LoadingTimer.Enabled = false;
		}

		private void Form_Config_KeyUp(object sender, KeyEventArgs e)
		{
			if (this.tmpLabel != null)
			{
				this.tmpLabel.Text = e.KeyCode.ToString();
				this.tmpLabel.BackColor = this.tmpColor;
				this.tmpLabel = null;
			}
		}

		private void label_Click(object sender, EventArgs e)
		{
			if (this.tmpLabel != null)
			{
				return;
			}
			this.tmpLabel = (Label)sender;
			this.tmpColor = this.tmpLabel.BackColor;
			this.tmpLabel.BackColor = Color.White;
		}

		private void button_应用_Click(object sender, EventArgs e)
		{
			try
			{
				INI_RW ini_RW = new INI_RW(".\\Setting.INI");
				if (ini_RW.ExistINIFile())
				{
					ini_RW.IniWriteValue("Key", "Left", this.label_left.Text);
					ini_RW.IniWriteValue("Key", "Right", this.label_right.Text);
					ini_RW.IniWriteValue("Key", "Up", this.label_up.Text);
					ini_RW.IniWriteValue("Key", "Down", this.label_down.Text);
					ini_RW.IniWriteValue("Key", "Slow", this.label_slow.Text);
					ini_RW.IniWriteValue("Key", "Shoot", this.label_shoot.Text);
					ini_RW.IniWriteValue("Key", "Bomb", this.label_bomb.Text);
					ini_RW.IniWriteValue("DirectX", "VerticalSync", this.checkBox_VerticalSync.Checked ? "1" : "0");
					ini_RW.IniWriteValue("DirectX", "FullWindow", this.checkBox_fullWindow.Checked ? "1" : "0");
					if (this.radioButton_Circle2.Checked)
					{
						ini_RW.IniWriteValue("Mode", "PresentInterval", "2");
					}
					else if (this.radioButton_Circle3.Checked)
					{
						ini_RW.IniWriteValue("Mode", "PresentInterval", "3");
					}
					else
					{
						ini_RW.IniWriteValue("Mode", "PresentInterval", "1");
					}
					ini_RW.IniWriteValue("DirectInput", "UseDirectInput", this.checkBox_useDirectInput.Checked ? "1" : "0");
					ini_RW.IniWriteValue("JoyStick", "Skip", this.comboBox_Skip.SelectedIndex.ToString());
					ini_RW.IniWriteValue("JoyStick", "Pause", this.comboBox_Pause.SelectedIndex.ToString());
					ini_RW.IniWriteValue("JoyStick", "Slow", this.comboBox_Slow.SelectedIndex.ToString());
					ini_RW.IniWriteValue("JoyStick", "Shoot", this.comboBox_Shoot.SelectedIndex.ToString());
					ini_RW.IniWriteValue("JoyStick", "Bomb", this.comboBox_Bomb.SelectedIndex.ToString());
					ini_RW.IniWriteValue("Game", "FontType", this.comboBox_Font.SelectedItem.ToString());
				}
			}
			catch
			{
				MessageBox.Show("读取配置文件失败，悲剧啊！", "Setting Load Error");
			}
		}

		private void button_应用并启动_Click(object sender, EventArgs e)
		{
			this.button_应用_Click(this, e);
			Process.Start("THSSS.exe");
			Application.Exit();
		}

		private void checkBox_useDirectInput_CheckedChanged(object sender, EventArgs e)
		{
			if (this.checkBox_useDirectInput.Checked)
			{
				this.label8.Visible = true;
				this.label9.Visible = true;
				this.label10.Visible = true;
				this.label11.Visible = true;
				this.label12.Visible = true;
				this.label13.Visible = true;
				this.label14.Visible = true;
				this.groupBox5.Enabled = true;
				this.label_left.Enabled = false;
				this.label_right.Enabled = false;
				this.label_up.Enabled = false;
				this.label_down.Enabled = false;
				this.label_slow.Enabled = false;
				this.label_shoot.Enabled = false;
				this.label_bomb.Enabled = false;
				return;
			}
			this.label8.Visible = false;
			this.label9.Visible = false;
			this.label10.Visible = false;
			this.label11.Visible = false;
			this.label12.Visible = false;
			this.label13.Visible = false;
			this.label14.Visible = false;
			this.groupBox5.Enabled = false;
			this.label_left.Enabled = true;
			this.label_right.Enabled = true;
			this.label_up.Enabled = true;
			this.label_down.Enabled = true;
			this.label_slow.Enabled = true;
			this.label_shoot.Enabled = true;
			this.label_bomb.Enabled = true;
		}

		private void comboBox_Font_SelectedIndexChanged(object sender, EventArgs e)
		{
			this.label_Font.Font = new Font(this.comboBox_Font.SelectedItem.ToString(), 12f);
		}

		private Label tmpLabel;

		private Color tmpColor;

		private Timer LoadingTimer = new Timer();

		private int Loading;
	}
}
