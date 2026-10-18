/* Entry: 0x00464100; symbol: FUN_00464100; body bytes: 208 */

void __fastcall FUN_00464100(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056808d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  FUN_00463fb0(param_1);
  iVar2 = FUN_004640a0(0x5ba568);
  if (iVar2 == 4) {
    FUN_004640e0(param_1);
  }
  else {
    FUN_004640e0(param_1);
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

