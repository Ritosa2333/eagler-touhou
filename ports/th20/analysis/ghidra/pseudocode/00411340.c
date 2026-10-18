/* Entry: 0x00411340; symbol: FUN_00411340; body bytes: 204 */

void FUN_00411340(void *param_1,ulong param_2)

{
  int *unaff_FS_OFFSET;
  uint *local_1c;
  uint local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567b4d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_System_error_message::_System_error_message((_System_error_message *)&local_1c,param_2);
  local_8 = 0;
  if ((local_1c == (uint *)0x0) || (local_18 == 0)) {
    FUN_0040dbf0(param_1,(uint *)"unknown error",0xd);
    local_8 = 0xffffffff;
    FUN_0040e610(&local_1c);
  }
  else {
    FUN_0040dbf0(param_1,local_1c,local_18);
    local_8 = 0xffffffff;
    FUN_0040e610(&local_1c);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

