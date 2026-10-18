/* Entry: 0x0047b050; symbol: FUN_0047b050; body bytes: 196 */

void __cdecl FUN_0047b050(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  int *unaff_FS_OFFSET;
  int local_28;
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
  puVar1 = (undefined4 *)move<>(&param_1);
  local_28 = FUN_0040b2a0(puVar1);
  FUN_0040c080(local_20,0xc);
  FUN_00414060(local_20,param_3,param_4);
  local_8 = 0;
  for (; param_2 != 0; param_2 = param_2 + -1) {
    FUN_0047acb0(local_20,local_28);
    local_28 = local_28 + 0x2c;
  }
  FUN_00414b70(local_20);
  local_8 = 0xffffffff;
  FUN_004143d0(local_20);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

