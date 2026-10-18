/* Entry: 0x00516250; symbol: FUN_00516250; body bytes: 102 */

void __fastcall FUN_00516250(int *param_1)

{
  uint uVar1;
  int *piVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  piVar2 = (int *)FUN_00438520(*param_1 + *(int *)(*(int *)*param_1 + 4));
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 8))(uVar1);
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

