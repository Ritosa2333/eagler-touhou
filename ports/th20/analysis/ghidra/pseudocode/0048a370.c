/* Entry: 0x0048a370; symbol: FUN_0048a370; body bytes: 213 */

void __cdecl
FUN_0048a370(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  int iVar2;
  int *unaff_FS_OFFSET;
  int local_24;
  undefined4 local_20 [3];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056943d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  puVar1 = (undefined4 *)move<>(&param_1);
  local_24 = FUN_0040b2a0(puVar1);
  puVar1 = (undefined4 *)move<>(&param_2);
  iVar2 = FUN_0040b2a0(puVar1);
  FUN_0040c080(local_20,0xc);
  FUN_00414060(local_20,param_3,param_4);
  local_8 = 0;
  for (; local_24 != iVar2; local_24 = local_24 + 0x2c) {
    FUN_0047acb0(local_20,local_24);
  }
  FUN_00414b70(local_20);
  local_8 = 0xffffffff;
  FUN_004143d0(local_20);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

