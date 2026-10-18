/* Entry: 0x00500290; symbol: FUN_00500290; body bytes: 228 */

undefined4 __fastcall FUN_00500290(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  float10 fVar3;
  undefined4 local_28;
  void *local_24;
  void *local_20;
  default_delete<class_std::_Facet_base> *local_1c;
  float *local_18;
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
  puVar1 = (undefined4 *)FUN_004aac00(local_10,&local_28,(float *)local_c,DAT_00572660);
  *(undefined4 *)(local_8 + 0xa0) = *puVar1;
  iVar2 = FUN_0040c300((undefined4 *)(local_8 + 0xa0));
  if (iVar2 == 0) {
    local_24 = (void *)(local_8 + 0x50);
    FUN_0047a560(local_24,DAT_0056e0f8 / DAT_0056c8d0);
  }
  else {
    local_14 = FUN_004aaac0((int *)(local_8 + 0xa0));
    local_20 = (void *)(local_8 + 0x50);
    local_18 = (float *)FUN_0047a2c0(local_14);
    local_1c = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(local_8 + 0x50));
    fVar3 = (float10)FUN_0047a2e0((float *)local_1c,local_18);
    FUN_0047a560(local_20,(float)fVar3);
  }
  return 0;
}

