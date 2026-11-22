using System;

namespace SlimDX
{
	public class Direct3D9Exception : Exception
	{
		public Result ResultCode { get; private set; }

		public Direct3D9Exception(string message)
			: base(message)
		{
			this.ResultCode = new Result(-1);
		}
	}
}
