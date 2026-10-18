/* Entry: 0x004c0480; symbol: FUN_004c0480; body bytes: 1707 */

void __thiscall
FUN_004c0480(void *this,float *param_1,float *param_2,float param_3,float param_4,
            undefined4 *param_5,undefined4 *param_6,int param_7,int param_8)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  undefined4 *puVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  int *unaff_FS_OFFSET;
  undefined4 local_d0 [2];
  int aiStack_c8 [13];
  int *local_94;
  void *local_90;
  void *local_8c;
  default_delete<class_std::_Facet_base> *local_88;
  void *local_84;
  int local_80;
  void *local_7c;
  code *local_78;
  undefined4 local_74;
  int local_70;
  int local_6c;
  undefined4 local_68;
  int local_64;
  void *local_60;
  void *local_5c;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  void *local_44;
  void *local_40;
  void *local_3c;
  int local_38;
  int local_34;
  int local_30;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_2c;
  float local_28 [3];
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569a0d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_3c = this;
  iVar2 = FUN_0040ff90(*(int *)((int)this + 0xc45c));
  iVar2 = FUN_004c0ce0(iVar2);
  if (iVar2 != 0) {
    local_30 = 0;
    local_90 = (void *)FUN_00424010(*(int *)((int)local_3c + 0xc45c));
    local_38 = FUN_00477cf0(local_90,param_1,param_2);
    if ((param_5 != (undefined4 *)0x0) && (*param_5 = 0, 0 < local_38)) {
      *param_5 = 1;
    }
    local_30 = local_30 + local_38;
    aiStack_c8[9] = 0;
    aiStack_c8[10] = 0;
    aiStack_c8[0xb] = 0;
    aiStack_c8[0xc] = 0;
    aiStack_c8[5] = 0;
    aiStack_c8[6] = 0;
    aiStack_c8[7] = 0;
    aiStack_c8[8] = 0;
    aiStack_c8[1] = 0;
    aiStack_c8[2] = 0;
    aiStack_c8[3] = 0;
    aiStack_c8[4] = 0;
    local_48 = 0;
    local_44 = (void *)((int)local_3c + 0xc414);
    FUN_0040c080(local_1c,8);
    FUN_00412280(local_44,local_1c);
    local_8 = 0;
    local_94 = (int *)FUN_00412540();
    while (bVar1 = FUN_00411bb0(local_1c,local_94), bVar1) {
      local_2c = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)FUN_0040c300(local_1c);
      local_48 = local_48 + 1;
      piVar3 = (int *)FUN_0040f160(local_2c);
      local_5c = (void *)(*piVar3 + 0x7c);
      piVar3 = (int *)FUN_0040f160(local_2c);
      local_58 = *(int *)(*piVar3 + 0xa0);
      iVar2 = FUN_00478e00(local_5c,local_58);
      if (((iVar2 != 0) && (piVar3 = (int *)FUN_0040f160(local_2c), *(int *)(*piVar3 + 0xa8) < 1))
         && (piVar3 = (int *)FUN_0040f160(local_2c), *(int *)(*piVar3 + 0x94) != 0)) {
        puVar4 = (undefined4 *)FUN_0040f160(local_2c);
        local_60 = (void *)*puVar4;
        uVar5 = FUN_004c1030(local_60,param_1,param_2,param_3,param_4);
        if ((uVar5 & 0xff) != 0) {
          piVar3 = (int *)FUN_0040f160(local_2c);
          if ((0 < *(int *)(*piVar3 + 0xac)) &&
             (piVar3 = (int *)FUN_0040f160(local_2c), *(int *)(*piVar3 + 0xac) < 5)) {
            piVar3 = (int *)FUN_0040f160(local_2c);
            if (aiStack_c8[*(int *)(*piVar3 + 0xac) + 8] != 0) {
              piVar3 = (int *)FUN_0040f160(local_2c);
              iVar2 = *(int *)(*piVar3 + 0xac);
              piVar3 = (int *)FUN_0040f160(local_2c);
              if (*(int *)(*piVar3 + 0x94) < aiStack_c8[iVar2 + 4]) goto LAB_004c05df;
            }
            piVar3 = (int *)FUN_0040f160(local_2c);
            aiStack_c8[*(int *)(*piVar3 + 0xac) + 8] = 1;
            piVar3 = (int *)FUN_0040f160(local_2c);
            local_30 = local_30 - aiStack_c8[*(int *)(*piVar3 + 0xac)];
            piVar3 = (int *)FUN_0040f160(local_2c);
            local_64 = *(int *)(*piVar3 + 0x94);
            piVar3 = (int *)FUN_0040f160(local_2c);
            aiStack_c8[*(int *)(*piVar3 + 0xac) + 4] = local_64;
          }
          iVar2 = FUN_0040c300(&param_8);
          if (iVar2 == 0) {
LAB_004c082c:
            if ((param_5 != (undefined4 *)0x0) &&
               (piVar3 = (int *)FUN_0040f160(local_2c), (*(uint *)(*piVar3 + 0x14) >> 4 & 1) != 0))
            {
              *param_5 = 1;
            }
            piVar3 = (int *)FUN_0040f160(local_2c);
            local_34 = *piVar3;
            piVar3 = (int *)FUN_0040f160(local_2c);
            if (*(int *)(*piVar3 + 0x9c) < 9999999) {
              piVar3 = (int *)FUN_0040f160(local_2c);
              iVar2 = *piVar3;
              piVar3 = (int *)FUN_0040f160(local_2c);
              if (*(int *)(iVar2 + 0x9c) <= *(int *)(*piVar3 + 0x98)) {
                puVar4 = (undefined4 *)FUN_0040f160(local_2c);
                FUN_004c1f30((char **)*puVar4);
              }
            }
            local_38 = *(int *)(local_34 + 0x94);
            if (param_7 == 0) {
              piVar3 = (int *)FUN_0040f160(local_2c);
              local_70 = *(int *)(*piVar3 + 0x94);
              local_6c = *(int *)(local_34 + 0x98);
              *(int *)(local_34 + 0x98) = local_6c + local_70;
              if (*(int *)(local_34 + 0xb4) != 0) {
                *(int *)(local_34 + 0xb0) = param_8;
                local_78 = *(code **)(&DAT_00570c78 + *(int *)(local_34 + 0xb4) * 4);
                puVar4 = (undefined4 *)FUN_0040f160(local_2c);
                local_74 = *puVar4;
                local_50 = (*local_78)(local_74,param_1,param_2,param_3,param_4);
                if (-1 < local_50) {
                  local_38 = local_50;
                }
              }
            }
            if ((0 < *(int *)(local_34 + 0xac)) && (*(int *)(local_34 + 0xac) < 5)) {
              aiStack_c8[*(int *)(local_34 + 0xac)] = local_38;
            }
            local_30 = local_30 + local_38;
            if (param_6 != (undefined4 *)0x0) {
              local_7c = (void *)(local_34 + 0x34);
              puVar4 = FUN_0048bd70(local_7c,local_d0);
              *param_6 = *puVar4;
              param_6[1] = puVar4[1];
              param_6[2] = puVar4[2];
            }
            if (0 < local_30) {
              local_84 = (void *)FUN_00412730(*(int *)((int)local_3c + 0xc45c));
              local_80 = FUN_0040c300(&param_8);
              local_40 = (void *)FUN_00498a80(local_84,local_80);
              if (local_40 != (void *)0x0) {
                local_88 = std::
                           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                         *)(local_34 + 0x34));
                FUN_00429440(local_88,local_28,param_1);
                FUN_004c0240(local_28,DAT_0056c8d0);
                FUN_004296e0(local_28,param_1);
                local_8c = (void *)FUN_00464230(0);
                uVar5 = FUN_004c1ac0((int)local_40);
                if ((uVar5 & 0xff) == 0) {
                  local_54 = local_30;
                }
                else {
                  local_54 = local_30 * 10;
                }
                FUN_004a9f80(local_8c,local_28,local_54,0xd);
                if ((*(uint *)(local_34 + 0x14) >> 6 & 1) != 0) {
                  FUN_004c1fc0(local_40,1);
                }
              }
            }
          }
          else {
            piVar3 = (int *)FUN_0040f160(local_2c);
            iVar2 = FUN_0040c300((undefined4 *)(*piVar3 + 0xa4));
            iVar6 = FUN_0040c300(&param_8);
            if (iVar2 != iVar6) {
              uVar7 = FUN_0040c300(&param_8);
              local_68 = uVar7;
              piVar3 = (int *)FUN_0040f160(local_2c);
              FUN_004117a0((void *)(*piVar3 + 0xa4),uVar7);
              local_4c = FUN_004aaac0(&param_8);
              if ((local_4c == 0) ||
                 (piVar3 = (int *)FUN_0040f160(local_2c), (*(uint *)(*piVar3 + 0x14) >> 5 & 1) == 0)
                 ) goto LAB_004c082c;
              iVar2 = FUN_004aabd0(local_4c);
              FUN_004c1e90(iVar2);
            }
          }
        }
      }
LAB_004c05df:
      FUN_00411c30(local_1c);
    }
    local_8 = 0xffffffff;
    FUN_00411b00(local_1c);
    iVar2 = FUN_0040ff90(*(int *)((int)local_3c + 0xc45c));
    iVar2 = FUN_004ff4e0(iVar2);
    if (iVar2 < local_30) {
      iVar2 = FUN_0040ff90(*(int *)((int)local_3c + 0xc45c));
      local_30 = FUN_004ff4e0(iVar2);
    }
    if ((param_7 == 0) && (local_30 != 0)) {
      FUN_00488550(&DAT_005ba568,local_30 / 10 + 10);
    }
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

