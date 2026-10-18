/* Entry: 0x004598a0; symbol: FUN_004598a0; body bytes: 88 */

void __fastcall FUN_004598a0(int *param_1)

{
  uint uVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  if (*param_1 != 0) {
    (**(code **)(*(int *)*param_1 + 8))(*param_1,uVar1);
    *param_1 = 0;
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

