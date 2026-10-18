/* Entry: 0x00440340; symbol: FUN_00440340; body bytes: 1378 */

void FUN_00440340(int param_1)

{
  int iVar1;
  float *pfVar2;
  void *extraout_ECX;
  void *extraout_ECX_00;
  undefined4 *puVar3;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var4;
  float10 fVar5;
  undefined1 local_b0 [12];
  float local_a4;
  float local_a0;
  float local_98;
  float *local_94;
  float local_90;
  float *local_8c;
  float local_88;
  float local_84;
  float local_80;
  float local_7c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_78 [48];
  float local_48;
  float fStack_44;
  float fStack_40;
  undefined1 local_38 [12];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_2c [12];
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = FUN_00437770(param_1);
  local_a0 = *(float *)(iVar1 + 8);
  fVar5 = FUN_00439820(extraout_ECX,local_a0);
  local_88 = (float)fVar5;
  fVar5 = FUN_004397c0(extraout_ECX_00,local_a0);
  local_84 = (float)fVar5;
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first(local_78);
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)&local_14);
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first(local_2c);
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)&local_20);
  FUN_00422dd0(local_38,0,0,0);
  puVar3 = &DAT_0056e100;
  p_Var4 = local_78;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined4 *)p_Var4 = *puVar3;
    puVar3 = puVar3 + 1;
    p_Var4 = p_Var4 + 4;
  }
  local_48 = *(float *)(param_1 + 0x2c) + *(float *)(param_1 + 0x5bc) + *(float *)(param_1 + 0x484);
  fStack_44 = *(float *)(param_1 + 0x30) + *(float *)(param_1 + 0x5c0) + *(float *)(param_1 + 0x488)
  ;
  fStack_40 = *(float *)(param_1 + 0x34) + *(float *)(param_1 + 0x5c4) + *(float *)(param_1 + 0x48c)
  ;
  D3DXVec3Project(&local_14,local_38,DAT_005c5840 + 0xe0,DAT_005c5840 + 0xa0,DAT_005c5840 + 0x60,
                  local_78);
  if ((0.0 <= local_c) && (local_c <= DAT_0056c8cc)) {
    D3DXVec3Project(local_2c,DAT_005c5840 + 0x30,DAT_005c5840 + 0xe0,DAT_005c5840 + 0xa0,
                    DAT_005c5840 + 0x60,local_78);
    pfVar2 = (float *)FUN_00429440(local_2c,local_b0,&local_14);
    local_20 = *pfVar2;
    local_1c = pfVar2[1];
    local_18 = pfVar2[2];
    fVar5 = (float10)FUN_004399b0(&local_20);
    local_a4 = (float)fVar5;
    local_98 = local_a4 * DAT_0056e0ec * *(float *)(param_1 + 0x70) * *(float *)(param_1 + 0x50) *
               *(float *)(param_1 + 0x58);
    local_90 = local_a4 * DAT_0056e0ec * *(float *)(param_1 + 0x74) * *(float *)(param_1 + 0x54) *
               *(float *)(param_1 + 0x5c);
    local_7c = local_14;
    local_80 = local_10;
    DAT_005aef6c = local_c;
    DAT_005aef50 = local_c;
    DAT_005aef34 = local_c;
    DAT_005aef18 = local_c;
    local_94 = (float *)(&DAT_0056e140 + *(int *)(param_1 + 0x4a8) * 0x10);
    local_8c = (float *)(&DAT_0056e170 + *(int *)(param_1 + 0x4ac) * 0x10);
    FUN_00445b20((float *)&DAT_005aef10,*local_94 * local_98,*local_8c * local_90,local_88,local_84,
                 local_14,local_10);
    FUN_00445b20((float *)&DAT_005aef2c,local_94[1] * local_98,local_8c[1] * local_90,local_88,
                 local_84,local_7c,local_80);
    FUN_00445b20((float *)&DAT_005aef48,local_94[2] * local_98,local_8c[2] * local_90,local_88,
                 local_84,local_7c,local_80);
    FUN_00445b20((float *)&DAT_005aef64,local_94[3] * local_98,local_8c[3] * local_90,local_88,
                 local_84,local_7c,local_80);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

