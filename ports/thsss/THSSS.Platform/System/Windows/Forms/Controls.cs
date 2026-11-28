using System.Collections;

namespace System.Windows.Forms
{
	public class Keys
	{
		private readonly string name;

		private Keys(string name)
		{
			this.name = name;
		}

		public static readonly Keys None = new Keys("None");
		public static readonly Keys Left = new Keys("Left");
		public static readonly Keys Right = new Keys("Right");
		public static readonly Keys Up = new Keys("Up");
		public static readonly Keys Down = new Keys("Down");
		public static readonly Keys ShiftKey = new Keys("ShiftKey");
		public static readonly Keys ControlKey = new Keys("ControlKey");
		public static readonly Keys Menu = new Keys("Menu");
		public static readonly Keys Return = new Keys("Return");
		public static readonly Keys Escape = new Keys("Escape");
		public static readonly Keys Space = new Keys("Space");
		public static readonly Keys Oemplus = new Keys("Oemplus");
		public static readonly Keys OemMinus = new Keys("OemMinus");
		public static readonly Keys F8 = new Keys("F8");
		public static readonly Keys P = new Keys("P");
		public static readonly Keys R = new Keys("R");
		public static readonly Keys Z = new Keys("Z");
		public static readonly Keys X = new Keys("X");
		public static readonly Keys C = new Keys("C");
		public static readonly Keys A = new Keys("A");
		public static readonly Keys S = new Keys("S");
		public static readonly Keys D = new Keys("D");
		public static readonly Keys Q = new Keys("Q");
		public static readonly Keys W = new Keys("W");
		public static readonly Keys E = new Keys("E");

		public static Keys Parse(string name)
		{
			return new Keys(name ?? "None");
		}

		public override string ToString()
		{
			return this.name;
		}

		public override bool Equals(object obj)
		{
			var keys = obj as Keys;
			return keys != null && keys.name == this.name;
		}

		public override int GetHashCode()
		{
			return this.name != null ? this.name.GetHashCode() : 0;
		}
	}

	public class CommonDialog
	{
		public DialogResult ShowDialog()
		{
			return DialogResult.Cancel;
		}
	}

	public class ButtonBase : Control
	{
		public bool UseVisualStyleBackColor { get; set; }
	}

	public class ListControl : Control
	{
		public int SelectedIndex { get; set; }
	}

	public class UpDownBase : Control
	{
		public HorizontalAlignment TextAlign { get; set; }
	}

	public class NumericUpDown : UpDownBase, System.ComponentModel.ISupportInitialize
	{
		public decimal Value { get; set; }

		public decimal Minimum { get; set; }

		public decimal Maximum { get; set; }

		public decimal Increment { get; set; }

		public int DecimalPlaces { get; set; }

		public void BeginInit()
		{
		}

		public void EndInit()
		{
		}
	}

	public class RichTextBox : Control
	{
	}

	public class CheckBox : ButtonBase
	{
		public bool Checked { get; set; }
	}

	public class Button : ButtonBase
	{
	}

	public class ListBox : ListControl
	{
		public bool FormattingEnabled { get; set; }

		public bool HorizontalScrollbar { get; set; }

		public int ItemHeight { get; set; }

		public ObjectCollection Items { get; private set; }

		public ListBox()
		{
			this.Items = new ObjectCollection();
		}

		public class ObjectCollection
		{
			private readonly ArrayList list = new ArrayList();

			public int Count
			{
				get
				{
					return this.list.Count;
				}
			}

			public void Add(object item)
			{
				this.list.Add(item);
			}

			public void Clear()
			{
				this.list.Clear();
			}
		}
	}

	public class TreeNode
	{
		public string Name { get; set; }

		public string Text { get; set; }

		public TreeNodeCollection Nodes { get; private set; }

		public TreeNode()
		{
			this.Nodes = new TreeNodeCollection();
		}

		public TreeNode(string text)
			: this()
		{
			this.Text = text;
		}
	}

	public class TreeNodeCollection
	{
		private readonly ArrayList list = new ArrayList();

		public void Add(TreeNode node)
		{
			this.list.Add(node);
		}
	}

	public class TreeView : Control
	{
		public TreeNodeCollection Nodes { get; private set; }

		public TreeView()
		{
			this.Nodes = new TreeNodeCollection();
		}
	}

	public class Panel : Control
	{
	}

	public class FolderBrowserDialog
	{
		public string SelectedPath { get; set; }

		public DialogResult ShowDialog()
		{
			return DialogResult.Cancel;
		}
	}
}
