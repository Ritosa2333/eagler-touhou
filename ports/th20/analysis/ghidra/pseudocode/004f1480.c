/* Entry: 0x004f1480; symbol: FUN_004f1480; body bytes: 1261 */

/* WARNING: Removing unreachable block (ram,0x004f16b1) */

void __cdecl
FUN_004f1480(undefined4 *param_1,undefined4 param_2,uint param_3,int param_4,int param_5,
            undefined4 param_6,uint param_7,undefined4 param_8,undefined4 param_9,
            undefined4 param_10)

{
  char cVar1;
  uint uVar2;
  undefined4 *puVar3;
  _Ptr_base<struct__EXCEPTION_RECORD_const_> *p_Var4;
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_16c [24];
  undefined1 local_154 [8];
  _Ptr_base<struct__EXCEPTION_RECORD_const_> local_14c [8];
  locale local_144 [8];
  void *local_13c;
  int local_138;
  locale *local_134;
  int local_130;
  undefined4 local_12c;
  uint local_128;
  undefined4 local_124;
  undefined4 local_120;
  undefined4 local_118;
  undefined4 local_114;
  uint local_110;
  int local_10c;
  locale *local_108;
  undefined4 local_104;
  int local_100;
  uint local_fc;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_f8;
  int local_f4;
  char *local_f0;
  undefined4 local_ec;
  undefined4 local_e8;
  undefined4 local_e4;
  undefined4 local_e0;
  undefined8 local_dc;
  uint local_d4;
  int local_d0;
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
  puStack_18 = &LAB_0056a34b;
  local_1c = *unaff_FS_OFFSET;
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_24 = uVar2;
  if ((char)param_7 == 'c') {
    puStack_20 = &stack0xfffffffc;
    cVar1 = FUN_004ecac0(param_3,param_4);
    if (cVar1 == '\0') {
      func_0x004fcd20("integral cannot be stored in char",uVar2);
    }
    param_7 = param_7 & 0xffffff;
    local_e8 = param_10;
    local_130 = param_5;
    local_12c = param_6;
    local_128 = param_7;
    local_124 = param_8;
    local_120 = param_9;
    local_ca = (byte)param_3;
    puVar3 = (undefined4 *)move<>(&param_2);
    local_ec = *puVar3;
    local_e4 = local_ec;
    local_e0 = local_ec;
    FUN_004e9710(param_1,local_ec,local_ca,local_130,local_12c,local_128,local_124,local_120,
                 local_e8);
  }
  else {
    if (param_7._2_1_ == '\0') {
      param_7._0_3_ = CONCAT12(2,(undefined2)param_7);
    }
    local_d4 = 10;
    local_d0 = (char)param_7 + -0x42;
    switch((char)param_7) {
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
    local_110 = param_3;
    local_10c = param_4;
    local_f0 = (char *)FUN_004f3330((int)local_70);
    local_dc = FUN_005001f0(local_70,local_f0,local_110,local_10c,local_d4);
    local_c8 = local_70;
    local_78 = (int)(char *)local_dc - (int)local_c8;
    if (param_7._2_1_ != '\x02') {
      local_78 = local_78 + 1;
    }
    if ((char)param_7 == 'X') {
      FUN_004fb8c0(local_c8,(char *)local_dc);
    }
    FUN_0040c080(&local_84,8);
    std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
              ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)&local_84);
    if (param_7._3_1_ != '\0') {
      p_Var4 = FUN_004ec7e0(local_14c,(char)param_7,param_3,param_4);
      local_84 = *(undefined4 *)p_Var4;
      local_80 = *(undefined4 *)(p_Var4 + 4);
      local_138 = FUN_0040ff90((int)&local_84);
      local_f4 = local_78;
      local_78 = local_78 + local_138;
    }
    local_7c = 0;
    FUN_0040dc60(local_9c);
    local_14 = 0;
    if ((char)param_8 != '\0') {
      local_134 = FUN_004fbf20(&param_10,local_144);
      local_14._0_1_ = 1;
      local_108 = local_134;
      local_13c = (void *)FUN_004f3910(local_134);
      local_f8 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)FUN_004ff7a0(local_13c,local_16c);
      FUN_00418eb0(local_9c,local_f8);
      FUN_0040e5a0(local_16c);
      local_14 = (uint)local_14._1_3_ << 8;
      FUN_004f5600((int)local_144);
      puVar3 = (undefined4 *)FUN_004494f0(local_9c,local_154);
      local_118 = *puVar3;
      local_114 = puVar3[1];
      local_fc = (int)(char *)local_dc - (int)local_c8;
      local_7c = FUN_004fb9c0(local_fc);
      local_78 = local_78 + local_7c;
    }
    if ((param_8._1_1_ == '\0') || (param_7._1_1_ != '\0')) {
      local_c9 = '\0';
    }
    else {
      local_c9 = '\x01';
    }
    local_71 = local_c9;
    FUN_0040c080(local_c4,0x28);
    FUN_004f3d00(local_c4,(char *)local_dc,&param_5,&param_3,&local_84,&local_71,&local_78,&local_7c
                 ,&local_c8,local_9c,&param_10);
    if (local_71 == '\0') {
      local_100 = local_78;
      puVar3 = (undefined4 *)move<>(&param_2);
      local_104 = *puVar3;
      FUN_004efd60(param_1,local_104,local_100,&param_5,'\x02',local_c4);
      local_14 = 0xffffffff;
      FUN_0040e5a0(local_9c);
    }
    else {
      puVar3 = (undefined4 *)move<>(&param_2);
      FUN_004f6c40(local_c4,param_1,*puVar3);
      local_14 = 0xffffffff;
      FUN_0040e5a0(local_9c);
    }
  }
  *unaff_FS_OFFSET = local_1c;
  FUN_005429ee(local_24 ^ (uint)&stack0xfffffff0);
  return;
}

