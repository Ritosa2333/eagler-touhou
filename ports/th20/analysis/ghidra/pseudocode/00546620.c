/* Entry: 0x00546620; symbol: __except_handler4; body bytes: 350 */

/* Library Function - Single Match
    __except_handler4
   
   Library: Visual Studio 2019 Release */

undefined4 __cdecl __except_handler4(PEXCEPTION_RECORD param_1,PVOID param_2,int param_3)

{
  uint uVar1;
  code *pcVar2;
  undefined *puVar3;
  DWORD DVar4;
  int iVar5;
  BOOL BVar6;
  undefined4 uVar7;
  int iVar8;
  uint uVar9;
  PEXCEPTION_RECORD local_20;
  int local_1c;
  int *local_18;
  int local_14;
  undefined4 local_10;
  int *local_c;
  char local_5;
  
  local_5 = '\0';
  local_10 = 1;
  DVar4 = FUN_0056746f(param_1->ExceptionCode);
  param_1->ExceptionCode = DVar4;
  iVar8 = (int)param_2 + 0x10;
  local_c = (int *)(*(uint *)((int)param_2 + 8) ^ DAT_005b25c0);
  local_14 = iVar8;
  _ValidateLocalCookies(local_c,iVar8);
  FUN_0054769c(param_3);
  uVar9 = *(uint *)((int)param_2 + 0xc);
  if ((param_1->ExceptionFlags & 0x66) == 0) {
    local_20 = param_1;
    local_1c = param_3;
    *(PEXCEPTION_RECORD **)((int)param_2 + -4) = &local_20;
    if (uVar9 == 0xfffffffe) {
      return local_10;
    }
    do {
      iVar5 = uVar9 * 3 + 4;
      uVar1 = local_c[iVar5];
      local_18 = local_c + iVar5;
      if ((undefined *)local_18[1] != (undefined *)0x0) {
        iVar5 = _EH4_CallFilterFunc((undefined *)local_18[1]);
        local_5 = '\x01';
        if (iVar5 < 0) {
          local_10 = 0;
          goto LAB_005466e4;
        }
        if (0 < iVar5) {
          if (((param_1->ExceptionCode == 0xe06d7363) && (PTR_FUN_00597180 != (undefined *)0x0)) &&
             (BVar6 = __IsNonwritableInCurrentImage((PBYTE)&PTR_FUN_00597180),
             puVar3 = PTR_FUN_00597180, BVar6 != 0)) {
            (*(code *)PTR_guard_check_icall_0056c37c)(param_1,1);
            (*(code *)puVar3)();
            iVar8 = local_14;
          }
          _EH4_GlobalUnwind2(param_2,param_1);
          if (*(uint *)((int)param_2 + 0xc) != uVar9) {
            _EH4_LocalUnwind((int)param_2,uVar9,iVar8,&DAT_005b25c0);
          }
          *(uint *)((int)param_2 + 0xc) = uVar1;
          _ValidateLocalCookies(local_c,iVar8);
          _EH4_TransferToHandler((undefined *)local_18[2]);
          pcVar2 = (code *)swi(3);
          uVar7 = (*pcVar2)();
          return uVar7;
        }
      }
      uVar9 = uVar1;
    } while (uVar1 != 0xfffffffe);
    if (local_5 == '\0') {
      return local_10;
    }
  }
  else {
    if (uVar9 == 0xfffffffe) {
      return local_10;
    }
    _EH4_LocalUnwind((int)param_2,0xfffffffe,iVar8,&DAT_005b25c0);
  }
LAB_005466e4:
  _ValidateLocalCookies(local_c,iVar8);
  return local_10;
}

