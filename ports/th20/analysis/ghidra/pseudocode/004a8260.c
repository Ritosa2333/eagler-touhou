/* Entry: 0x004a8260; symbol: FUN_004a8260; body bytes: 1258 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004a8260(int param_1)

{
  bool bVar1;
  int iVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  undefined4 *puVar4;
  float *pfVar5;
  float10 fVar6;
  undefined1 local_88 [4];
  undefined4 *local_84;
  undefined4 *local_80;
  void *local_7c;
  void *local_78;
  void *local_74;
  void *local_70;
  void *local_6c;
  void *local_68;
  void *local_64;
  float local_60;
  float local_5c;
  float local_58;
  float local_54;
  float local_50;
  float local_4c;
  void *local_48;
  undefined4 *local_44;
  undefined4 *local_40;
  int local_3c;
  int local_38;
  int local_34;
  void *local_30;
  int local_2c;
  undefined4 *local_28;
  undefined4 *local_24;
  void *local_20;
  int local_1c;
  undefined4 local_18 [3];
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1c = param_1;
  if ((*(uint *)(param_1 + 0x2cc) >> 2 & 1) == 0) {
    *(uint *)(param_1 + 0x2cc) = *(uint *)(param_1 + 0x2cc) | 4;
    iVar2 = FUN_004ab4c0(param_1);
    if ((iVar2 == 0) && (iVar2 = FUN_004a5df0(local_1c), iVar2 == 0)) {
      FUN_004a4190(local_1c);
      if ((*(uint *)(local_1c + 0x2cc) >> 10 & 1) == 0) {
        local_34 = local_1c + 0xc;
        local_24 = (undefined4 *)FUN_00414f30(local_34);
        local_40 = (undefined4 *)FUN_00414f70(local_34);
        for (; local_24 != local_40; local_24 = local_24 + 5) {
          local_28 = local_24;
          local_20 = (void *)FUN_0044ced0(local_24);
          if (local_20 != (void *)0x0) {
            local_44 = local_28 + 1;
            pdVar3 = std::
                     _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                   *)(local_1c + 0x110));
            FUN_00429570(local_44,local_18,(float *)pdVar3);
            if (-1 < (int)local_28[4]) {
              local_48 = (void *)(local_1c + 0xc);
              puVar4 = (undefined4 *)FUN_0048bce0(local_48,local_28[4]);
              local_38 = FUN_0044ced0(puVar4);
              if (local_38 != 0) {
                pfVar5 = (float *)FUN_0044cb60(local_38);
                FUN_004296e0(local_18,pfVar5);
              }
            }
            FUN_0045dd60(local_20,local_18);
            local_2c = (*(byte *)((int)local_20 + 0x4a2) >> 5) - 1;
            switch(local_2c) {
            case 0:
              pfVar5 = (float *)FUN_004562c0(local_1c + 0x110);
              fVar6 = (float10)FUN_00456210(pfVar5);
              FUN_00450590(local_20,(float)fVar6);
              fVar6 = FUN_00437930((int)local_20);
              *(float *)(local_1c + 0x38) = (float)fVar6;
              break;
            case 1:
              pfVar5 = (float *)FUN_004562c0(local_1c + 0x110);
              fVar6 = (float10)FUN_00456210(pfVar5);
              FUN_00429210(&local_c,(float)fVar6);
              fVar6 = FUN_004292e0(&local_c);
              local_4c = (float)fVar6;
              if (local_4c < DAT_0056e0f8 / DAT_0056c8d0) {
LAB_004a84ac:
                pfVar5 = (float *)FUN_00452fc0(&local_c,local_88,DAT_0056e0f0);
                fVar6 = FUN_004292e0(pfVar5);
                FUN_00450590(local_20,(float)fVar6);
                fVar6 = FUN_004379c0((int)local_20);
                fVar6 = FUN_0047a630((float)fVar6);
                local_54 = (float)fVar6;
                FUN_004777f0(local_20,(uint)local_54 ^ _DAT_0056d7d0);
              }
              else {
                fVar6 = FUN_004292e0(&local_c);
                local_50 = (float)fVar6;
                if (DAT_0056e0f0 / DAT_0056c8d0 < local_50) goto LAB_004a84ac;
                fVar6 = FUN_004292e0(&local_c);
                FUN_00450590(local_20,(float)fVar6);
                fVar6 = FUN_004379c0((int)local_20);
                fVar6 = FUN_0047a630((float)fVar6);
                FUN_004777f0(local_20,(float)fVar6);
              }
              fVar6 = FUN_00437930((int)local_20);
              *(float *)(local_1c + 0x38) = (float)fVar6;
              break;
            case 2:
              pfVar5 = (float *)FUN_004562c0(local_1c + 0x110);
              fVar6 = (float10)FUN_00456210(pfVar5);
              local_58 = (float)fVar6;
              fVar6 = FUN_00438540(local_58 + DAT_0056e0f0);
              FUN_00450590(local_20,(float)fVar6);
              fVar6 = FUN_00437930((int)local_20);
              *(float *)(local_1c + 0x38) = (float)fVar6;
              break;
            case 3:
              pfVar5 = (float *)FUN_004562c0(local_1c + 0x110);
              fVar6 = (float10)FUN_00456210(pfVar5);
              local_5c = (float)fVar6;
              fVar6 = FUN_00438540(local_5c + DAT_0056e0f0 / DAT_0056c8d0);
              FUN_00450590(local_20,(float)fVar6);
              fVar6 = FUN_00437930((int)local_20);
              *(float *)(local_1c + 0x38) = (float)fVar6;
              break;
            case 4:
              pfVar5 = (float *)FUN_004562c0(local_1c + 0x110);
              fVar6 = (float10)FUN_00456210(pfVar5);
              local_60 = (float)fVar6;
              fVar6 = FUN_00438540(local_60 - DAT_0056e0f0 / DAT_0056c8d0);
              FUN_00450590(local_20,(float)fVar6);
              fVar6 = FUN_00437930((int)local_20);
              *(float *)(local_1c + 0x38) = (float)fVar6;
            }
          }
        }
      }
      else {
        local_3c = local_1c + 0xc;
        local_30 = (void *)FUN_00414f30(local_3c);
        local_64 = (void *)FUN_00414f70(local_3c);
        for (; local_30 != local_64; local_30 = (void *)((int)local_30 + 0x14)) {
          local_68 = local_30;
          local_6c = local_30;
          pdVar3 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)(local_1c + 0x110));
          FUN_004645e0(local_6c,(undefined4 *)pdVar3);
        }
      }
      FUN_004a5320(local_1c + 0x1a8);
      local_70 = (void *)(local_1c + 0x288);
      bVar1 = FUN_00461070(local_70,0);
      if (bVar1) {
        local_74 = (void *)(local_1c + 0x288);
        FUN_00429420(local_74);
      }
      local_78 = (void *)(local_1c + 0x298);
      bVar1 = FUN_00461070(local_78,0);
      if (bVar1) {
        local_7c = (void *)(local_1c + 0x298);
        FUN_00429420(local_7c);
      }
      local_80 = (undefined4 *)(local_1c + 0xb8);
      FUN_00423540(local_80);
      local_84 = (undefined4 *)(local_1c + 0xa8);
      FUN_00423540(local_84);
      *(uint *)(local_1c + 0x2cc) = *(uint *)(local_1c + 0x2cc) & 0xfbffffff;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

