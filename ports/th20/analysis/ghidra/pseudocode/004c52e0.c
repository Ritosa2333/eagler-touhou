/* Entry: 0x004c52e0; symbol: FUN_004c52e0; body bytes: 87 */

void FUN_004c52e0(void)

{
  uint uVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00569acd;
  local_10 = *unaff_FS_OFFSET;
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004c50c0();
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(uVar1 ^ (uint)&stack0xfffffffc);
  return;
}

