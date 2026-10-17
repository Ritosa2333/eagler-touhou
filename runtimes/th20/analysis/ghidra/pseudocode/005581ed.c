/* Entry: 0x005581ed; symbol: ___acrt_LCIDToLocaleName@16; body bytes: 63 */

/* Library Function - Single Match
    ___acrt_LCIDToLocaleName@16
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall
___acrt_LCIDToLocaleName_16(void *this,int param_1,short *param_2,int param_3,undefined4 param_4)

{
  FARPROC pFVar1;
  
  pFVar1 = FUN_00557dd2(this);
  if (pFVar1 == (FARPROC)0x0) {
    FUN_005625c9(param_1,param_2,param_3);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1,param_2,param_3,param_4);
    (*pFVar1)();
  }
  return;
}

