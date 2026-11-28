$ErrorActionPreference='Stop'
Add-Type -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
public static class ClosedLauncherStackProof {
 [StructLayout(LayoutKind.Sequential)] struct ModuleInfo { public IntPtr Base;public uint Size;public IntPtr Entry; }
 [DllImport("kernel32.dll",SetLastError=true)] static extern IntPtr OpenProcess(uint a,bool i,uint p);
 [DllImport("kernel32.dll",SetLastError=true)] static extern IntPtr OpenThread(uint a,bool i,uint t);
 [DllImport("kernel32.dll",SetLastError=true)] static extern uint SuspendThread(IntPtr t);
 [DllImport("kernel32.dll",SetLastError=true)] static extern uint ResumeThread(IntPtr t);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool GetThreadContext(IntPtr t,IntPtr c);
 [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool GetExitCodeProcess(IntPtr p,out uint c);
 [DllImport("kernel32.dll",SetLastError=true)] static extern uint WaitForSingleObject(IntPtr h,uint ms);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool GetProcessTimes(IntPtr p,out long c,out long e,out long k,out long u);
 [DllImport("psapi.dll",SetLastError=true)] static extern bool EnumProcessModulesEx(IntPtr p,[Out]IntPtr[] mods,int bytes,out int need,uint filter);
 [DllImport("psapi.dll",SetLastError=true,CharSet=CharSet.Unicode)] static extern uint GetModuleFileNameEx(IntPtr p,IntPtr m,StringBuilder name,int size);
 [DllImport("psapi.dll",SetLastError=true)] static extern bool GetModuleInformation(IntPtr p,IntPtr m,out ModuleInfo info,int size);
 [DllImport("kernel32.dll",SetLastError=true,CharSet=CharSet.Unicode)] static extern bool QueryFullProcessImageName(IntPtr p,uint f,StringBuilder b,ref int n);
 [DllImport("dbghelp.dll",SetLastError=true,CharSet=CharSet.Ansi)] static extern bool SymInitialize(IntPtr p,string path,bool invade);
 [DllImport("dbghelp.dll")] static extern bool SymCleanup(IntPtr p);
 [DllImport("dbghelp.dll")] static extern uint SymSetOptions(uint options);
 [DllImport("dbghelp.dll",SetLastError=true)] static extern bool StackWalk64(uint machine,IntPtr p,IntPtr t,IntPtr frame,IntPtr context,IntPtr read,IntPtr table,IntPtr module,IntPtr translate);
 [DllImport("dbghelp.dll",SetLastError=true,CharSet=CharSet.Ansi)] static extern bool SymFromAddr(IntPtr p,ulong addr,out ulong displacement,IntPtr info);
 public static object Read(uint pid,uint tid) {
  var p=OpenProcess(0x100410,false,pid);
  if(p==IntPtr.Zero)return new{pid,open=false,error=Marshal.GetLastWin32Error()};
  try {
   var image=new StringBuilder(32768);int size=image.Capacity;
   if(!QueryFullProcessImageName(p,0,image,ref size)||!image.ToString().Equals(@"D:\workspace\eagler\artifacts\native-launcher-85941f0d56\hover-detail-perf\preview\touhouonline.exe",StringComparison.OrdinalIgnoreCase))throw new Exception("Unexpected process image");
   uint exit;GetExitCodeProcess(p,out exit);var wait=WaitForSingleObject(p,0);
   long created,exited,kernel,user;GetProcessTimes(p,out created,out exited,out kernel,out user);
   var mods=new List<(ulong start,ulong end,string path)>();var handles=new IntPtr[1024];int needed;
   if(EnumProcessModulesEx(p,handles,handles.Length*IntPtr.Size,out needed,3)) {
    for(int i=0;i<Math.Min(handles.Length,needed/IntPtr.Size);i++) {
     ModuleInfo info;var name=new StringBuilder(32768);
     if(GetModuleInformation(p,handles[i],out info,Marshal.SizeOf<ModuleInfo>())&&GetModuleFileNameEx(p,handles[i],name,name.Capacity)>0)mods.Add(((ulong)info.Base.ToInt64(),(ulong)info.Base.ToInt64()+info.Size,name.ToString()));
    }
   }
   var thread=OpenThread(0x4a,false,tid);
   if(thread==IntPtr.Zero)return new{pid,tid,threadOpen=false,error=Marshal.GetLastWin32Error()};
   SymSetOptions(0x81206);if(!SymInitialize(p,"",true))throw new Exception("SymInitialize failed: "+Marshal.GetLastWin32Error());var dbg=NativeLibrary.Load("dbghelp.dll");var fn=NativeLibrary.GetExport(dbg,"SymFunctionTableAccess64");var mb=NativeLibrary.GetExport(dbg,"SymGetModuleBase64");var addresses=new List<ulong>();var frame=Marshal.AllocHGlobal(512);Marshal.Copy(new byte[512],0,frame,512);ulong rip=0;var raw=Marshal.AllocHGlobal(1248);var context=new IntPtr((raw.ToInt64()+15)&~15L);bool suspended=false;
   try {
    Marshal.Copy(new byte[1232],0,context,1232);Marshal.WriteInt32(context,48,0x100003);
    var previous=SuspendThread(thread);
    if(previous==uint.MaxValue)throw new Exception("SuspendThread failed: "+Marshal.GetLastWin32Error());
    suspended=true;
    if(!GetThreadContext(thread,context))throw new Exception("GetThreadContext failed: "+Marshal.GetLastWin32Error());
        rip=(ulong)Marshal.ReadInt64(context,248);addresses.Add(rip);
    Marshal.WriteInt64(frame,0,(long)rip);Marshal.WriteInt32(frame,12,3);
    Marshal.WriteInt64(frame,32,Marshal.ReadInt64(context,160));Marshal.WriteInt32(frame,44,3);
    Marshal.WriteInt64(frame,48,Marshal.ReadInt64(context,152));Marshal.WriteInt32(frame,60,3);
    for(int i=0;i<72;i++) {
     if(!StackWalk64(0x8664,p,thread,frame,context,IntPtr.Zero,fn,mb,IntPtr.Zero))break;
     var pc=(ulong)Marshal.ReadInt64(frame,0);if(pc==0)break;addresses.Add(pc);
    }
   } finally {if(suspended)ResumeThread(thread);Marshal.FreeHGlobal(raw);CloseHandle(thread);}
      var frames=new List<object>();
   foreach(var addr in addresses) {
    string containing="unmapped";ulong delta=0;foreach(var m in mods)if(addr>=m.start&&addr<m.end){containing=m.path;delta=addr-m.start;break;}
    var symbol=Marshal.AllocHGlobal(1112);string label="";ulong disp=0;
    try{Marshal.Copy(new byte[1112],0,symbol,1112);Marshal.WriteInt32(symbol,0,88);Marshal.WriteInt32(symbol,80,1024);if(SymFromAddr(p,addr,out disp,symbol)){var count=Marshal.ReadInt32(symbol,76);label=Marshal.PtrToStringAnsi(IntPtr.Add(symbol,84),Math.Min(count,1023));}}
    finally{Marshal.FreeHGlobal(symbol);}
    frames.Add(new{address=addr.ToString("x"),module=containing,offset=delta.ToString("x"),export=label,exportOffset=disp.ToString("x")});
   }
   Marshal.FreeHGlobal(frame);SymCleanup(p);NativeLibrary.Free(dbg);
   string module="unmapped";ulong offset=0;
   foreach(var m in mods)if(rip>=m.start&&rip<m.end){module=m.path;offset=rip-m.start;break;}
   return new{pid,tid,exitCode=exit,waitResult=wait,created,exited,kernel100ns=kernel,user100ns=user,stackFrames=frames,modules=mods.Count,rip=rip.ToString("x"),module,offset=offset.ToString("x")};
  } finally {CloseHandle(p);}
 }
}
'@
$proofRecords=@(for($sample=0;$sample -lt 6;$sample++) {
 [ClosedLauncherStackProof]::Read(39464,34564)
 Start-Sleep -Milliseconds 200
})
$proofRecords | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath 'C:/Users/Ritosa/AppData/Local/Temp/native-process-proof-20261128/live-stack-full.json' -Encoding utf8
$proofRecords | ConvertTo-Json -Depth 4

