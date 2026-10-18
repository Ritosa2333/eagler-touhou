/* Entry: 0x0046c210; symbol: FUN_0046c210; body bytes: 1823 */

void __cdecl FUN_0046c210(void *param_1,float *param_2)

{
  bool bVar1;
  char cVar2;
  undefined4 uVar3;
  wchar_t *pwVar4;
  int *piVar5;
  int *unaff_FS_OFFSET;
  undefined1 auStack_1f0 [20];
  undefined4 uStack_1dc;
  va_list pcVar6;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var7;
  undefined1 local_1c0 [32];
  undefined1 *local_1a0;
  undefined8 local_19c;
  undefined8 local_194;
  undefined4 *local_18c;
  undefined4 *local_188;
  undefined4 *local_184;
  int local_180;
  int local_17c;
  LPCSTR local_178;
  int local_174;
  void *local_170;
  undefined4 *local_16c;
  undefined4 *local_168;
  void *local_164;
  int *local_160;
  void *local_15c;
  va_list local_158;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_154;
  int local_150;
  undefined4 *local_14c;
  undefined4 *local_148;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_144 [28];
  int local_128;
  undefined4 local_124;
  int local_120 [2];
  undefined4 local_118;
  wchar_t local_114 [128];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568e31;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar3 = FUN_0040c310(&DAT_005c0240,0x12);
  FUN_0040b730(&local_118,uVar3);
  local_8 = 0;
  pcVar6 = &stack0x00000014;
  local_158 = pcVar6;
  pwVar4 = (wchar_t *)FUN_0040c300((undefined4 *)&stack0x0000000c);
  FUN_0046a550(local_114,pwVar4,pcVar6);
  local_158 = (va_list)0x0;
  FUN_0040c080(local_144,0x1c);
  FUN_0046a830(local_144,(uint *)local_114);
  local_8._0_1_ = 1;
  local_150 = 0;
  local_15c = (void *)((int)param_1 + 0x1a20c);
  FUN_0040c080(local_120,8);
  FUN_00412280(local_15c,local_120);
  local_8 = CONCAT31(local_8._1_3_,2);
  local_160 = (int *)FUN_00412540();
  while (bVar1 = FUN_00411bb0(local_120,local_160), bVar1) {
    local_154 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 *)FUN_0040c300(local_120);
    p_Var7 = local_144;
    piVar5 = (int *)FUN_0040f160(local_154);
    cVar2 = FUN_00467fd0((void *)(*piVar5 + 0x5f8),(int)p_Var7);
    if ((cVar2 != '\0') &&
       (piVar5 = (int *)FUN_0040f160(local_154),
       *(int *)(*piVar5 + 0x62c) == *(int *)((int)param_1 + 0x1a1e4))) {
      piVar5 = (int *)FUN_0040f160(local_154);
      local_150 = *piVar5;
      if (*(int *)(local_150 + 0x620) == 0) {
        *(float *)(local_150 + 0x614) = *param_2 * DAT_005b8818;
        *(float *)(local_150 + 0x618) = param_2[1] * DAT_005b8818;
        *(undefined4 *)(local_150 + 0x64c) = *(undefined4 *)((int)param_1 + 0x1a1c0);
        *(undefined4 *)(local_150 + 0x654) = *(undefined4 *)((int)param_1 + 0x1a1c4);
        *(undefined4 *)(local_150 + 0x650) = *(undefined4 *)((int)param_1 + 0x1a1c8);
        *(undefined4 *)(local_150 + 0x620) = 1;
      }
      else {
        local_14c = FUN_0046a1c0();
        local_14c[0x18b] = *(undefined4 *)((int)param_1 + 0x1a1e4);
        uVar3 = *(undefined4 *)(local_150 + 0x638);
        local_14c[0x18d] = *(undefined4 *)(local_150 + 0x634);
        local_14c[0x18e] = uVar3;
        local_14c[399] = *(undefined4 *)(local_150 + 0x63c);
        local_14c[400] = *(undefined4 *)(local_150 + 0x640);
        local_14c[0x191] = *(undefined4 *)(local_150 + 0x644);
        local_14c[0x192] = *(undefined4 *)(local_150 + 0x648);
        local_164 = (void *)FUN_00470670();
        FUN_004708e0(local_164,local_14c + 5,0x34);
        local_168 = local_14c + 5;
        uStack_1dc = 0x46c52d;
        FUN_00470b80(local_168,local_14c[399],local_14c[400],local_14c[0x191],local_14c[0x192]);
        FUN_0046b130(local_14c + 0x17e,local_144);
        *(undefined1 *)(local_14c + 0x19e) = 1;
        local_14c[0x19f] = local_150 + 0x678;
        FUN_0046d170(param_1,param_2,local_14c);
      }
      break;
    }
    FUN_00411c30(local_120);
  }
  local_8 = CONCAT31(local_8._1_3_,1);
  FUN_00411b00(local_120);
  if (local_150 == 0) {
    local_148 = FUN_0046a1c0();
    FUN_0046d170(param_1,param_2,local_148);
    local_170 = (void *)FUN_00470670();
    local_16c = local_148 + 5;
    FUN_004708e0(local_170,local_16c,0x1b);
    local_148[0x18b] = *(undefined4 *)((int)param_1 + 0x1a1e4);
    if (local_148[0x18c] == 0) {
      local_174 = *(int *)((int)param_1 + 0x1a1e4);
      local_178 = (LPCSTR)FUN_0044baf0((int)local_144);
      local_194 = FUN_004156e0(local_178,local_174);
      *(undefined8 *)(local_148 + 0x18d) = local_194;
      local_148[0x191] = local_148[0x18d];
      local_148[0x192] = local_148[0x18e];
    }
    else {
      local_19c = FUN_004156e0(&DAT_0056f75c,*(int *)((int)param_1 + 0x1a1e4));
      *(undefined8 *)(local_148 + 0x18d) = local_19c;
      local_180 = FUN_0041ca90((int)local_144);
      local_180 = local_180 * local_148[0x18c];
      local_17c = local_148[0x18d];
      local_148[0x18d] = local_17c + local_180;
      local_148[0x191] = local_148[0x18d];
      local_148[0x192] = local_148[0x18e];
    }
    FUN_00470680(param_1,&local_128,local_148[0x191],local_148[0x192]);
    if (local_128 < 0) {
      FUN_0046a100(local_148);
      local_148 = (undefined4 *)0x0;
      local_8 = local_8 & 0xffffff00;
      FUN_00449240(local_144);
      local_8 = 0xffffffff;
      FUN_0040b900(&local_118);
      goto LAB_0046c915;
    }
    local_148[399] = local_128;
    local_148[400] = local_124;
    local_184 = local_148 + 5;
    uStack_1dc = 0x46c85a;
    FUN_00470b80(local_184,local_148[399],local_148[400],local_148[0x191],local_148[0x192]);
    *(undefined1 *)(local_148 + 0x19e) = 0;
    FUN_0040baa0(local_148 + 0x1a0,0);
    FUN_0046b130(local_148 + 0x17e,local_144);
    local_1a0 = auStack_1f0;
    local_18c = FUN_0046a5e0(local_1c0,&local_148,local_144);
    local_8._0_1_ = 3;
    local_188 = local_18c;
    FUN_00467ee0(auStack_1f0,local_18c);
    FUN_00470180((int)param_1);
    local_8 = CONCAT31(local_8._1_3_,1);
    FUN_0046ade0((int)local_1c0);
  }
  local_8 = local_8 & 0xffffff00;
  FUN_00449240(local_144);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_118);
LAB_0046c915:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

