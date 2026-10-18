/* Entry: 0x00447480; symbol: FUN_00447480; body bytes: 158 */

void FUN_00447480(undefined4 param_1,uint param_2)

{
  undefined4 uVar1;
  uint uVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005676ed;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,1);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  uVar2 = (uint)((ulonglong)param_2 * 4);
  if ((int)((ulonglong)param_2 * 4 >> 0x20) != 0) {
    uVar2 = 0xffffffff;
  }
  FUN_00542a01(uVar2);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

