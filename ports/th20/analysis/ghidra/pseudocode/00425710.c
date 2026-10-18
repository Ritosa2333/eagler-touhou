/* Entry: 0x00425710; symbol: FUN_00425710; body bytes: 155 */

void __cdecl FUN_00425710(undefined4 param_1,int param_2,undefined4 param_3)

{
  int *unaff_FS_OFFSET;
  undefined4 local_20 [3];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056818d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040c080(local_20,0xc);
  FUN_00414060(local_20,param_1,param_3);
  local_8 = 0;
  for (; param_2 != 0; param_2 = param_2 + -1) {
    FUN_00425270((int)local_20);
  }
  FUN_00414b70(local_20);
  local_8 = 0xffffffff;
  FUN_004143d0(local_20);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

