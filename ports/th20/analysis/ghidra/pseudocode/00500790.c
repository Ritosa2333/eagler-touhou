/* Entry: 0x00500790; symbol: FUN_00500790; body bytes: 299 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00500790(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  float10 fVar3;
  undefined4 local_34;
  void *local_30;
  void *local_2c;
  float local_28;
  float local_24;
  default_delete<class_std::_Facet_base> *local_20;
  float *local_1c;
  float local_18;
  int local_14;
  void *local_10;
  default_delete<class_std::_Facet_base> *local_c;
  int local_8;
  
  local_8 = param_1;
  local_10 = (void *)FUN_00412730(*(int *)(param_1 + 0x120));
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)(local_8 + 0x50));
  puVar1 = (undefined4 *)FUN_004aac00(local_10,&local_34,(float *)local_c,DAT_0056fa38);
  *(undefined4 *)(local_8 + 0xa0) = *puVar1;
  iVar2 = FUN_0040c300((undefined4 *)(local_8 + 0xa0));
  if (iVar2 == 0) {
    local_30 = (void *)(local_8 + 0x50);
    FUN_0047a560(local_30,DAT_0056e0f8 / DAT_0056c8d0);
  }
  else {
    local_14 = FUN_004aaac0((int *)(local_8 + 0xa0));
    local_2c = (void *)(local_8 + 0x50);
    local_1c = (float *)FUN_0047a2c0(local_14);
    local_20 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(local_8 + 0x50));
    fVar3 = FUN_004298e0(0x5ba4a8);
    local_18 = (float)fVar3;
    local_28 = (local_18 * DAT_0056e0f0 * DAT_0056edd4) / _DAT_00570db0;
    fVar3 = (float10)FUN_0047a2e0((float *)local_20,local_1c);
    local_24 = (float)fVar3;
    FUN_0047a560(local_2c,local_24 + local_28);
  }
  return 0;
}

