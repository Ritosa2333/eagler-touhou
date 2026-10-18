/* Entry: 0x0055f5b0; symbol: __recalloc_base; body bytes: 109 */

/* Library Function - Single Match
    __recalloc_base
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

LPVOID __cdecl __recalloc_base(LPCVOID param_1,uint param_2,uint param_3)

{
  __acrt_ptd *p_Var1;
  LPVOID pvVar2;
  SIZE_T SVar3;
  uint uVar4;
  
  if ((param_2 == 0) || (param_3 <= 0xffffffe0 / param_2)) {
    if (param_1 == (LPCVOID)0x0) {
      SVar3 = 0;
    }
    else {
      SVar3 = __msize_base(param_1);
    }
    uVar4 = param_2 * param_3;
    pvVar2 = __realloc_base(param_1,uVar4);
    if ((pvVar2 != (LPVOID)0x0) && (SVar3 < uVar4)) {
      _memset((void *)((int)pvVar2 + SVar3),0,uVar4 - SVar3);
    }
  }
  else {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0xc;
    pvVar2 = (LPVOID)0x0;
  }
  return pvVar2;
}

