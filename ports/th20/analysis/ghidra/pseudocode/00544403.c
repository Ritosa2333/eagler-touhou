/* Entry: 0x00544403; symbol: FUN_00544403; body bytes: 96 */

undefined4 __cdecl
FUN_00544403(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4,int param_5)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  int local_1c;
  code *local_18;
  uint local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  local_14 = (uint)&local_1c ^ DAT_005b25c0;
  local_10 = param_2;
  local_8 = param_4 + 1;
  local_18 = FUN_005445e6;
  local_c = param_1;
  local_1c = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_1c;
  uVar1 = __CallSettingFrame_12(param_3,param_1,param_5);
  *unaff_FS_OFFSET = local_1c;
  return uVar1;
}

