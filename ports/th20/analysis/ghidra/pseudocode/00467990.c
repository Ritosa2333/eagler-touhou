/* Entry: 0x00467990; symbol: FUN_00467990; body bytes: 645 */

undefined4 __thiscall FUN_00467990(void *this,undefined4 *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  float10 fVar4;
  undefined1 local_6c [8];
  undefined1 local_64 [8];
  undefined4 local_5c [2];
  float local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  void *local_44;
  void *local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  void *local_2c;
  void *local_28;
  default_delete<class_std::_Facet_base> *local_24;
  void *local_20;
  void *local_1c;
  default_delete<class_std::_Facet_base> *local_18;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_14;
  int local_10;
  default_delete<class_std::_Facet_base> *local_c;
  void *local_8;
  
  *(undefined4 *)((int)this + 0x188) = param_2;
  local_1c = (void *)((int)this + 8);
  local_8 = this;
  puVar2 = (undefined4 *)FUN_0045d670(local_1c,0);
  *puVar2 = 0;
  local_20 = (void *)((int)local_8 + 8);
  iVar3 = FUN_0045d670(local_20,0);
  *(undefined4 *)(iVar3 + 4) = 0;
  FUN_00452f20((void *)((int)local_8 + 0x170),(float)param_1[6]);
  FUN_00423520((void *)((int)local_8 + 0x178),1);
  local_10 = 0xff;
  local_14 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)((int)local_8 + 0xf8);
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first(local_14);
  local_24 = (default_delete<class_std::_Facet_base> *)FUN_00466fb0((int)local_14);
  for (; local_c != local_24; local_c = local_c + 4) {
    local_18 = local_c;
    local_10 = local_10 + -8;
    *(undefined4 *)local_c = param_1[8];
    local_c[3] = local_10._0_1_;
  }
  local_28 = (void *)((int)local_8 + 8);
  FUN_00429250(local_5c,0,0);
  FUN_00467c70(local_28,local_5c);
  local_2c = *(void **)((int)local_8 + 4);
  FUN_0045dd60(local_2c,param_1);
  fVar4 = FUN_00438290(0x5b6758);
  local_30 = (float)fVar4;
  local_3c = local_30 * (float)param_1[0xb] * DAT_0056e0ec;
  fVar4 = FUN_00438290(0x5b6758);
  local_34 = (float)fVar4;
  local_38 = local_34 * (float)param_1[0xc] * DAT_0056e0ec;
  puVar2 = FUN_00429250(local_64,local_3c,local_38);
  uVar1 = puVar2[1];
  *(undefined4 *)((int)local_8 + 0x18c) = *puVar2;
  *(undefined4 *)((int)local_8 + 400) = uVar1;
  puVar2 = FUN_00429250(local_6c,0,0);
  uVar1 = puVar2[1];
  *(undefined4 *)((int)local_8 + 0x194) = *puVar2;
  *(undefined4 *)((int)local_8 + 0x198) = uVar1;
  local_40 = *(void **)((int)local_8 + 4);
  FUN_00450350(local_40,5);
  local_44 = *(void **)((int)local_8 + 4);
  FUN_00438c60(local_44,0x1e,0,0xff,0);
  local_48 = *(void **)((int)local_8 + 4);
  FUN_00450350(local_48,0x22);
  local_4c = *(void **)((int)local_8 + 4);
  FUN_004614e0(local_4c,2);
  local_50 = *(void **)((int)local_8 + 4);
  FUN_0045dd40(local_50,1);
  fVar4 = FUN_00429830(0x5ba4c4);
  local_54 = (float)fVar4;
  *(float *)((int)local_8 + 0x174) = local_54 * DAT_0056cd90 + DAT_0056d7bc;
  fVar4 = FUN_004297d0(0x5ba4c4);
  FUN_00452f20((void *)((int)local_8 + 0x170),(float)fVar4);
  return 0;
}

