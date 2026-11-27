using System;
using System.Linq;
using Mono.Cecil;
using Mono.Cecil.Cil;

internal static class Program
{
	static int Main(string[] args)
	{
		if (args.Length != 3)
		{
			Console.Error.WriteLine("usage: rewrite <THSSS.exe> <THSSS.Observe.dll> <output.exe>");
			return 2;
		}
		var exe = AssemblyDefinition.ReadAssembly(args[0], new ReaderParameters { ReadWrite = false });
		var observe = AssemblyDefinition.ReadAssembly(args[1]);
		var sink = observe.MainModule.Types.First(type => type.FullName == "THSSS.Observe.VerifyHost");
		var after = sink.Methods.First(method => method.Name == "AfterUpdate");
		var begin = sink.Methods.First(method => method.Name == "BeginFrame");
		var note = sink.Methods.First(method => method.Name == "NoteRng" && method.Parameters.Count == 4);
		var noteDouble = sink.Methods.First(method => method.Name == "NoteRngDouble");
		var game = exe.MainModule.Types.First(type => type.FullName == "Shooting.Game_Main");
		var main = game.Methods.First(method => method.Name == "MainProcess" && method.HasBody);
		var imported = exe.MainModule.ImportReference(after);
		HookBegin(main, exe.MainModule.ImportReference(begin));
		HookRandom(exe, exe.MainModule.ImportReference(note), exe.MainModule.ImportReference(noteDouble));
		var inserted = false;
		for (var i = 0; i < main.Body.Instructions.Count; i++)
		{
			var instruction = main.Body.Instructions[i];
			if (instruction.OpCode != OpCodes.Call && instruction.OpCode != OpCodes.Callvirt)
			{
				continue;
			}
			var target = instruction.Operand as MethodReference;
			if (target == null || target.Name != "UpdateData")
			{
				continue;
			}
			var processor = main.Body.GetILProcessor();
			processor.InsertAfter(instruction, processor.Create(OpCodes.Call, imported));
			processor.InsertAfter(instruction, processor.Create(OpCodes.Ldarg_0));
			inserted = true;
			break;
		}
		if (!inserted)
		{
			Console.Error.WriteLine("MainProcess has no UpdateData call");
			return 1;
		}
		exe.Write(args[2]);
		Console.WriteLine("rewrote " + args[2]);
		return 0;
	}

	static void HookBegin(MethodDefinition main, MethodReference begin)
	{
		var processor = main.Body.GetILProcessor();
		var first = main.Body.Instructions[0];
		processor.InsertBefore(first, processor.Create(OpCodes.Ldarg_0));
		processor.InsertBefore(first, processor.Create(OpCodes.Call, begin));
		if (main.Body.MaxStackSize < 8)
		{
			main.Body.MaxStackSize = 8;
		}
	}

	static void HookRandom(AssemblyDefinition exe, MethodReference note, MethodReference noteDouble)
	{
		var random = exe.MainModule.Types.First(type => type.FullName == "Shooting.MyRandom");
		var hooked = 0;
		foreach (var method in random.Methods)
		{
			if (!method.HasBody || method.Name != "Next" && method.Name != "NextDouble")
			{
				continue;
			}
			if (method.Name == "NextDouble")
			{
				HookReturns(method, noteDouble, -1);
			}
			else
			{
				HookReturns(method, note, method.Parameters.Count);
			}
			hooked++;
		}
		if (hooked < 4)
		{
			throw new InvalidOperationException("MyRandom hooks = " + hooked);
		}
	}

	static void HookReturns(MethodDefinition method, MethodReference note, int argc)
	{
		var processor = method.Body.GetILProcessor();
		var returns = method.Body.Instructions.Where(instruction => instruction.OpCode == OpCodes.Ret).ToList();
		foreach (var ret in returns)
		{
			processor.InsertBefore(ret, processor.Create(OpCodes.Dup));
			if (argc >= 0)
			{
				processor.InsertBefore(ret, processor.Create(OpCodes.Ldc_I4, argc == 0 ? 0 : argc == 1 ? 1 : 2));
				processor.InsertBefore(ret, argc >= 1 ? processor.Create(OpCodes.Ldarg, method.Parameters[0]) : processor.Create(OpCodes.Ldc_I4_0));
				processor.InsertBefore(ret, argc >= 2 ? processor.Create(OpCodes.Ldarg, method.Parameters[1]) : processor.Create(OpCodes.Ldc_I4_0));
			}
			processor.InsertBefore(ret, processor.Create(OpCodes.Call, note));
		}
		method.Body.MaxStackSize += 8;
	}
}
