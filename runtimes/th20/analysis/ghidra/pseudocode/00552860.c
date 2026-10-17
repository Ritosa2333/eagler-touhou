/* Entry: 0x00552860; symbol: FUN_00552860; body bytes: 44 */

bool __cdecl FUN_00552860(undefined4 param_1)

{
  code *pcVar1;
  int iVar2;
  bool bVar3;
  
  pcVar1 = (code *)FUN_005528c0();
  if (pcVar1 == (code *)0x0) {
    bVar3 = false;
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1);
    iVar2 = (*pcVar1)();
    bVar3 = iVar2 != 0;
  }
  return bVar3;
}

