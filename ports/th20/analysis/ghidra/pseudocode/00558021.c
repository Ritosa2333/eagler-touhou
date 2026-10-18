/* Entry: 0x00558021; symbol: ___acrt_AreFileApisANSI@0; body bytes: 31 */

/* Library Function - Single Match
    ___acrt_AreFileApisANSI@0
   
   Library: Visual Studio 2019 Release */

int __fastcall ___acrt_AreFileApisANSI_0(void *param_1)

{
  FARPROC pFVar1;
  int iVar2;
  
  pFVar1 = FUN_00557c7b(param_1);
  if (pFVar1 != (FARPROC)0x0) {
    (*(code *)PTR_guard_check_icall_0056c37c)();
    iVar2 = (*pFVar1)();
    return iVar2;
  }
  return 1;
}

