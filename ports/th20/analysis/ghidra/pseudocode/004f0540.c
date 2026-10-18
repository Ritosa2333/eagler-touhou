/* Entry: 0x004f0540; symbol: FUN_004f0540; body bytes: 1207 */

void __cdecl
FUN_004f0540(undefined4 *param_1,undefined4 param_2,uint param_3,int param_4,undefined4 param_5,
            uint param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9)

{
  char cVar1;
  uint uVar2;
  undefined4 *puVar3;
  _Ptr_base<struct__EXCEPTION_RECORD_const_> *p_Var4;
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_168 [24];
  undefined1 local_150 [8];
  _Ptr_base<struct__EXCEPTION_RECORD_const_> local_148 [8];
  locale local_140 [8];
  int local_138;
  undefined4 local_134;
  uint local_130;
  undefined4 local_12c;
  undefined4 local_128;
  undefined4 local_120;
  undefined4 local_11c;
  undefined4 local_114;
  int local_110;
  uint local_10c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_108;
  void *local_104;
  locale *local_100;
  locale *local_fc;
  int local_f8;
  int local_f4;
  char *local_f0;
  uint local_ec;
  undefined4 local_e8;
  undefined4 local_e4;
  undefined4 local_e0;
  undefined8 local_dc;
  uint local_d4;
  int local_d0;
  char local_c9;
  char *local_c8;
  undefined1 local_c4 [40];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_9c [24];
  undefined4 local_84;
  undefined4 local_80;
  int local_7c;
  int local_78;
  char local_71;
  char local_70;
  char acStack_6f [75];
  uint local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  int local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0056a2fb;
  local_1c = *unaff_FS_OFFSET;
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_24 = uVar2;
  if ((char)param_6 == 'c') {
    puStack_20 = &stack0xfffffffc;
    cVar1 = FUN_004eca60(param_3);
    if (cVar1 == '\0') {
      func_0x004fcd20("integral cannot be stored in char",uVar2);
    }
    param_6 = param_6 & 0xffffff;
    local_e4 = param_9;
    local_138 = param_4;
    local_134 = param_5;
    local_130 = param_6;
    local_12c = param_7;
    local_128 = param_8;
    puVar3 = (undefined4 *)move<>(&param_2);
    local_e8 = *puVar3;
    local_e0 = local_e8;
    FUN_004e9710(param_1,local_e8,(byte)param_3,local_138,local_134,local_130,local_12c,local_128,
                 local_e4);
  }
  else {
    if (param_6._2_1_ == '\0') {
      param_6._0_3_ = CONCAT12(2,(undefined2)param_6);
    }
    local_d4 = 10;
    local_d0 = (char)param_6 + -0x42;
    switch((char)param_6) {
    case 'B':
    case 'b':
      local_d4 = 2;
      break;
    case 'X':
    case 'x':
      local_d4 = 0x10;
      break;
    case 'o':
      local_d4 = 8;
    }
    local_ec = param_3;
    local_f0 = (char *)FUN_004f3330((int)&local_70);
    local_dc = FUN_00459210(&local_70,local_f0,local_ec,local_d4);
    local_c8 = &local_70;
    local_78 = (int)(char *)local_dc - (int)local_c8;
    if ((int)param_3 < 0) {
      local_c8 = acStack_6f;
    }
    else if (param_6._2_1_ != '\x02') {
      local_78 = local_78 + 1;
    }
    if ((char)param_6 == 'X') {
      FUN_004fb8c0(local_c8,(char *)local_dc);
    }
    FUN_0040c080(&local_84,8);
    std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
              ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)&local_84);
    if (param_6._3_1_ != '\0') {
      p_Var4 = FUN_004ec6c0(local_148,(char)param_6,param_3);
      local_84 = *(undefined4 *)p_Var4;
      local_80 = *(undefined4 *)(p_Var4 + 4);
      local_f8 = FUN_0040ff90((int)&local_84);
      local_f4 = local_78;
      local_78 = local_78 + local_f8;
    }
    local_7c = 0;
    FUN_0040dc60(local_9c);
    local_14 = 0;
    if ((char)param_7 != '\0') {
      local_100 = FUN_004fbf20(&param_9,local_140);
      local_14._0_1_ = 1;
      local_fc = local_100;
      local_104 = (void *)FUN_004f3910(local_100);
      local_108 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   *)FUN_004ff7a0(local_104,local_168);
      FUN_00418eb0(local_9c,local_108);
      FUN_0040e5a0(local_168);
      local_14 = (uint)local_14._1_3_ << 8;
      FUN_004f5600((int)local_140);
      puVar3 = (undefined4 *)FUN_004494f0(local_9c,local_150);
      local_120 = *puVar3;
      local_11c = puVar3[1];
      local_10c = (int)(char *)local_dc - (int)local_c8;
      local_7c = FUN_004fb9c0(local_10c);
      local_78 = local_78 + local_7c;
    }
    if ((param_7._1_1_ == '\0') || (param_6._1_1_ != '\0')) {
      local_c9 = '\0';
    }
    else {
      local_c9 = '\x01';
    }
    local_71 = local_c9;
    FUN_0040c080(local_c4,0x28);
    FUN_004f3d00(local_c4,(char *)local_dc,&param_4,&param_3,&local_84,&local_71,&local_78,&local_7c
                 ,&local_c8,local_9c,&param_9);
    if (local_71 == '\0') {
      local_110 = local_78;
      puVar3 = (undefined4 *)move<>(&param_2);
      local_114 = *puVar3;
      FUN_004ef940(param_1,local_114,local_110,&param_4,'\x02',local_c4);
      local_14 = 0xffffffff;
      FUN_0040e5a0(local_9c);
    }
    else {
      puVar3 = (undefined4 *)move<>(&param_2);
      FUN_004f6300(local_c4,param_1,*puVar3);
      local_14 = 0xffffffff;
      FUN_0040e5a0(local_9c);
    }
  }
  *unaff_FS_OFFSET = local_1c;
  FUN_005429ee(local_24 ^ (uint)&stack0xfffffff0);
  return;
}

