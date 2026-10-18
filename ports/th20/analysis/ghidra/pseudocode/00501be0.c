/* Entry: 0x00501be0; symbol: FUN_00501be0; body bytes: 844 */

void __fastcall FUN_00501be0(void *param_1)

{
  bool bVar1;
  int iVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  int *piVar4;
  undefined3 extraout_var;
  float *pfVar5;
  undefined4 *puVar6;
  int *unaff_FS_OFFSET;
  float10 fVar7;
  undefined4 local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  float local_44;
  void *local_40;
  void *local_3c;
  int *local_38;
  void *local_34;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_30;
  void *local_2c;
  float local_28;
  float local_24;
  undefined4 local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a5dd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_2c = param_1;
  if (*(int *)((int)param_1 + 0x9c) != 2) {
    if ((*(uint *)((int)param_1 + 0x3c) >> 2 & 0xf) == 0) {
      iVar2 = FUN_00412730(*(int *)((int)param_1 + 0x120));
      if (iVar2 == 0) {
        FUN_004117a0((void *)((int)local_2c + 0xa0),0);
      }
      else {
        iVar2 = FUN_0040c300((undefined4 *)((int)local_2c + 0xa0));
        if (iVar2 == 0) {
          pdVar3 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)local_2c + 0x50));
          local_28 = *(float *)pdVar3;
          local_24 = *(float *)(pdVar3 + 4);
          local_20 = *(undefined4 *)(pdVar3 + 8);
          iVar2 = FUN_00412730(*(int *)((int)local_2c + 0x120));
          local_34 = (void *)FUN_004fce40(iVar2);
          FUN_0040c080(local_1c,8);
          FUN_00412280(local_34,local_1c);
          local_8 = 0;
          local_38 = (int *)FUN_00412540();
          while (bVar1 = FUN_00411bb0(local_1c,local_38), bVar1) {
            local_30 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)FUN_0040c300(local_1c);
            piVar4 = (int *)FUN_0040f160(local_30);
            bVar1 = FUN_004b8470(*piVar4);
            if (CONCAT31(extraout_var,bVar1) != 0) {
              piVar4 = (int *)FUN_0040f160(local_30);
              iVar2 = FUN_0047a3e0(*piVar4);
              if (iVar2 == 0) {
                piVar4 = (int *)FUN_0040f160(local_30);
                iVar2 = FUN_0047a2c0(*piVar4);
                if (*(float *)(iVar2 + 4) - DAT_0056cd90 <= local_24) {
                  piVar4 = (int *)FUN_0040f160(local_30);
                  iVar2 = FUN_0047a2c0(*piVar4);
                  if (local_24 <= *(float *)(iVar2 + 4) + DAT_0056cd90) {
                    piVar4 = (int *)FUN_0040f160(local_30);
                    pfVar5 = (float *)FUN_0047a2c0(*piVar4);
                    if (*pfVar5 - DAT_0056cd90 < local_28) {
                      piVar4 = (int *)FUN_0040f160(local_30);
                      pfVar5 = (float *)FUN_0047a2c0(*piVar4);
                      if (local_28 < *pfVar5 + DAT_0056cd90) goto LAB_00501ccc;
                    }
                    *(uint *)((int)local_2c + 0x3c) =
                         *(uint *)((int)local_2c + 0x3c) & 0xffffffc3 | 4;
                    puVar6 = FUN_005064b0(local_2c,&local_54);
                    FUN_00486310(puVar6);
                    FUN_00506a70(local_2c,0);
                    local_3c = (void *)((int)local_2c + 0x50);
                    FUN_0047a5c0(local_3c,0);
                    piVar4 = (int *)FUN_0040f160(local_30);
                    puVar6 = (undefined4 *)FUN_0047a2c0(*piVar4);
                    *(undefined4 *)((int)local_2c + 0x100) = *puVar6;
                    *(undefined4 *)((int)local_2c + 0x104) = puVar6[1];
                    *(undefined4 *)((int)local_2c + 0x108) = puVar6[2];
                    break;
                  }
                }
              }
            }
LAB_00501ccc:
            FUN_00411c30(local_1c);
          }
          local_8 = 0xffffffff;
          FUN_00411b00(local_1c);
        }
      }
    }
    if ((*(uint *)((int)local_2c + 0x3c) >> 2 & 0xf) == 1) {
      local_40 = (void *)FUN_0044cb60((int)local_2c);
      bVar1 = FUN_004785e0(local_40,4);
      if (bVar1) {
        fVar7 = FUN_004292e0((float *)((int)local_2c + 0x50));
        local_44 = (float)fVar7;
        if (local_44 < *(float *)((int)local_2c + 0x100) ||
            local_44 == *(float *)((int)local_2c + 0x100)) {
          local_4c = (void *)((int)local_2c + 0x50);
          FUN_0047a560(local_4c,0.0);
        }
        else {
          local_48 = (void *)((int)local_2c + 0x50);
          FUN_0047a560(local_48,DAT_0056e0f8);
        }
        local_50 = (void *)((int)local_2c + 0x50);
        FUN_0047a5c0(local_50,DAT_0056f7ac);
        *(uint *)((int)local_2c + 0x3c) = *(uint *)((int)local_2c + 0x3c) & 0xffffffc3 | 8;
      }
      FUN_00505fa0(local_2c,1);
    }
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

