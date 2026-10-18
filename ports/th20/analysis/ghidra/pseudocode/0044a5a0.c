/* Entry: 0x0044a5a0; symbol: FUN_0044a5a0; body bytes: 154 */

int __fastcall FUN_0044a5a0(int param_1)

{
  int iVar1;
  int *unaff_FS_OFFSET;
  undefined1 auStack_54 [36];
  undefined4 uStack_30;
  uint uStack_2c;
  undefined1 *local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_0056847d;
  local_10 = *unaff_FS_OFFSET;
  uStack_2c = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  uStack_30 = 0x44a5d7;
  local_14 = param_1;
  iVar1 = FUN_0040ff90(param_1);
  if (iVar1 == 0) {
    local_18 = 0;
  }
  else {
    uStack_30 = 0x44a5e3;
    local_1c = FUN_0040ff90(local_14);
    local_28 = auStack_54;
    function<>(auStack_54,(int)&stack0x00000004);
    local_20 = FUN_0044a500(local_1c);
    local_18 = local_20;
  }
  local_24 = local_18;
  local_8 = 0xffffffff;
  uStack_30 = 0x44a626;
  FUN_00414400(&stack0x00000004);
  *unaff_FS_OFFSET = local_10;
  return local_24;
}

