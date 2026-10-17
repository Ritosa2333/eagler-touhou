/* Entry: 0x005013b0; symbol: FUN_005013b0; body bytes: 434 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_005013b0(int param_1)

{
  char cVar1;
  int iVar2;
  undefined4 *puVar3;
  float10 fVar4;
  undefined4 local_3c;
  void *local_38;
  float local_34;
  float local_30;
  float local_2c;
  default_delete<class_std::_Facet_base> *local_28;
  float *local_24;
  float local_20;
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
      puVar3 = (undefined4 *)FUN_004aac00(local_14,&local_3c,(float *)local_10,DAT_0056cdac);
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
          local_24 = (float *)FUN_0047a2c0(local_c);
          local_28 = std::
                     _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                   *)(local_8 + 0x50));
          fVar4 = FUN_004298e0(0x5ba4a8);
          local_20 = (float)fVar4;
          local_30 = (local_20 * DAT_0056e0f0 * DAT_0056edd4) / _DAT_005732c8;
          fVar4 = (float10)FUN_0047a2e0((float *)local_28,local_24);
          local_2c = (float)fVar4;
          local_34 = local_2c + local_30;
          local_38 = (void *)(local_8 + 0x50);
          FUN_0047a560(local_38,local_34);
        }
        else {
          FUN_004117a0((void *)(local_8 + 0xa0),0);
        }
      }
    }
  }
  return 0;
}

