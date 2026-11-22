namespace Config
{
	public partial class Form_Config : global::System.Windows.Forms.Form
	{
		protected override void Dispose(bool disposing)
		{
			if (disposing && this.components != null)
			{
				this.components.Dispose();
			}
			base.Dispose(disposing);
		}

		private void InitializeComponent()
		{
			global::System.ComponentModel.ComponentResourceManager componentResourceManager = new global::System.ComponentModel.ComponentResourceManager(typeof(global::Config.Form_Config));
			this.button_应用 = new global::System.Windows.Forms.Button();
			this.checkBox_useDirectInput = new global::System.Windows.Forms.CheckBox();
			this.checkBox_VerticalSync = new global::System.Windows.Forms.CheckBox();
			this.groupBox1 = new global::System.Windows.Forms.GroupBox();
			this.groupBox2 = new global::System.Windows.Forms.GroupBox();
			this.groupBox3 = new global::System.Windows.Forms.GroupBox();
			this.label8 = new global::System.Windows.Forms.Label();
			this.label9 = new global::System.Windows.Forms.Label();
			this.label10 = new global::System.Windows.Forms.Label();
			this.label11 = new global::System.Windows.Forms.Label();
			this.label12 = new global::System.Windows.Forms.Label();
			this.label13 = new global::System.Windows.Forms.Label();
			this.label14 = new global::System.Windows.Forms.Label();
			this.label_bomb = new global::System.Windows.Forms.Label();
			this.label_shoot = new global::System.Windows.Forms.Label();
			this.label_slow = new global::System.Windows.Forms.Label();
			this.label_down = new global::System.Windows.Forms.Label();
			this.label_up = new global::System.Windows.Forms.Label();
			this.label_right = new global::System.Windows.Forms.Label();
			this.label_left = new global::System.Windows.Forms.Label();
			this.label7 = new global::System.Windows.Forms.Label();
			this.label6 = new global::System.Windows.Forms.Label();
			this.label5 = new global::System.Windows.Forms.Label();
			this.label4 = new global::System.Windows.Forms.Label();
			this.label3 = new global::System.Windows.Forms.Label();
			this.label2 = new global::System.Windows.Forms.Label();
			this.label1 = new global::System.Windows.Forms.Label();
			this.button_应用并启动 = new global::System.Windows.Forms.Button();
			this.groupBox4 = new global::System.Windows.Forms.GroupBox();
			this.checkBox_fullWindow = new global::System.Windows.Forms.CheckBox();
			this.groupBox5 = new global::System.Windows.Forms.GroupBox();
			this.comboBox_Bomb = new global::System.Windows.Forms.ComboBox();
			this.comboBox_Shoot = new global::System.Windows.Forms.ComboBox();
			this.comboBox_Slow = new global::System.Windows.Forms.ComboBox();
			this.comboBox_Pause = new global::System.Windows.Forms.ComboBox();
			this.comboBox_Skip = new global::System.Windows.Forms.ComboBox();
			this.label15 = new global::System.Windows.Forms.Label();
			this.label16 = new global::System.Windows.Forms.Label();
			this.label17 = new global::System.Windows.Forms.Label();
			this.label18 = new global::System.Windows.Forms.Label();
			this.label19 = new global::System.Windows.Forms.Label();
			this.groupBox6 = new global::System.Windows.Forms.GroupBox();
			this.radioButton_Circle1 = new global::System.Windows.Forms.RadioButton();
			this.radioButton_Circle3 = new global::System.Windows.Forms.RadioButton();
			this.radioButton_Circle2 = new global::System.Windows.Forms.RadioButton();
			this.groupBox7 = new global::System.Windows.Forms.GroupBox();
			this.label_Font = new global::System.Windows.Forms.Label();
			this.comboBox_Font = new global::System.Windows.Forms.ComboBox();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.groupBox4.SuspendLayout();
			this.groupBox5.SuspendLayout();
			this.groupBox6.SuspendLayout();
			this.groupBox7.SuspendLayout();
			base.SuspendLayout();
			this.button_应用.Location = new global::System.Drawing.Point(393, 206);
			this.button_应用.Name = "button_应用";
			this.button_应用.Size = new global::System.Drawing.Size(69, 36);
			this.button_应用.TabIndex = 0;
			this.button_应用.Text = "应用";
			this.button_应用.UseVisualStyleBackColor = true;
			this.button_应用.Click += new global::System.EventHandler(this.button_应用_Click);
			this.checkBox_useDirectInput.AutoSize = true;
			this.checkBox_useDirectInput.Location = new global::System.Drawing.Point(7, 25);
			this.checkBox_useDirectInput.Name = "checkBox_useDirectInput";
			this.checkBox_useDirectInput.Size = new global::System.Drawing.Size(138, 16);
			this.checkBox_useDirectInput.TabIndex = 1;
			this.checkBox_useDirectInput.Text = "使用DirectInput输入";
			this.checkBox_useDirectInput.UseVisualStyleBackColor = true;
			this.checkBox_useDirectInput.CheckedChanged += new global::System.EventHandler(this.checkBox_useDirectInput_CheckedChanged);
			this.checkBox_VerticalSync.AutoSize = true;
			this.checkBox_VerticalSync.Location = new global::System.Drawing.Point(6, 25);
			this.checkBox_VerticalSync.Name = "checkBox_VerticalSync";
			this.checkBox_VerticalSync.Size = new global::System.Drawing.Size(96, 16);
			this.checkBox_VerticalSync.TabIndex = 0;
			this.checkBox_VerticalSync.Text = "启用垂直同步";
			this.checkBox_VerticalSync.UseVisualStyleBackColor = true;
			this.groupBox1.Controls.Add(this.checkBox_VerticalSync);
			this.groupBox1.Location = new global::System.Drawing.Point(12, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new global::System.Drawing.Size(180, 54);
			this.groupBox1.TabIndex = 2;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "显示设置";
			this.groupBox2.Controls.Add(this.checkBox_useDirectInput);
			this.groupBox2.Location = new global::System.Drawing.Point(198, 12);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new global::System.Drawing.Size(192, 54);
			this.groupBox2.TabIndex = 3;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "输入设置";
			this.groupBox3.Controls.Add(this.label8);
			this.groupBox3.Controls.Add(this.label9);
			this.groupBox3.Controls.Add(this.label10);
			this.groupBox3.Controls.Add(this.label11);
			this.groupBox3.Controls.Add(this.label12);
			this.groupBox3.Controls.Add(this.label13);
			this.groupBox3.Controls.Add(this.label14);
			this.groupBox3.Controls.Add(this.label_bomb);
			this.groupBox3.Controls.Add(this.label_shoot);
			this.groupBox3.Controls.Add(this.label_slow);
			this.groupBox3.Controls.Add(this.label_down);
			this.groupBox3.Controls.Add(this.label_up);
			this.groupBox3.Controls.Add(this.label_right);
			this.groupBox3.Controls.Add(this.label_left);
			this.groupBox3.Controls.Add(this.label7);
			this.groupBox3.Controls.Add(this.label6);
			this.groupBox3.Controls.Add(this.label5);
			this.groupBox3.Controls.Add(this.label4);
			this.groupBox3.Controls.Add(this.label3);
			this.groupBox3.Controls.Add(this.label2);
			this.groupBox3.Controls.Add(this.label1);
			this.groupBox3.Location = new global::System.Drawing.Point(12, 72);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new global::System.Drawing.Size(180, 170);
			this.groupBox3.TabIndex = 4;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "键设置";
			this.label8.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label8.Enabled = false;
			this.label8.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label8.Location = new global::System.Drawing.Point(92, 142);
			this.label8.Name = "label8";
			this.label8.Size = new global::System.Drawing.Size(80, 20);
			this.label8.TabIndex = 13;
			this.label8.Text = "X";
			this.label8.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label9.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label9.Enabled = false;
			this.label9.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label9.Location = new global::System.Drawing.Point(92, 122);
			this.label9.Name = "label9";
			this.label9.Size = new global::System.Drawing.Size(80, 20);
			this.label9.TabIndex = 12;
			this.label9.Text = "Z";
			this.label9.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label10.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label10.Enabled = false;
			this.label10.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label10.Location = new global::System.Drawing.Point(92, 102);
			this.label10.Name = "label10";
			this.label10.Size = new global::System.Drawing.Size(80, 20);
			this.label10.TabIndex = 11;
			this.label10.Text = "ShiftKey";
			this.label10.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label11.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label11.Enabled = false;
			this.label11.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label11.Location = new global::System.Drawing.Point(92, 82);
			this.label11.Name = "label11";
			this.label11.Size = new global::System.Drawing.Size(80, 20);
			this.label11.TabIndex = 10;
			this.label11.Text = "Down";
			this.label11.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label12.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label12.Enabled = false;
			this.label12.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label12.Location = new global::System.Drawing.Point(92, 62);
			this.label12.Name = "label12";
			this.label12.Size = new global::System.Drawing.Size(80, 20);
			this.label12.TabIndex = 9;
			this.label12.Text = "Up";
			this.label12.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label13.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label13.Enabled = false;
			this.label13.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label13.Location = new global::System.Drawing.Point(92, 42);
			this.label13.Name = "label13";
			this.label13.Size = new global::System.Drawing.Size(80, 20);
			this.label13.TabIndex = 8;
			this.label13.Text = "Right";
			this.label13.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label14.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label14.Enabled = false;
			this.label14.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label14.Location = new global::System.Drawing.Point(92, 22);
			this.label14.Name = "label14";
			this.label14.Size = new global::System.Drawing.Size(80, 20);
			this.label14.TabIndex = 7;
			this.label14.Text = "Left";
			this.label14.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label_bomb.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label_bomb.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label_bomb.Location = new global::System.Drawing.Point(92, 142);
			this.label_bomb.Name = "label_bomb";
			this.label_bomb.Size = new global::System.Drawing.Size(80, 20);
			this.label_bomb.TabIndex = 13;
			this.label_bomb.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label_bomb.Click += new global::System.EventHandler(this.label_Click);
			this.label_shoot.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label_shoot.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label_shoot.Location = new global::System.Drawing.Point(92, 122);
			this.label_shoot.Name = "label_shoot";
			this.label_shoot.Size = new global::System.Drawing.Size(80, 20);
			this.label_shoot.TabIndex = 11;
			this.label_shoot.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label_shoot.Click += new global::System.EventHandler(this.label_Click);
			this.label_slow.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label_slow.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label_slow.Location = new global::System.Drawing.Point(92, 102);
			this.label_slow.Name = "label_slow";
			this.label_slow.Size = new global::System.Drawing.Size(80, 20);
			this.label_slow.TabIndex = 9;
			this.label_slow.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label_slow.Click += new global::System.EventHandler(this.label_Click);
			this.label_down.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label_down.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label_down.Location = new global::System.Drawing.Point(92, 82);
			this.label_down.Name = "label_down";
			this.label_down.Size = new global::System.Drawing.Size(80, 20);
			this.label_down.TabIndex = 7;
			this.label_down.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label_down.Click += new global::System.EventHandler(this.label_Click);
			this.label_up.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label_up.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label_up.Location = new global::System.Drawing.Point(92, 62);
			this.label_up.Name = "label_up";
			this.label_up.Size = new global::System.Drawing.Size(80, 20);
			this.label_up.TabIndex = 5;
			this.label_up.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label_up.Click += new global::System.EventHandler(this.label_Click);
			this.label_right.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label_right.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label_right.Location = new global::System.Drawing.Point(92, 42);
			this.label_right.Name = "label_right";
			this.label_right.Size = new global::System.Drawing.Size(80, 20);
			this.label_right.TabIndex = 3;
			this.label_right.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label_right.Click += new global::System.EventHandler(this.label_Click);
			this.label_left.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label_left.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label_left.Location = new global::System.Drawing.Point(92, 22);
			this.label_left.Name = "label_left";
			this.label_left.Size = new global::System.Drawing.Size(80, 20);
			this.label_left.TabIndex = 1;
			this.label_left.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label_left.Click += new global::System.EventHandler(this.label_Click);
			this.label7.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label7.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label7.Location = new global::System.Drawing.Point(6, 142);
			this.label7.Name = "label7";
			this.label7.Size = new global::System.Drawing.Size(80, 20);
			this.label7.TabIndex = 6;
			this.label7.Text = "Bomb/取消";
			this.label7.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label6.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label6.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label6.Location = new global::System.Drawing.Point(6, 122);
			this.label6.Name = "label6";
			this.label6.Size = new global::System.Drawing.Size(80, 20);
			this.label6.TabIndex = 5;
			this.label6.Text = "射击/确认";
			this.label6.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label5.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label5.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label5.Location = new global::System.Drawing.Point(6, 102);
			this.label5.Name = "label5";
			this.label5.Size = new global::System.Drawing.Size(80, 20);
			this.label5.TabIndex = 4;
			this.label5.Text = "低速移动";
			this.label5.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label4.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label4.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label4.Location = new global::System.Drawing.Point(6, 82);
			this.label4.Name = "label4";
			this.label4.Size = new global::System.Drawing.Size(80, 20);
			this.label4.TabIndex = 3;
			this.label4.Text = "下";
			this.label4.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label3.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label3.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label3.Location = new global::System.Drawing.Point(6, 62);
			this.label3.Name = "label3";
			this.label3.Size = new global::System.Drawing.Size(80, 20);
			this.label3.TabIndex = 2;
			this.label3.Text = "上";
			this.label3.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label2.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label2.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label2.Location = new global::System.Drawing.Point(6, 42);
			this.label2.Name = "label2";
			this.label2.Size = new global::System.Drawing.Size(80, 20);
			this.label2.TabIndex = 1;
			this.label2.Text = "右";
			this.label2.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label1.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label1.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label1.Location = new global::System.Drawing.Point(6, 22);
			this.label1.Name = "label1";
			this.label1.Size = new global::System.Drawing.Size(80, 20);
			this.label1.TabIndex = 0;
			this.label1.Text = "左";
			this.label1.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.button_应用并启动.Location = new global::System.Drawing.Point(468, 206);
			this.button_应用并启动.Name = "button_应用并启动";
			this.button_应用并启动.Size = new global::System.Drawing.Size(127, 36);
			this.button_应用并启动.TabIndex = 1;
			this.button_应用并启动.Text = "应用并启动";
			this.button_应用并启动.UseVisualStyleBackColor = true;
			this.button_应用并启动.Click += new global::System.EventHandler(this.button_应用并启动_Click);
			this.groupBox4.Controls.Add(this.checkBox_fullWindow);
			this.groupBox4.Location = new global::System.Drawing.Point(396, 12);
			this.groupBox4.Name = "groupBox4";
			this.groupBox4.Size = new global::System.Drawing.Size(202, 54);
			this.groupBox4.TabIndex = 5;
			this.groupBox4.TabStop = false;
			this.groupBox4.Text = "启动设置";
			this.checkBox_fullWindow.AccessibleRole = global::System.Windows.Forms.AccessibleRole.None;
			this.checkBox_fullWindow.AutoSize = true;
			this.checkBox_fullWindow.Location = new global::System.Drawing.Point(6, 25);
			this.checkBox_fullWindow.Name = "checkBox_fullWindow";
			this.checkBox_fullWindow.Size = new global::System.Drawing.Size(102, 16);
			this.checkBox_fullWindow.TabIndex = 9;
			this.checkBox_fullWindow.Text = " 全屏启动游戏";
			this.checkBox_fullWindow.UseVisualStyleBackColor = true;
			this.groupBox5.Controls.Add(this.comboBox_Bomb);
			this.groupBox5.Controls.Add(this.comboBox_Shoot);
			this.groupBox5.Controls.Add(this.comboBox_Slow);
			this.groupBox5.Controls.Add(this.comboBox_Pause);
			this.groupBox5.Controls.Add(this.comboBox_Skip);
			this.groupBox5.Controls.Add(this.label15);
			this.groupBox5.Controls.Add(this.label16);
			this.groupBox5.Controls.Add(this.label17);
			this.groupBox5.Controls.Add(this.label18);
			this.groupBox5.Controls.Add(this.label19);
			this.groupBox5.Location = new global::System.Drawing.Point(198, 72);
			this.groupBox5.Name = "groupBox5";
			this.groupBox5.Size = new global::System.Drawing.Size(192, 170);
			this.groupBox5.TabIndex = 6;
			this.groupBox5.TabStop = false;
			this.groupBox5.Text = "手柄设置";
			this.comboBox_Bomb.DropDownStyle = global::System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox_Bomb.FormattingEnabled = true;
			this.comboBox_Bomb.ImeMode = global::System.Windows.Forms.ImeMode.Hiragana;
			this.comboBox_Bomb.Items.AddRange(new object[]
			{
				"Button1", "Button2", "Button3", "Button4", "Button5", "Button6", "Button7", "Button8", "Button9", "Button10",
				"Button11", "Button12"
			});
			this.comboBox_Bomb.Location = new global::System.Drawing.Point(92, 134);
			this.comboBox_Bomb.Margin = new global::System.Windows.Forms.Padding(3, 2, 3, 2);
			this.comboBox_Bomb.Name = "comboBox_Bomb";
			this.comboBox_Bomb.Size = new global::System.Drawing.Size(94, 24);
			this.comboBox_Bomb.TabIndex = 24;
			this.comboBox_Shoot.DropDownStyle = global::System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox_Shoot.FormattingEnabled = true;
			this.comboBox_Shoot.ImeMode = global::System.Windows.Forms.ImeMode.Hiragana;
			this.comboBox_Shoot.Items.AddRange(new object[]
			{
				"Button1", "Button2", "Button3", "Button4", "Button5", "Button6", "Button7", "Button8", "Button9", "Button10",
				"Button11", "Button12"
			});
			this.comboBox_Shoot.Location = new global::System.Drawing.Point(92, 106);
			this.comboBox_Shoot.Margin = new global::System.Windows.Forms.Padding(3, 2, 3, 2);
			this.comboBox_Shoot.Name = "comboBox_Shoot";
			this.comboBox_Shoot.Size = new global::System.Drawing.Size(94, 24);
			this.comboBox_Shoot.TabIndex = 23;
			this.comboBox_Slow.DropDownStyle = global::System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox_Slow.FormattingEnabled = true;
			this.comboBox_Slow.ImeMode = global::System.Windows.Forms.ImeMode.Hiragana;
			this.comboBox_Slow.Items.AddRange(new object[]
			{
				"Button1", "Button2", "Button3", "Button4", "Button5", "Button6", "Button7", "Button8", "Button9", "Button10",
				"Button11", "Button12"
			});
			this.comboBox_Slow.Location = new global::System.Drawing.Point(92, 78);
			this.comboBox_Slow.Margin = new global::System.Windows.Forms.Padding(3, 2, 3, 2);
			this.comboBox_Slow.Name = "comboBox_Slow";
			this.comboBox_Slow.Size = new global::System.Drawing.Size(94, 24);
			this.comboBox_Slow.TabIndex = 22;
			this.comboBox_Pause.DropDownStyle = global::System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox_Pause.FormattingEnabled = true;
			this.comboBox_Pause.ImeMode = global::System.Windows.Forms.ImeMode.Hiragana;
			this.comboBox_Pause.Items.AddRange(new object[]
			{
				"Button1", "Button2", "Button3", "Button4", "Button5", "Button6", "Button7", "Button8", "Button9", "Button10",
				"Button11", "Button12"
			});
			this.comboBox_Pause.Location = new global::System.Drawing.Point(92, 50);
			this.comboBox_Pause.Margin = new global::System.Windows.Forms.Padding(3, 2, 3, 2);
			this.comboBox_Pause.Name = "comboBox_Pause";
			this.comboBox_Pause.Size = new global::System.Drawing.Size(94, 24);
			this.comboBox_Pause.TabIndex = 21;
			this.comboBox_Skip.DropDownStyle = global::System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox_Skip.FormattingEnabled = true;
			this.comboBox_Skip.ImeMode = global::System.Windows.Forms.ImeMode.Hiragana;
			this.comboBox_Skip.Items.AddRange(new object[]
			{
				"Button1", "Button2", "Button3", "Button4", "Button5", "Button6", "Button7", "Button8", "Button9", "Button10",
				"Button11", "Button12"
			});
			this.comboBox_Skip.Location = new global::System.Drawing.Point(92, 22);
			this.comboBox_Skip.Margin = new global::System.Windows.Forms.Padding(3, 2, 3, 2);
			this.comboBox_Skip.Name = "comboBox_Skip";
			this.comboBox_Skip.Size = new global::System.Drawing.Size(94, 24);
			this.comboBox_Skip.TabIndex = 20;
			this.label15.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label15.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label15.Location = new global::System.Drawing.Point(6, 135);
			this.label15.Name = "label15";
			this.label15.Size = new global::System.Drawing.Size(80, 20);
			this.label15.TabIndex = 19;
			this.label15.Text = "Bomb/取消";
			this.label15.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label16.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label16.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label16.Location = new global::System.Drawing.Point(6, 107);
			this.label16.Name = "label16";
			this.label16.Size = new global::System.Drawing.Size(80, 20);
			this.label16.TabIndex = 18;
			this.label16.Text = "射击/确认";
			this.label16.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label17.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label17.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label17.Location = new global::System.Drawing.Point(6, 79);
			this.label17.Name = "label17";
			this.label17.Size = new global::System.Drawing.Size(80, 20);
			this.label17.TabIndex = 17;
			this.label17.Text = "低速移动";
			this.label17.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label18.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label18.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label18.Location = new global::System.Drawing.Point(6, 51);
			this.label18.Name = "label18";
			this.label18.Size = new global::System.Drawing.Size(80, 20);
			this.label18.TabIndex = 16;
			this.label18.Text = "暂停";
			this.label18.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.label19.BorderStyle = global::System.Windows.Forms.BorderStyle.Fixed3D;
			this.label19.FlatStyle = global::System.Windows.Forms.FlatStyle.System;
			this.label19.Location = new global::System.Drawing.Point(7, 23);
			this.label19.Name = "label19";
			this.label19.Size = new global::System.Drawing.Size(80, 20);
			this.label19.TabIndex = 15;
			this.label19.Text = "跳过对话";
			this.label19.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.groupBox6.Controls.Add(this.radioButton_Circle1);
			this.groupBox6.Controls.Add(this.radioButton_Circle3);
			this.groupBox6.Controls.Add(this.radioButton_Circle2);
			this.groupBox6.Location = new global::System.Drawing.Point(396, 72);
			this.groupBox6.Name = "groupBox6";
			this.groupBox6.Size = new global::System.Drawing.Size(202, 50);
			this.groupBox6.TabIndex = 7;
			this.groupBox6.TabStop = false;
			this.groupBox6.Text = "绘制周期";
			this.radioButton_Circle1.AutoSize = true;
			this.radioButton_Circle1.Location = new global::System.Drawing.Point(6, 25);
			this.radioButton_Circle1.Name = "radioButton_Circle1";
			this.radioButton_Circle1.Size = new global::System.Drawing.Size(47, 16);
			this.radioButton_Circle1.TabIndex = 3;
			this.radioButton_Circle1.TabStop = true;
			this.radioButton_Circle1.Text = "每次";
			this.radioButton_Circle1.UseVisualStyleBackColor = true;
			this.radioButton_Circle3.AutoSize = true;
			this.radioButton_Circle3.Location = new global::System.Drawing.Point(133, 25);
			this.radioButton_Circle3.Name = "radioButton_Circle3";
			this.radioButton_Circle3.Size = new global::System.Drawing.Size(53, 16);
			this.radioButton_Circle3.TabIndex = 5;
			this.radioButton_Circle3.TabStop = true;
			this.radioButton_Circle3.Text = "1/3次";
			this.radioButton_Circle3.UseVisualStyleBackColor = true;
			this.radioButton_Circle2.AutoSize = true;
			this.radioButton_Circle2.Location = new global::System.Drawing.Point(70, 25);
			this.radioButton_Circle2.Name = "radioButton_Circle2";
			this.radioButton_Circle2.Size = new global::System.Drawing.Size(53, 16);
			this.radioButton_Circle2.TabIndex = 4;
			this.radioButton_Circle2.TabStop = true;
			this.radioButton_Circle2.Text = "1/2次";
			this.radioButton_Circle2.UseVisualStyleBackColor = true;
			this.groupBox7.Controls.Add(this.label_Font);
			this.groupBox7.Controls.Add(this.comboBox_Font);
			this.groupBox7.Location = new global::System.Drawing.Point(393, 128);
			this.groupBox7.Name = "groupBox7";
			this.groupBox7.Size = new global::System.Drawing.Size(202, 74);
			this.groupBox7.TabIndex = 8;
			this.groupBox7.TabStop = false;
			this.groupBox7.Text = "字体设置";
			this.label_Font.Font = new global::System.Drawing.Font("宋体", 12f);
			this.label_Font.Location = new global::System.Drawing.Point(6, 47);
			this.label_Font.Name = "label_Font";
			this.label_Font.Size = new global::System.Drawing.Size(156, 33);
			this.label_Font.TabIndex = 21;
			this.label_Font.Text = "夏夜祭E比星莲船E难";
			this.label_Font.TextAlign = global::System.Drawing.ContentAlignment.MiddleCenter;
			this.comboBox_Font.DropDownStyle = global::System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox_Font.FormattingEnabled = true;
			this.comboBox_Font.ImeMode = global::System.Windows.Forms.ImeMode.Hiragana;
			this.comboBox_Font.Items.AddRange(new object[] { "宋体", "黑体", "楷体", "仿宋", "新宋体", "幼圆", "隶书" });
			this.comboBox_Font.Location = new global::System.Drawing.Point(6, 23);
			this.comboBox_Font.Margin = new global::System.Windows.Forms.Padding(3, 2, 3, 2);
			this.comboBox_Font.Name = "comboBox_Font";
			this.comboBox_Font.Size = new global::System.Drawing.Size(190, 24);
			this.comboBox_Font.TabIndex = 20;
			this.comboBox_Font.SelectedIndexChanged += new global::System.EventHandler(this.comboBox_Font_SelectedIndexChanged);
			base.AutoScaleMode = global::System.Windows.Forms.AutoScaleMode.None;
			base.ClientSize = new global::System.Drawing.Size(609, 252);
			base.Controls.Add(this.groupBox7);
			base.Controls.Add(this.groupBox6);
			base.Controls.Add(this.groupBox5);
			base.Controls.Add(this.groupBox4);
			base.Controls.Add(this.button_应用并启动);
			base.Controls.Add(this.groupBox3);
			base.Controls.Add(this.groupBox2);
			base.Controls.Add(this.groupBox1);
			base.Controls.Add(this.button_应用);
			this.Font = new global::System.Drawing.Font("宋体", 12f, global::System.Drawing.FontStyle.Regular, global::System.Drawing.GraphicsUnit.Point, 134);
			base.Icon = (global::System.Drawing.Icon)componentResourceManager.GetObject("$this.Icon");
			base.KeyPreview = true;
			base.MaximizeBox = false;
			this.MaximumSize = new global::System.Drawing.Size(625, 290);
			this.MinimumSize = new global::System.Drawing.Size(625, 290);
			base.Name = "Form_Config";
			base.StartPosition = global::System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Config";
			base.KeyUp += new global::System.Windows.Forms.KeyEventHandler(this.Form_Config_KeyUp);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.groupBox3.ResumeLayout(false);
			this.groupBox4.ResumeLayout(false);
			this.groupBox4.PerformLayout();
			this.groupBox5.ResumeLayout(false);
			this.groupBox6.ResumeLayout(false);
			this.groupBox6.PerformLayout();
			this.groupBox7.ResumeLayout(false);
			base.ResumeLayout(false);
		}

		private global::System.ComponentModel.IContainer components;

		private global::System.Windows.Forms.Button button_应用;

		private global::System.Windows.Forms.CheckBox checkBox_useDirectInput;

		private global::System.Windows.Forms.CheckBox checkBox_VerticalSync;

		private global::System.Windows.Forms.GroupBox groupBox1;

		private global::System.Windows.Forms.GroupBox groupBox2;

		private global::System.Windows.Forms.GroupBox groupBox3;

		private global::System.Windows.Forms.Label label_right;

		private global::System.Windows.Forms.Label label_left;

		private global::System.Windows.Forms.Label label6;

		private global::System.Windows.Forms.Label label5;

		private global::System.Windows.Forms.Label label4;

		private global::System.Windows.Forms.Label label3;

		private global::System.Windows.Forms.Label label2;

		private global::System.Windows.Forms.Label label1;

		private global::System.Windows.Forms.Button button_应用并启动;

		private global::System.Windows.Forms.Label label_slow;

		private global::System.Windows.Forms.Label label_down;

		private global::System.Windows.Forms.Label label_up;

		private global::System.Windows.Forms.Label label_bomb;

		private global::System.Windows.Forms.Label label_shoot;

		private global::System.Windows.Forms.GroupBox groupBox4;

		private global::System.Windows.Forms.Label label7;

		private global::System.Windows.Forms.Label label8;

		private global::System.Windows.Forms.Label label9;

		private global::System.Windows.Forms.Label label10;

		private global::System.Windows.Forms.Label label11;

		private global::System.Windows.Forms.Label label12;

		private global::System.Windows.Forms.Label label13;

		private global::System.Windows.Forms.Label label14;

		private global::System.Windows.Forms.GroupBox groupBox5;

		private global::System.Windows.Forms.Label label15;

		private global::System.Windows.Forms.Label label16;

		private global::System.Windows.Forms.Label label17;

		private global::System.Windows.Forms.Label label18;

		private global::System.Windows.Forms.Label label19;

		private global::System.Windows.Forms.ComboBox comboBox_Skip;

		private global::System.Windows.Forms.GroupBox groupBox6;

		private global::System.Windows.Forms.RadioButton radioButton_Circle3;

		private global::System.Windows.Forms.RadioButton radioButton_Circle2;

		private global::System.Windows.Forms.RadioButton radioButton_Circle1;

		private global::System.Windows.Forms.ComboBox comboBox_Bomb;

		private global::System.Windows.Forms.ComboBox comboBox_Shoot;

		private global::System.Windows.Forms.ComboBox comboBox_Slow;

		private global::System.Windows.Forms.ComboBox comboBox_Pause;

		private global::System.Windows.Forms.GroupBox groupBox7;

		private global::System.Windows.Forms.ComboBox comboBox_Font;

		private global::System.Windows.Forms.Label label_Font;

		private global::System.Windows.Forms.CheckBox checkBox_fullWindow;
	}
}
