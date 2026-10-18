/* Entry: 0x00470180; symbol: FUN_00470180; body bytes: 153 */

void __fastcall FUN_00470180(int param_1)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  puStack_c = &LAB_00568e75;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x12);
  FUN_0040b730(&local_18,uVar1);
  local_8._0_1_ = 1;
  FUN_00470a20((void *)(param_1 + 0x1a234),&stack0x00000004);
  local_8 = (uint)local_8._1_3_ << 8;
  FUN_0040b900(&local_18);
  local_8 = 0xffffffff;
  FUN_00414400(&stack0x00000004);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

