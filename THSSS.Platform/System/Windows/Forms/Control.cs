using System.Collections.Generic;
using System.Drawing;

namespace System.Windows.Forms
{
	public class Control : IDisposable
	{
		private readonly List<Control> controls = new List<Control>();

		private Size size;

		public string Name { get; set; }

		public string Text { get; set; }

		public bool Visible { get; set; }

		public bool Enabled { get; set; }

		public bool AutoSize { get; set; }

		public bool DoubleBuffered { get; set; }

		public int TabIndex { get; set; }

		public Point Location { get; set; }

		public Size Size
		{
			get
			{
				return this.size;
			}
			set
			{
				if (this.size == value)
				{
					return;
				}
				this.size = value;
				this.RaiseSizeChanged();
			}
		}

		public Size MaximumSize { get; set; }

		public Size MinimumSize { get; set; }

		public Color BackColor { get; set; }

		public Color ForeColor { get; set; }

		public Image BackgroundImage { get; set; }

		public ImageLayout BackgroundImageLayout { get; set; }

		public BorderStyle BorderStyle { get; set; }

		public Padding Margin { get; set; }

		public Padding Padding { get; set; }

		public AnchorStyles Anchor { get; set; }

		public DockStyle Dock { get; set; }

		public Font Font { get; set; }

		public AutoScaleMode AutoScaleMode { get; set; }

		public ImeMode ImeMode { get; set; }

		public object Tag { get; set; }

		public event EventHandler DoubleClick;

		public void RaiseDoubleClick()
		{
			var eventHandler = this.DoubleClick;
			if (eventHandler != null)
			{
				eventHandler(this, EventArgs.Empty);
			}
		}

		public int Width
		{
			get
			{
				return this.Size.Width;
			}
			set
			{
				this.Size = new Size(value, this.Size.Height);
			}
		}

		public int Height
		{
			get
			{
				return this.Size.Height;
			}
			set
			{
				this.Size = new Size(this.Size.Width, value);
			}
		}

		public int Left
		{
			get
			{
				return this.Location.X;
			}
			set
			{
				this.Location = new Point(value, this.Location.Y);
			}
		}

		public int Top
		{
			get
			{
				return this.Location.Y;
			}
			set
			{
				this.Location = new Point(this.Location.X, value);
			}
		}

		public int Right
		{
			get
			{
				return this.Left + this.Width;
			}
		}

		public int Bottom
		{
			get
			{
				return this.Top + this.Height;
			}
		}

		public Rectangle Bounds
		{
			get
			{
				return new Rectangle(this.Location, this.Size);
			}
			set
			{
				this.Location = value.Location;
				this.Size = value.Size;
			}
		}

		public Size ClientSize
		{
			get
			{
				return this.Size;
			}
			set
			{
				this.Size = value;
			}
		}

		public virtual IntPtr Handle
		{
			get
			{
				return new IntPtr(1);
			}
		}

		public ControlCollection Controls { get; private set; }

		public event EventHandler Click;

		public event EventHandler SizeChanged;

		public event KeyEventHandler KeyDown;

		public event KeyEventHandler KeyUp;

		public Control()
		{
			this.Visible = true;
			this.Enabled = true;
			this.Controls = new ControlCollection(this.controls);
			this.Size = new Size(640, 480);
		}

		public void Show()
		{
			this.Visible = true;
		}

		public void Refresh()
		{
		}

		public void SuspendLayout()
		{
		}

		public void ResumeLayout()
		{
		}

		public void ResumeLayout(bool performLayout)
		{
		}

		public void PerformLayout()
		{
		}

		public void RaiseClick()
		{
			var eventHandler = this.Click;
			if (eventHandler != null)
			{
				eventHandler(this, EventArgs.Empty);
			}
		}

		public void RaiseSizeChanged()
		{
			var eventHandler = this.SizeChanged;
			if (eventHandler != null)
			{
				eventHandler(this, EventArgs.Empty);
			}
		}

		public void RaiseKeyDown(Keys key)
		{
			var keyEventHandler = this.KeyDown;
			if (keyEventHandler != null)
			{
				keyEventHandler(this, new KeyEventArgs(key));
			}
		}

		public void RaiseKeyUp(Keys key)
		{
			var keyEventHandler = this.KeyUp;
			if (keyEventHandler != null)
			{
				keyEventHandler(this, new KeyEventArgs(key));
			}
		}

		public void Dispose()
		{
			this.Dispose(true);
		}

		protected virtual void Dispose(bool disposing)
		{
		}

		public class ControlCollection
		{
			private readonly List<Control> items;

			public ControlCollection(List<Control> items)
			{
				this.items = items;
			}

			public void Add(Control control)
			{
				this.items.Add(control);
			}

			public void Clear()
			{
				this.items.Clear();
			}

			public int Count
			{
				get
				{
					return this.items.Count;
				}
			}
		}
	}
}
