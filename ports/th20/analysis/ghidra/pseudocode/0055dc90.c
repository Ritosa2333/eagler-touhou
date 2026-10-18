/* Entry: 0x0055dc90; symbol: __realloc_base; body bytes: 105 */

/* Library Function - Single Match
    __realloc_base
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

LPVOID __cdecl __realloc_base(LPVOID param_1,uint param_2)

{
  bool bVar1;
  LPVOID pvVar2;
  __acrt_ptd *p_Var3;
  int iVar4;
  undefined3 extraout_var;
  
  if (param_1 == (LPVOID)0x0) {
    pvVar2 = FUN_005584c0(param_2);
  }
  else {
    if (param_2 == 0) {
      FUN_00557ba0(param_1);
    }
    else {
      if (param_2 < 0xffffffe1) {
        do {
          pvVar2 = HeapReAlloc(DAT_005e5990,0,param_1,param_2);
          if (pvVar2 != (LPVOID)0x0) {
            return pvVar2;
          }
          iVar4 = FUN_005571c0();
        } while ((iVar4 != 0) && (bVar1 = FUN_00552860(param_2), CONCAT31(extraout_var,bVar1) != 0))
        ;
      }
      p_Var3 = FUN_005516c1();
      *(undefined4 *)p_Var3 = 0xc;
    }
    pvVar2 = (LPVOID)0x0;
  }
  return pvVar2;
}

