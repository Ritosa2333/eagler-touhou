/* Entry: 0x005408a2; symbol: FUN_005408a2; body bytes: 112 */

void FUN_005408a2(uint param_1,undefined4 param_2,undefined4 param_3,DWORD param_4)

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
  pcVar1 = DAT_005e4b80;
  if (uVar3 < 3) {
    FUN_00540917(param_1,param_2,param_3,0,FUN_005406df,param_4);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1,param_2,param_3,param_4,uVar2);
    (*pcVar1)();
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

