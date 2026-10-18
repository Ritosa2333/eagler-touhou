/* Entry: 0x0052c320; symbol: FUN_0052c320; body bytes: 184 */

void __fastcall FUN_0052c320(int param_1)

{
  undefined4 uVar1;
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
  uVar1 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  *(int *)(param_1 + 0x124280) = *(int *)(param_1 + 0x124280) + -1;
  FUN_00484980((void *)(param_1 + 0x124284),*(uint *)(param_1 + 0x124280));
  FUN_004beb40(param_1);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

