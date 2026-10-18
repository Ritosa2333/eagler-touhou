/* Entry: 0x004a21e0; symbol: FUN_004a21e0; body bytes: 203 */

void __cdecl
FUN_004a21e0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  int local_24;
  undefined4 local_20 [3];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567d6d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_24 = FUN_0040b2a0(&param_1);
  iVar1 = FUN_0040b2a0(&param_2);
  FUN_0040c080(local_20,0xc);
  FUN_00414060(local_20,param_3,param_4);
  local_8 = 0;
  for (; local_24 != iVar1; local_24 = local_24 + 0x88) {
    uVar2 = move<>(local_24);
    FUN_004a1a00(local_20,uVar2);
  }
  FUN_00414b70(local_20);
  local_8 = 0xffffffff;
  FUN_004143d0(local_20);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

