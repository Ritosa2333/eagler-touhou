/* Entry: 0x0044fff0; symbol: FUN_0044fff0; body bytes: 276 */

undefined8 *
FUN_0044fff0(undefined8 *param_1,undefined4 param_2,undefined4 param_3,int param_4,
            undefined4 param_5,undefined4 param_6,LPCSTR param_7,int param_8,int param_9,
            int param_10,int *param_11)

{
  int *unaff_FS_OFFSET;
  undefined1 auStack_58 [36];
  undefined4 uStack_34;
  uint uStack_30;
  undefined8 local_2c;
  undefined8 local_24;
  undefined1 *local_1c;
  undefined1 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_005686fd;
  local_10 = *unaff_FS_OFFSET;
  uStack_30 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  if (param_8 == 0) {
    local_18 = auStack_58;
    function<>(auStack_58,(int)&stack0x00000034);
    local_24 = FUN_00415830(param_11,param_4,param_5,param_6,param_7,param_2,param_9,param_10,1);
    *param_1 = local_24;
    local_8 = 0xffffffff;
    uStack_34 = 0x450087;
    FUN_00414400(&stack0x00000034);
  }
  else {
    local_1c = auStack_58;
    function<>(auStack_58,(int)&stack0x00000034);
    local_2c = FUN_00415830(param_11,param_4,param_5,0,param_7,param_2,param_9,param_10,0);
    *param_1 = local_2c;
    local_8 = 0xffffffff;
    uStack_34 = 0x4500ef;
    FUN_00414400(&stack0x00000034);
  }
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

