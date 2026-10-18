/* Entry: 0x0041f220; symbol: FID_conflict:_swprintf; body bytes: 55 */

/* Library Function - Multiple Matches With Different Base Names
    _sprintf_s
    _swprintf
    _swprintf_s
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

int __cdecl FID_conflict__swprintf(wchar_t *_Dest,wchar_t *_Format,...)

{
  int iVar1;
  wchar_t *in_stack_0000000c;
  
  iVar1 = __vswprintf_s_l(_Dest,(size_t)_Format,in_stack_0000000c,(_locale_t)0x0,&stack0x00000010);
  return iVar1;
}

