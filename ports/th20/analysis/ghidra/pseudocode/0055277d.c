/* Entry: 0x0055277d; symbol: allocate; body bytes: 39 */

/* Library Function - Multiple Matches With Same Base Name
    public: int __thiscall __crt_win32_buffer<char,struct
   __crt_win32_buffer_no_resizing>::allocate(unsigned int)
    public: int __thiscall __crt_win32_buffer<wchar_t,struct
   __crt_win32_buffer_no_resizing>::allocate(unsigned int)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

undefined4 __fastcall allocate(int param_1)

{
  __acrt_ptd *p_Var1;
  
  if (*(char *)(param_1 + 0x14) != '\0') {
    *(undefined1 *)(param_1 + 0x14) = 0;
  }
  p_Var1 = FUN_005516c1();
  *(undefined4 *)p_Var1 = 0x22;
  *(undefined4 *)(param_1 + 0xc) = 0;
  *(undefined1 *)(param_1 + 0x14) = 0;
  return 0x22;
}

