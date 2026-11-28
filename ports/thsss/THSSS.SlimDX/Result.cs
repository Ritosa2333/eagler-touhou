namespace SlimDX
{
	public class Result
	{
		public int Code { get; private set; }

		public bool IsFailure
		{
			get
			{
				return this.Code < 0;
			}
		}

		public string Description
		{
			get
			{
				return "0x" + this.Code.ToString("X8");
			}
		}

		public static Result Last { get; set; }

		public Result(int code)
		{
			this.Code = code;
		}

		public static Result Success
		{
			get
			{
				return new Result(0);
			}
		}

		public static bool operator ==(Result a, Result b)
		{
			if (object.ReferenceEquals(a, b))
			{
				return true;
			}
			if (a is null || b is null)
			{
				return false;
			}
			return a.Code == b.Code;
		}

		public static bool operator !=(Result a, Result b)
		{
			return !(a == b);
		}

		public override bool Equals(object obj)
		{
			var result = obj as Result;
			return result != null && result.Code == this.Code;
		}

		public override int GetHashCode()
		{
			return this.Code;
		}
	}
}
