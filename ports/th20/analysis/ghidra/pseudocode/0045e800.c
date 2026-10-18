/* Entry: 0x0045e800; symbol: FUN_0045e800; body bytes: 538 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0045e800(void *this,float param_1)

{
  float fVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  default_delete<class_std::_Facet_base> *pdVar4;
  float *pfVar5;
  undefined4 *puVar6;
  float10 fVar7;
  int local_2c;
  default_delete<class_std::_Facet_base> *local_28;
  default_delete<class_std::_Facet_base> *local_24;
  float local_18;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  local_24 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)this + 0x2d0));
  pdVar4 = (default_delete<class_std::_Facet_base> *)FUN_0045dca0((int)this + 0x2d0);
  for (; local_24 != pdVar4; local_24 = local_24 + 4) {
    fVar7 = FUN_004298e0(0x5ba4c4);
    *(float *)local_24 = *(float *)local_24 + (float)fVar7 * DAT_0056c8d0;
    if (*(float *)local_24 <= param_1 + DAT_0056c8e0) {
      if (*(float *)local_24 <= param_1 - DAT_0056c8e0 &&
          param_1 - DAT_0056c8e0 != *(float *)local_24) {
        *(float *)local_24 = param_1 - DAT_0056c8e0;
      }
    }
    else {
      *(float *)local_24 = param_1 + DAT_0056c8e0;
    }
  }
  local_2c = 0;
  FUN_00429210(&local_18,0.0);
  local_28 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
  pdVar4 = (default_delete<class_std::_Facet_base> *)FUN_0045dcc0((int)this);
  for (; local_28 != pdVar4; local_28 = local_28 + 8) {
    pfVar5 = (float *)FUN_00414580((void *)((int)this + 0x2d0),local_2c);
    fVar1 = *pfVar5;
    fVar7 = FUN_004292e0(&local_18);
    FUN_00439330(local_28,(float *)local_28,(float)fVar7,fVar1);
    FUN_004530c0(&local_18,(DAT_0056e0f0 * DAT_0056c8d0) / _DAT_0056ee48);
    local_2c = local_2c + 1;
  }
  puVar6 = (undefined4 *)FUN_0045d670(this,0);
  uVar2 = *puVar6;
  uVar3 = puVar6[1];
  puVar6 = (undefined4 *)FUN_0045d670(this,0x3b);
  *puVar6 = uVar2;
  puVar6[1] = uVar3;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

