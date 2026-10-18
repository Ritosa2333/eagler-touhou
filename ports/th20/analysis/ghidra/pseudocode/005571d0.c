/* Entry: 0x005571d0; symbol: __set_new_mode; body bytes: 47 */

/* Library Function - Single Match
    __set_new_mode
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 __cdecl __set_new_mode(int param_1)

{
  undefined4 uVar1;
  __acrt_ptd *p_Var2;
  
  uVar1 = DAT_005e55e0;
  if ((param_1 != 0) && (param_1 != 1)) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
    return 0xffffffff;
  }
  LOCK();
  DAT_005e55e0 = param_1;
  UNLOCK();
  return uVar1;
}

