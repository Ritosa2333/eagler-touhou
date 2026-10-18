/* Entry: 0x00501240; symbol: FUN_00501240; body bytes: 351 */

undefined4 __fastcall FUN_00501240(int param_1)

{
  char cVar1;
  int iVar2;
  undefined4 *puVar3;
  float10 fVar4;
  undefined4 local_2c;
  void *local_28;
  default_delete<class_std::_Facet_base> *local_24;
  float *local_20;
  void *local_1c;
  int local_18;
  void *local_14;
  default_delete<class_std::_Facet_base> *local_10;
  int local_c;
  int local_8;
  
  local_8 = param_1;
  iVar2 = FUN_00412730(*(int *)(param_1 + 0x120));
  if (iVar2 == 0) {
    FUN_004117a0((void *)(local_8 + 0xa0),0);
  }
  else {
    iVar2 = FUN_0040c300((undefined4 *)(local_8 + 0xa0));
    if (iVar2 == 0) {
      local_14 = (void *)FUN_00412730(*(int *)(local_8 + 0x120));
      local_10 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                               *)(local_8 + 0x50));
      puVar3 = (undefined4 *)FUN_004aac00(local_14,&local_2c,(float *)local_10,DAT_0056cdac);
      *(undefined4 *)(local_8 + 0xa0) = *puVar3;
    }
    iVar2 = FUN_0040c300((undefined4 *)(local_8 + 0xa0));
    if (iVar2 != 0) {
      local_1c = (void *)FUN_00412730(*(int *)(local_8 + 0x120));
      local_18 = FUN_0040c300((undefined4 *)(local_8 + 0xa0));
      cVar1 = FUN_0049bf00(local_1c,local_18);
      if (cVar1 == '\0') {
        FUN_004117a0((void *)(local_8 + 0xa0),0);
      }
      else {
        local_c = FUN_004aaac0((int *)(local_8 + 0xa0));
        iVar2 = FUN_0047a3e0(local_c);
        if (iVar2 == 0) {
          local_28 = (void *)(local_8 + 0x50);
          local_20 = (float *)FUN_0047a2c0(local_c);
          local_24 = std::
                     _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                   *)(local_8 + 0x50));
          fVar4 = (float10)FUN_0047a2e0((float *)local_24,local_20);
          FUN_0047a560(local_28,(float)fVar4);
        }
        else {
          FUN_004117a0((void *)(local_8 + 0xa0),0);
        }
      }
    }
  }
  return 0;
}

