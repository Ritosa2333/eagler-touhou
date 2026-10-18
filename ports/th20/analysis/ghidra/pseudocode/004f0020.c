/* Entry: 0x004f0020; symbol: FUN_004f0020; body bytes: 1236 */

/* WARNING: Removing unreachable block (ram,0x004f023b) */

void __cdecl
FUN_004f0020(undefined4 *param_1,undefined4 param_2,byte param_3,int param_4,undefined4 param_5,
            uint param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9)

{
  char cVar1;
  uint uVar2;
  undefined4 *puVar3;
  _Ptr_base<struct__EXCEPTION_RECORD_const_> *p_Var4;
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_164 [24];
  undefined1 local_14c [8];
  _Ptr_base<struct__EXCEPTION_RECORD_const_> local_144 [8];
  locale local_13c [8];
  locale *local_134;
  int local_130;
  int local_12c;
  int local_128;
  undefined4 local_124;
  uint local_120;
  undefined4 local_11c;
  undefined4 local_118;
  undefined4 local_110;
  undefined4 local_10c;
  locale *local_108;
  undefined4 local_104;
  int local_100;
  uint local_fc;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_f8;
  void *local_f4;
  char *local_f0;
  undefined4 local_ec;
  undefined4 local_e8;
  undefined4 local_e4;
  undefined4 local_e0;
  undefined8 local_dc;
  int local_d4;
  int local_d0;
  byte local_cb;
  byte local_ca;
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
  char local_70 [76];
  uint local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  int local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0056a2ab;
  local_1c = *unaff_FS_OFFSET;
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_24 = uVar2;
  if ((char)param_6 == 'c') {
    puStack_20 = &stack0xfffffffc;
    cVar1 = FUN_004eca40(param_3);
    if (cVar1 == '\0') {
      func_0x004fcd20("integral cannot be stored in char",uVar2);
    }
    param_6 = param_6 & 0xffffff;
    local_e8 = param_9;
    local_128 = param_4;
    local_124 = param_5;
    local_120 = param_6;
    local_11c = param_7;
    local_118 = param_8;
    local_ca = param_3;
    puVar3 = (undefined4 *)move<>(&param_2);
    local_ec = *puVar3;
    local_e4 = local_ec;
    local_e0 = local_ec;
    FUN_004e9710(param_1,local_ec,local_ca,local_128,local_124,local_120,local_11c,local_118,
                 local_e8);
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
    local_cb = param_3;
    local_f0 = (char *)FUN_004f3330((int)local_70);
    local_dc = FUN_00500080(local_70,local_f0,local_cb,local_d4);
    local_c8 = local_70;
    local_78 = (int)(char *)local_dc - (int)local_c8;
    if (param_6._2_1_ != '\x02') {
      local_78 = local_78 + 1;
    }
    if ((char)param_6 == 'X') {
      FUN_004fb8c0(local_c8,(char *)local_dc);
    }
    FUN_0040c080(&local_84,8);
    std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
              ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)&local_84);
    if (param_6._3_1_ != '\0') {
      p_Var4 = FUN_004ec5a0(local_144,(char)param_6,param_3);
      local_84 = *(undefined4 *)p_Var4;
      local_80 = *(undefined4 *)(p_Var4 + 4);
      local_12c = FUN_0040ff90((int)&local_84);
      local_130 = local_78;
      local_78 = local_78 + local_12c;
    }
    local_7c = 0;
    FUN_0040dc60(local_9c);
    local_14 = 0;
    if ((char)param_7 != '\0') {
      local_134 = FUN_004fbf20(&param_9,local_13c);
      local_14._0_1_ = 1;
      local_108 = local_134;
      local_f4 = (void *)FUN_004f3910(local_134);
      local_f8 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)FUN_004ff7a0(local_f4,local_164);
      FUN_00418eb0(local_9c,local_f8);
      FUN_0040e5a0(local_164);
      local_14 = (uint)local_14._1_3_ << 8;
      FUN_004f5600((int)local_13c);
      puVar3 = (undefined4 *)FUN_004494f0(local_9c,local_14c);
      local_110 = *puVar3;
      local_10c = puVar3[1];
      local_fc = (int)(char *)local_dc - (int)local_c8;
      local_7c = FUN_004fb9c0(local_fc);
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
      local_100 = local_78;
      puVar3 = (undefined4 *)move<>(&param_2);
      local_104 = *puVar3;
      FUN_004ef7e0(param_1,local_104,local_100,&param_4,'\x02',local_c4);
      local_14 = 0xffffffff;
      FUN_0040e5a0(local_9c);
    }
    else {
      puVar3 = (undefined4 *)move<>(&param_2);
      FUN_004f5fe0(local_c4,param_1,*puVar3);
      local_14 = 0xffffffff;
      FUN_0040e5a0(local_9c);
    }
  }
  *unaff_FS_OFFSET = local_1c;
  FUN_005429ee(local_24 ^ (uint)&stack0xfffffff0);
  return;
}

