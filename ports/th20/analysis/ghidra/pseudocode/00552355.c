/* Entry: 0x00552355; symbol: rand_s; body bytes: 72 */

/* Library Function - Single Match
    _rand_s
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 __cdecl rand_s(undefined4 *param_1)

{
  char cVar1;
  __acrt_ptd *p_Var2;
  void *in_ECX;
  
  if (param_1 == (undefined4 *)0x0) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
    return 0x16;
  }
  *param_1 = 0;
  cVar1 = FUN_00558341(in_ECX,param_1,4);
  if (cVar1 == '\0') {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0xc;
    p_Var2 = FUN_005516c1();
    return *(undefined4 *)p_Var2;
  }
  return 0;
}

