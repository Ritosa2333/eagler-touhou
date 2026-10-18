/* Entry: 0x00479360; symbol: FUN_00479360; body bytes: 1279 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00479360(void)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  float *pfVar5;
  default_delete<class_std::_Facet_base> *pdVar6;
  float10 fVar7;
  undefined1 local_9c [12];
  undefined4 local_90;
  undefined1 local_8c [4];
  void *local_88;
  undefined4 *local_84;
  default_delete<class_std::_Facet_base> *local_80;
  void *local_7c;
  void *local_78;
  float local_74;
  void *local_70;
  void *local_6c;
  void *local_68;
  void *local_64;
  float local_60;
  float local_5c;
  float local_58;
  default_delete<class_std::_Facet_base> *local_54;
  float *local_50;
  void *local_4c;
  default_delete<class_std::_Facet_base> *local_48;
  void *local_44;
  void *local_40;
  void *local_3c;
  void *local_38;
  void *local_34;
  void *local_30;
  void *local_2c;
  void *local_28;
  void *local_24;
  float local_20;
  float local_1c;
  void *local_18;
  float local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  bVar1 = FUN_0045d030((int *)((int)local_18 + 0xa4));
  if (CONCAT31(extraout_var,bVar1) != 0) {
    local_88 = (void *)((int)local_18 + 0xa4);
    bVar1 = FUN_00423560(local_88,0x5a);
    if (bVar1) {
      local_24 = (void *)((int)local_18 + 4);
      iVar2 = FUN_00460830(0);
      puVar3 = (undefined4 *)FUN_00460850(iVar2);
      FUN_0047a5e0(local_24,puVar3);
      local_28 = (void *)((int)local_18 + 4);
      FUN_0047a210(local_28,DAT_0056cd68);
      local_2c = (void *)((int)local_18 + 4);
      FUN_0047a240(local_2c,*(float *)((int)local_18 + 0xd4));
    }
    else {
      local_30 = (void *)((int)local_18 + 0xa4);
      bVar1 = FUN_00423560(local_30,*(int *)((int)local_18 + 200) * 10 + 0x5a);
      if (bVar1) {
        local_34 = (void *)((int)local_18 + 4);
        iVar2 = FUN_00460830(0);
        puVar3 = (undefined4 *)FUN_00460850(iVar2);
        FUN_0047a5e0(local_34,puVar3);
        local_38 = (void *)((int)local_18 + 4);
        FUN_0047a240(local_38,*(float *)((int)local_18 + 0xd4));
      }
      else {
        local_3c = (void *)((int)local_18 + 0xa4);
        bVar1 = FUN_004785e0(local_3c,*(int *)((int)local_18 + 200) * 10 + 0x5a);
        if (bVar1) {
          FUN_0047a500((int)local_18 + 4);
          local_40 = (void *)((int)local_18 + 4);
          fVar7 = (float10)FUN_00456210((float *)((int)local_18 + 0xb4));
          FUN_0047a560(local_40,(float)fVar7);
          local_44 = (void *)((int)local_18 + 4);
          fVar7 = (float10)FUN_0047a2a0((float *)((int)local_18 + 0xb4));
          FUN_0047a5c0(local_44,(float)fVar7);
        }
        else {
          iVar2 = FUN_00478060(0);
          if (iVar2 != 0) {
            local_4c = (void *)FUN_00478060(0);
            local_48 = std::
                       _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                       ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                     *)((int)local_18 + 4));
            puVar3 = (undefined4 *)FUN_004aac00(local_4c,&local_90,(float *)local_48,DAT_0056f114);
            *(undefined4 *)((int)local_18 + 0xc0) = *puVar3;
          }
          iVar2 = FUN_0040c300((undefined4 *)((int)local_18 + 0xc0));
          if (iVar2 == 0) {
            local_70 = (void *)((int)local_18 + 4);
            iVar2 = FUN_0047a400(local_70,0.0,DAT_0056ed14,DAT_0056f10c,DAT_0056cda4);
            if (iVar2 != 0) {
              local_78 = (void *)((int)local_18 + 4);
              fVar7 = FUN_0047a320((int)local_18 + 4);
              local_74 = (float)fVar7;
              FUN_0047a5c0(local_78,local_74 * _DAT_0056fc00);
            }
          }
          else {
            uVar4 = FUN_004aaac0((int *)((int)local_18 + 0xc0));
            *(undefined4 *)((int)local_18 + 0xc4) = uVar4;
            iVar2 = FUN_0047a3e0(*(int *)((int)local_18 + 0xc4));
            if (iVar2 == 0) {
              local_50 = (float *)FUN_0047a2c0(*(int *)((int)local_18 + 0xc4));
              local_54 = std::
                         _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                       *)((int)local_18 + 4));
              fVar7 = (float10)FUN_0047a2e0((float *)local_54,local_50);
              local_58 = (float)fVar7;
              pfVar5 = (float *)FUN_00421830((int)local_18 + 4);
              fVar7 = FUN_004292e0(pfVar5);
              fVar7 = FUN_004396c0(local_58,(float)fVar7);
              local_20 = (float)fVar7;
              fVar7 = FUN_0047a320((int)local_18 + 4);
              local_1c = (float)fVar7;
              fVar7 = FUN_0047a630(local_20);
              local_5c = (float)fVar7;
              if (local_5c < (DAT_0056e0f0 * DAT_0056c8cc) / DAT_0056c8e0) {
                fVar7 = FUN_0047a630(local_20);
                local_60 = (float)fVar7;
                if ((local_60 < DAT_0056e0f0 / DAT_0056f7a8) &&
                   (local_1c = local_1c + DAT_0056fbf8, DAT_0056d7bc < local_1c)) {
                  local_1c = DAT_0056d7bc;
                }
              }
              else {
                local_1c = local_1c - _DAT_0056fbfc;
                if (local_1c < DAT_0056c8cc) {
                  local_1c = DAT_0056c8cc;
                }
              }
              local_68 = (void *)((int)local_18 + 4);
              local_64 = (void *)FUN_00421830((int)local_18 + 4);
              pfVar5 = (float *)FUN_00452fc0(local_64,local_8c,local_20 * DAT_0056ef28);
              fVar7 = FUN_004292e0(pfVar5);
              FUN_0047a560(local_68,(float)fVar7);
              local_6c = (void *)((int)local_18 + 4);
              FUN_0047a5c0(local_6c,local_1c);
            }
          }
        }
      }
    }
  }
  pdVar6 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)((int)local_18 + 4));
  local_14 = *(float *)pdVar6;
  local_10 = *(undefined4 *)(pdVar6 + 4);
  local_c = *(undefined4 *)(pdVar6 + 8);
  FUN_0047a1f0((float *)((int)local_18 + 4));
  local_7c = local_18;
  pdVar6 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)((int)local_18 + 4));
  FUN_004645e0(local_7c,(undefined4 *)pdVar6);
  local_80 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)local_18 + 4));
  puVar3 = (undefined4 *)FUN_00429440(local_80,local_9c,&local_14);
  *(undefined4 *)((int)local_18 + 0xb4) = *puVar3;
  *(undefined4 *)((int)local_18 + 0xb8) = puVar3[1];
  *(undefined4 *)((int)local_18 + 0xbc) = puVar3[2];
  local_84 = (undefined4 *)((int)local_18 + 0xa4);
  FUN_00423540(local_84);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

