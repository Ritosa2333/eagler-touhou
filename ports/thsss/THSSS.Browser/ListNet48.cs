using System;
using System.Collections.Generic;

namespace Shooting
{
	public static class ListNet48
	{
		public static void ForEachNet48<T>(this List<T> list, Action<T> action)
		{
			if (list == null || action == null)
			{
				return;
			}
			for (var i = 0; i < list.Count; i++)
			{
				action(list[i]);
			}
		}
	}
}
