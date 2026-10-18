/* Entry: 0x0045d780; symbol: FUN_0045d780; body bytes: 697 */

void __thiscall FUN_0045d780(void *this,float param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  undefined4 *puVar4;
  float *pfVar5;
  void *this_00;
  float10 fVar6;
  float fVar7;
  int local_30;
  default_delete<class_std::_Facet_base> *local_2c;
  default_delete<class_std::_Facet_base> *local_28;
  default_delete<class_std::_Facet_base> *local_24;
  float local_18;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  local_24 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)this + 0x1e0));
  pdVar3 = (default_delete<class_std::_Facet_base> *)FUN_0045dca0((int)this + 0x1e0);
  for (; local_24 != pdVar3; local_24 = local_24 + 4) {
    fVar6 = FUN_004298e0(0x5ba4c4);
    local_24[2] = SUB41((int)((float)fVar6 * DAT_0056cd90 + DAT_0056ed14),0);
  }
  local_28 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)this + 0x2d0));
  pdVar3 = (default_delete<class_std::_Facet_base> *)FUN_0045dca0((int)this + 0x2d0);
  for (; local_28 != pdVar3; local_28 = local_28 + 4) {
    fVar6 = FUN_004298e0(0x5ba4c4);
    *(float *)local_28 = *(float *)local_28 + (float)fVar6 * DAT_0056c8d0;
    if (*(float *)local_28 <= param_1 / DAT_0056c8e0 + param_1) {
      fVar7 = param_1 - param_1 / DAT_0056c8e0;
      if (*(float *)local_28 <= fVar7 && fVar7 != *(float *)local_28) {
        *(float *)local_28 = param_1 - param_1 / DAT_0056c8e0;
      }
    }
    else {
      *(float *)local_28 = param_1 / DAT_0056c8e0 + param_1;
    }
  }
  local_30 = 0;
  FUN_00429210(&local_18,0.0);
  puVar4 = (undefined4 *)FUN_00414580((void *)((int)this + 0x2d0),0);
  *puVar4 = 0;
  local_2c = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
  pdVar3 = (default_delete<class_std::_Facet_base> *)FUN_0045dcc0((int)this);
  for (; local_2c != pdVar3; local_2c = local_2c + 8) {
    pfVar5 = (float *)FUN_00414580((void *)((int)this + 0x2d0),local_30);
    fVar7 = *pfVar5;
    fVar6 = FUN_004292e0(&local_18);
    FUN_00439330(this_00,(float *)local_2c,(float)fVar6,fVar7);
    FUN_004530c0(&local_18,(DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056ed10);
    local_30 = local_30 + 1;
  }
  puVar4 = (undefined4 *)FUN_0045d670(this,1);
  uVar1 = *puVar4;
  uVar2 = puVar4[1];
  puVar4 = (undefined4 *)FUN_0045d670(this,0x3b);
  *puVar4 = uVar1;
  puVar4[1] = uVar2;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

