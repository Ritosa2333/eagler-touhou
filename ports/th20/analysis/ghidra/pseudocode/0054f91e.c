/* Entry: 0x0054f91e; symbol: set; body bytes: 48 */

/* WARNING: Removing unreachable block (ram,0x0054f93f) */
/* Library Function - Single Match
    public: void __thiscall __crt_stdio_input::scanset_buffer<wchar_t>::set(wchar_t)
   
   Libraries: Visual Studio 2015 Debug, Visual Studio 2015 Release */

void __thiscall
__crt_stdio_input::scanset_buffer<wchar_t>::set(scanset_buffer<wchar_t> *this,wchar_t param_1)

{
  int iVar1;
  byte *pbVar2;
  
  iVar1 = FUN_0054e9a0((int *)this);
  pbVar2 = (byte *)((uint)((ushort)param_1 >> 3) + iVar1);
  *pbVar2 = *pbVar2 | (byte)(1 << (param_1 & 7U));
  return;
}

