/* Entry: 0x004e9c70; symbol: FUN_004e9c70; body bytes: 737 */

undefined4 * __cdecl
FUN_004e9c70(undefined4 *param_1,undefined4 param_2,byte param_3,int param_4,undefined4 param_5,
            uint param_6,uint param_7,undefined4 param_8,undefined4 param_9)

{
  undefined4 *puVar1;
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_f0 [24];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_d8 [24];
  undefined1 local_c0 [8];
  undefined1 local_b8 [8];
  locale local_b0 [8];
  int local_a8;
  undefined4 local_a4;
  uint local_a0;
  uint local_9c;
  undefined4 local_98;
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  void *local_68;
  void *local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  undefined4 local_50;
  locale *local_4c;
  locale *local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  char *local_2c;
  void *local_28;
  uint local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  undefined4 local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0056a1a8;
  local_1c = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_24 = 0;
  if (((char)param_6 == '\0') || ((char)param_6 == 's')) {
    if ((char)param_7 == '\0') {
      local_78 = param_9;
      if (param_3 == 0) {
        local_2c = "false";
      }
      else {
        local_2c = "true";
      }
      puVar1 = (undefined4 *)move<>(&param_2);
      local_7c = *puVar1;
      FUN_004e9880(param_1,local_7c,local_2c,&param_4,local_78);
    }
    else {
      param_7 = param_7 & 0xffffff00;
      puStack_20 = &stack0xfffffffc;
      local_4c = FUN_004fbf20(&param_9,local_b0);
      local_14 = 0;
      local_48 = local_4c;
      local_28 = (void *)FUN_004f3910(local_4c);
      local_14 = 0xffffffff;
      FUN_004f5600((int)local_b0);
      local_50 = param_9;
      local_70 = param_9;
      if (param_3 == 0) {
        local_68 = (void *)FUN_004ff020(local_28,local_d8);
        local_14 = 2;
        local_24 = local_24 | 2;
        local_64 = local_68;
        local_60 = local_68;
        puVar1 = (undefined4 *)FUN_004494f0(local_68,local_c0);
        local_38 = *puVar1;
        local_34 = puVar1[1];
      }
      else {
        local_5c = (void *)FUN_00500230(local_28,local_f0);
        local_14 = 1;
        local_24 = local_24 | 1;
        local_58 = local_5c;
        local_54 = local_5c;
        puVar1 = (undefined4 *)FUN_004494f0(local_5c,local_b8);
        local_38 = *puVar1;
        local_34 = puVar1[1];
      }
      local_88 = local_38;
      local_84 = local_34;
      local_90 = local_38;
      local_8c = local_34;
      puVar1 = (undefined4 *)move<>(&param_2);
      local_74 = *puVar1;
      local_6c = local_74;
      FUN_004e9b30(param_1,local_74,local_90,local_8c,&param_4);
      local_14 = 1;
      if ((local_24 & 2) != 0) {
        local_24 = local_24 & 0xfffffffd;
        FUN_0040e5a0(local_d8);
      }
      local_14 = 0xffffffff;
      if ((local_24 & 1) != 0) {
        local_24 = local_24 & 0xfffffffe;
        FUN_0040e5a0(local_f0);
      }
    }
  }
  else {
    local_40 = param_9;
    local_a8 = param_4;
    local_a4 = param_5;
    local_a0 = param_6;
    local_9c = param_7;
    local_98 = param_8;
    puStack_20 = &stack0xfffffffc;
    puVar1 = (undefined4 *)move<>(&param_2);
    local_44 = *puVar1;
    local_3c = local_44;
    FUN_004f0020(param_1,local_44,param_3,local_a8,local_a4,local_a0,local_9c,local_98,local_40);
  }
  *unaff_FS_OFFSET = local_1c;
  return param_1;
}

