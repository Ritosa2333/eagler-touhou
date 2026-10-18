/* Entry: 0x00501820; symbol: FUN_00501820; body bytes: 870 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00501820(int param_1)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  undefined4 *puVar4;
  float *pfVar5;
  float10 fVar6;
  undefined1 local_5c [4];
  undefined4 local_58;
  void *local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  void *local_44;
  float local_40;
  float local_3c;
  void *local_38;
  float local_34;
  default_delete<class_std::_Facet_base> *local_30;
  float *local_2c;
  void *local_28;
  int local_24;
  void *local_20;
  default_delete<class_std::_Facet_base> *local_1c;
  int local_18;
  float local_14;
  float local_10;
  float local_c;
  int local_8;
  
  if (*(int *)(param_1 + 0x9c) != 2) {
    local_8 = param_1;
    iVar3 = FUN_00412730(*(int *)(param_1 + 0x120));
    if (iVar3 == 0) {
      FUN_004117a0((void *)(local_8 + 0xa0),0);
    }
    else {
      iVar3 = FUN_0040c300((undefined4 *)(local_8 + 0xa0));
      if (iVar3 == 0) {
        local_20 = (void *)FUN_00412730(*(int *)(local_8 + 0x120));
        local_1c = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)(local_8 + 0x50));
        puVar4 = (undefined4 *)FUN_004aac00(local_20,&local_58,(float *)local_1c,DAT_0056cda0);
        *(undefined4 *)(local_8 + 0xa0) = *puVar4;
      }
      iVar3 = FUN_0040c300((undefined4 *)(local_8 + 0xa0));
      if (iVar3 != 0) {
        local_28 = (void *)FUN_00412730(*(int *)(local_8 + 0x120));
        local_24 = FUN_0040c300((undefined4 *)(local_8 + 0xa0));
        cVar1 = FUN_0049bf00(local_28,local_24);
        if (cVar1 == '\0') {
          FUN_004117a0((void *)(local_8 + 0xa0),0);
        }
        else {
          local_18 = FUN_004aaac0((int *)(local_8 + 0xa0));
          iVar3 = FUN_0047a3e0(local_18);
          if (iVar3 == 0) {
            local_2c = (float *)FUN_0047a2c0(local_18);
            local_30 = std::
                       _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                       ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                     *)(local_8 + 0x50));
            fVar6 = (float10)FUN_0047a2e0((float *)local_30,local_2c);
            local_34 = (float)fVar6;
            pfVar5 = (float *)FUN_00421830(local_8 + 0x50);
            fVar6 = FUN_004292e0(pfVar5);
            fVar6 = FUN_004396c0(local_34,(float)fVar6);
            local_14 = (float)fVar6;
            fVar6 = FUN_0047a320(local_8 + 0x50);
            local_c = (float)fVar6;
            local_38 = (void *)FUN_00421830(local_8);
            bVar2 = FUN_00423560(local_38,0x3c);
            if (bVar2) {
              fVar6 = FUN_0047a630(local_14);
              local_3c = (float)fVar6;
              if (local_3c < (DAT_0056e0f0 * DAT_0056c8cc) / DAT_0056c8e0) {
                fVar6 = FUN_0047a630(local_14);
                local_40 = (float)fVar6;
                if ((local_40 < DAT_0056e0f0 / DAT_0056f7a8) &&
                   (local_c = local_c + DAT_0056fbf8, DAT_0056cd90 < local_c)) {
                  local_c = DAT_0056cd90;
                }
              }
              else {
                local_c = local_c - DAT_0056fbf8;
                if (local_c < DAT_0056c8e0) {
                  local_c = DAT_0056c8e0;
                }
              }
              local_48 = (void *)(local_8 + 0x50);
              local_44 = (void *)FUN_00421830(local_8 + 0x50);
              pfVar5 = (float *)FUN_00452fc0(local_44,local_5c,local_14 * _DAT_005732c4);
              fVar6 = FUN_004292e0(pfVar5);
              FUN_0047a560(local_48,(float)fVar6);
              local_4c = (void *)(local_8 + 0x50);
              FUN_0047a5c0(local_4c,local_c);
              return 0;
            }
            local_50 = (void *)(local_8 + 0x50);
            FUN_0047a5c0(local_50,local_c + DAT_0056fbf8);
            return 0;
          }
          FUN_004117a0((void *)(local_8 + 0xa0),0);
        }
      }
    }
    fVar6 = FUN_0047a320(local_8 + 0x50);
    local_10 = (float)fVar6 + DAT_0056ef28;
    if (DAT_0056cd90 < local_10) {
      local_10 = DAT_0056cd90;
    }
    local_54 = (void *)(local_8 + 0x50);
    FUN_0047a5c0(local_54,local_10);
  }
  return 0;
}

