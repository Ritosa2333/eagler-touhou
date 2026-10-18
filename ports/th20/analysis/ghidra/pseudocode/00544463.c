/* Entry: 0x00544463; symbol: FUN_00544463; body bytes: 255 */

undefined4 __cdecl
FUN_00544463(int *param_1,undefined4 *param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,undefined4 param_6,undefined4 param_7)

{
  int iVar1;
  int *unaff_FS_OFFSET;
  undefined4 *local_40;
  code *local_3c;
  uint local_38;
  undefined4 local_34;
  undefined4 *local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined1 *local_24;
  undefined1 *local_20;
  int local_1c;
  int *local_18;
  undefined4 local_14;
  code *local_10;
  undefined4 local_c;
  code *local_8;
  
  local_20 = &stack0xfffffffc;
  local_24 = &stack0xffffffbc;
  if (param_1 == (int *)0x123) {
    *param_2 = 0x544536;
    local_c = 1;
  }
  else {
    local_3c = FID_conflict_TranslatorGuardHandler;
    local_38 = DAT_005b25c0 ^ (uint)&local_40;
    local_34 = param_5;
    local_30 = param_2;
    local_2c = param_6;
    local_28 = param_7;
    local_1c = 0;
    local_40 = (undefined4 *)*unaff_FS_OFFSET;
    *unaff_FS_OFFSET = (int)&local_40;
    iVar1 = FUN_0056746f(*param_1);
    *param_1 = iVar1;
    local_c = 1;
    local_18 = param_1;
    local_14 = param_3;
    iVar1 = ___vcrt_getptd();
    local_8 = *(code **)(iVar1 + 8);
    guard_check_icall();
    local_10 = local_8;
    (*local_8)(*param_1,&local_18);
    local_c = 0;
    if (local_1c == 0) {
      *unaff_FS_OFFSET = (int)local_40;
    }
    else {
      *local_40 = *(undefined4 *)*unaff_FS_OFFSET;
      *unaff_FS_OFFSET = (int)local_40;
    }
  }
  return local_c;
}

