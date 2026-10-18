/* Entry: 0x005407e6; symbol: FUN_005407e6; body bytes: 87 */

void FUN_005407e6(uint param_1)

{
  code *pcVar1;
  uint uVar2;
  uint uVar3;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar3 = FUN_005406cc();
  pcVar1 = DAT_005e4b88;
  if (uVar3 < 3) {
    FUN_00540842(param_1);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1,uVar2);
    (*pcVar1)();
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

