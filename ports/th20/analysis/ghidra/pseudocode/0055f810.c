/* Entry: 0x0055f810; symbol: __msize_base; body bytes: 51 */

/* Library Function - Single Match
    __msize_base
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

SIZE_T __cdecl __msize_base(LPCVOID param_1)

{
  __acrt_ptd *p_Var1;
  SIZE_T SVar2;
  
  if (param_1 == (LPCVOID)0x0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return 0xffffffff;
  }
  SVar2 = HeapSize(DAT_005e5990,0,param_1);
  return SVar2;
}

