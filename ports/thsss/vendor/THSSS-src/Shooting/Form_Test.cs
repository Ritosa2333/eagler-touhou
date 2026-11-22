using System;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace Shooting;

public class Form_Test : Form
{
	private IContainer components;

	private NumericUpDown numericUpDown1;

	private NumericUpDown numericUpDown2;

	private NumericUpDown numericUpDown3;

	private NumericUpDown numericUpDown4;

	private NumericUpDown numericUpDown5;

	private NumericUpDown numericUpDown6;

	private RichTextBox richTextBox1;

	private NumericUpDown numericUpDown7;

	private NumericUpDown numericUpDown8;

	public ListBox listBox1;

	public CheckBox checkBox1;

	public FolderBrowserDialog folderBrowserDialog1;

	public TreeView treeView1;

	public int Para1 => (int)numericUpDown1.Value;

	public int Para2 => (int)numericUpDown2.Value;

	public int Para3 => (int)numericUpDown3.Value;

	public int Para4 => (int)numericUpDown4.Value;

	public int Para5 => (int)numericUpDown5.Value;

	public int Para6 => (int)numericUpDown6.Value;

	public int Para7 => (int)numericUpDown7.Value;

	public int Para8 => (int)numericUpDown8.Value;

	public Form_Test()
	{
		InitializeComponent();
		folderBrowserDialog1.SelectedPath = Application.StartupPath + "\\CS";
	}

	private void Form_Test_Load(object sender, EventArgs e)
	{
		((Form)this).Location = new Point(((Form)this).Location.X + 325 + ((Control)this).Width / 2, ((Form)this).Location.Y);
	}

	public void SendString(string s)
	{
		RichTextBox val = richTextBox1;
		((Control)val).Text = ((Control)val).Text + s;
		RichTextBox val2 = richTextBox1;
		((Control)val2).Text = ((Control)val2).Text + "\n";
	}

	private void listBox1_DoubleClick(object sender, EventArgs e)
	{
		//IL_0007: Unknown result type (might be due to invalid IL or missing references)
		//IL_000c: Invalid comparison between I4 and Unknown
		if (1 == (int)((CommonDialog)folderBrowserDialog1).ShowDialog())
		{
			listBox1.Items.Clear();
			string[] files = Directory.GetFiles(folderBrowserDialog1.SelectedPath, "*.mbg");
			for (int i = 0; i < files.Length; i++)
			{
				FileInfo fileInfo = new FileInfo(files[i]);
				listBox1.Items.Add((object)fileInfo.Name);
			}
			if (listBox1.Items.Count > 0)
			{
				((ListControl)listBox1).SelectedIndex = 0;
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
		//IL_0001: Unknown result type (might be due to invalid IL or missing references)
		//IL_000b: Expected Obj, but got Unknown
		//IL_000c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0016: Expected Obj, but got Unknown
		//IL_0017: Unknown result type (might be due to invalid IL or missing references)
		//IL_0021: Expected Obj, but got Unknown
		//IL_0022: Unknown result type (might be due to invalid IL or missing references)
		//IL_002c: Expected Obj, but got Unknown
		//IL_002d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0037: Expected Obj, but got Unknown
		//IL_0038: Unknown result type (might be due to invalid IL or missing references)
		//IL_0042: Expected Obj, but got Unknown
		//IL_0043: Unknown result type (might be due to invalid IL or missing references)
		//IL_004d: Expected Obj, but got Unknown
		//IL_004e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0058: Expected Obj, but got Unknown
		//IL_0059: Unknown result type (might be due to invalid IL or missing references)
		//IL_0063: Expected Obj, but got Unknown
		//IL_0064: Unknown result type (might be due to invalid IL or missing references)
		//IL_006e: Expected Obj, but got Unknown
		//IL_006f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0079: Expected Obj, but got Unknown
		//IL_007a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0084: Expected Obj, but got Unknown
		//IL_0085: Unknown result type (might be due to invalid IL or missing references)
		//IL_008f: Expected Obj, but got Unknown
		numericUpDown1 = new NumericUpDown();
		numericUpDown2 = new NumericUpDown();
		numericUpDown3 = new NumericUpDown();
		numericUpDown4 = new NumericUpDown();
		numericUpDown5 = new NumericUpDown();
		numericUpDown6 = new NumericUpDown();
		richTextBox1 = new RichTextBox();
		numericUpDown7 = new NumericUpDown();
		numericUpDown8 = new NumericUpDown();
		listBox1 = new ListBox();
		checkBox1 = new CheckBox();
		folderBrowserDialog1 = new FolderBrowserDialog();
		treeView1 = new TreeView();
		((ISupportInitialize)numericUpDown1).BeginInit();
		((ISupportInitialize)numericUpDown2).BeginInit();
		((ISupportInitialize)numericUpDown3).BeginInit();
		((ISupportInitialize)numericUpDown4).BeginInit();
		((ISupportInitialize)numericUpDown5).BeginInit();
		((ISupportInitialize)numericUpDown6).BeginInit();
		((ISupportInitialize)numericUpDown7).BeginInit();
		((ISupportInitialize)numericUpDown8).BeginInit();
		((Control)this).SuspendLayout();
		((Control)numericUpDown1).Location = new Point(0, 0);
		numericUpDown1.Maximum = new decimal(new int[4] { 255, 0, 0, 0 });
		((Control)numericUpDown1).Name = "numericUpDown1";
		((Control)numericUpDown1).Size = new Size(128, 21);
		((Control)numericUpDown1).TabIndex = 0;
		((UpDownBase)numericUpDown1).TextAlign = (HorizontalAlignment)2;
		((Control)numericUpDown2).Location = new Point(0, 22);
		numericUpDown2.Maximum = new decimal(new int[4] { 255, 0, 0, 0 });
		((Control)numericUpDown2).Name = "numericUpDown2";
		((Control)numericUpDown2).Size = new Size(128, 21);
		((Control)numericUpDown2).TabIndex = 1;
		((UpDownBase)numericUpDown2).TextAlign = (HorizontalAlignment)2;
		((Control)numericUpDown3).Location = new Point(0, 44);
		numericUpDown3.Maximum = new decimal(new int[4] { 255, 0, 0, 0 });
		((Control)numericUpDown3).Name = "numericUpDown3";
		((Control)numericUpDown3).Size = new Size(128, 21);
		((Control)numericUpDown3).TabIndex = 2;
		((UpDownBase)numericUpDown3).TextAlign = (HorizontalAlignment)2;
		numericUpDown4.Increment = new decimal(new int[4] { 500, 0, 0, 0 });
		((Control)numericUpDown4).Location = new Point(0, 73);
		numericUpDown4.Maximum = new decimal(new int[4] { 100000, 0, 0, 0 });
		((Control)numericUpDown4).Name = "numericUpDown4";
		((Control)numericUpDown4).Size = new Size(128, 21);
		((Control)numericUpDown4).TabIndex = 3;
		((UpDownBase)numericUpDown4).TextAlign = (HorizontalAlignment)2;
		numericUpDown5.Increment = new decimal(new int[4] { 10, 0, 0, 0 });
		((Control)numericUpDown5).Location = new Point(0, 94);
		numericUpDown5.Maximum = new decimal(new int[4] { 10000, 0, 0, 0 });
		((Control)numericUpDown5).Name = "numericUpDown5";
		((Control)numericUpDown5).Size = new Size(128, 21);
		((Control)numericUpDown5).TabIndex = 4;
		((UpDownBase)numericUpDown5).TextAlign = (HorizontalAlignment)2;
		numericUpDown6.Increment = new decimal(new int[4] { 10, 0, 0, 0 });
		((Control)numericUpDown6).Location = new Point(0, 115);
		numericUpDown6.Maximum = new decimal(new int[4] { 10000, 0, 0, 0 });
		((Control)numericUpDown6).Name = "numericUpDown6";
		((Control)numericUpDown6).Size = new Size(128, 21);
		((Control)numericUpDown6).TabIndex = 5;
		((UpDownBase)numericUpDown6).TextAlign = (HorizontalAlignment)2;
		((Control)richTextBox1).Anchor = (AnchorStyles)7;
		((Control)richTextBox1).Location = new Point(0, 209);
		((Control)richTextBox1).Name = "richTextBox1";
		((Control)richTextBox1).Size = new Size(128, 272);
		((Control)richTextBox1).TabIndex = 8;
		((Control)richTextBox1).Text = "";
		numericUpDown7.Increment = new decimal(new int[4] { 10, 0, 0, 0 });
		((Control)numericUpDown7).Location = new Point(0, 136);
		numericUpDown7.Maximum = new decimal(new int[4] { 10000, 0, 0, 0 });
		((Control)numericUpDown7).Name = "numericUpDown7";
		((Control)numericUpDown7).Size = new Size(128, 21);
		((Control)numericUpDown7).TabIndex = 6;
		((UpDownBase)numericUpDown7).TextAlign = (HorizontalAlignment)2;
		numericUpDown8.Increment = new decimal(new int[4] { 100, 0, 0, 0 });
		((Control)numericUpDown8).Location = new Point(0, 157);
		numericUpDown8.Maximum = new decimal(new int[4] { 10000, 0, 0, 0 });
		((Control)numericUpDown8).Name = "numericUpDown8";
		((Control)numericUpDown8).Size = new Size(128, 21);
		((Control)numericUpDown8).TabIndex = 7;
		((UpDownBase)numericUpDown8).TextAlign = (HorizontalAlignment)2;
		((Control)listBox1).Anchor = (AnchorStyles)13;
		((ListControl)listBox1).FormattingEnabled = true;
		listBox1.HorizontalScrollbar = true;
		listBox1.ItemHeight = 12;
		((Control)listBox1).Location = new Point(134, 0);
		((Control)listBox1).Name = "listBox1";
		((Control)listBox1).Size = new Size(183, 196);
		((Control)listBox1).TabIndex = 9;
		((Control)listBox1).DoubleClick += listBox1_DoubleClick;
		((Control)checkBox1).AutoSize = true;
		((Control)checkBox1).Location = new Point(0, 187);
		((Control)checkBox1).Name = "checkBox1";
		((Control)checkBox1).Size = new Size(96, 16);
		((Control)checkBox1).TabIndex = 10;
		((Control)checkBox1).Text = "显示判定范围";
		((ButtonBase)checkBox1).UseVisualStyleBackColor = true;
		((Control)treeView1).Anchor = (AnchorStyles)15;
		((Control)treeView1).Location = new Point(134, 209);
		((Control)treeView1).Name = "treeView1";
		((Control)treeView1).Size = new Size(183, 272);
		((Control)treeView1).TabIndex = 11;
		((ContainerControl)this).AutoScaleDimensions = new SizeF(6f, 12f);
		((ContainerControl)this).AutoScaleMode = (AutoScaleMode)1;
		((Form)this).ClientSize = new Size(318, 480);
		((Control)this).Controls.Add((Control)(object)treeView1);
		((Control)this).Controls.Add((Control)(object)checkBox1);
		((Control)this).Controls.Add((Control)(object)listBox1);
		((Control)this).Controls.Add((Control)(object)numericUpDown8);
		((Control)this).Controls.Add((Control)(object)numericUpDown7);
		((Control)this).Controls.Add((Control)(object)richTextBox1);
		((Control)this).Controls.Add((Control)(object)numericUpDown4);
		((Control)this).Controls.Add((Control)(object)numericUpDown5);
		((Control)this).Controls.Add((Control)(object)numericUpDown6);
		((Control)this).Controls.Add((Control)(object)numericUpDown3);
		((Control)this).Controls.Add((Control)(object)numericUpDown2);
		((Control)this).Controls.Add((Control)(object)numericUpDown1);
		((Control)this).Name = "Form_Test";
		((Form)this).StartPosition = (FormStartPosition)1;
		((Control)this).Text = "Test";
		((Form)this).Load += Form_Test_Load;
		((ISupportInitialize)numericUpDown1).EndInit();
		((ISupportInitialize)numericUpDown2).EndInit();
		((ISupportInitialize)numericUpDown3).EndInit();
		((ISupportInitialize)numericUpDown4).EndInit();
		((ISupportInitialize)numericUpDown5).EndInit();
		((ISupportInitialize)numericUpDown6).EndInit();
		((ISupportInitialize)numericUpDown7).EndInit();
		((ISupportInitialize)numericUpDown8).EndInit();
		((Control)this).ResumeLayout(false);
		((Control)this).PerformLayout();
	}
}
