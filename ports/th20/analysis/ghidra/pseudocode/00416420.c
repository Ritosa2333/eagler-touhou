/* Entry: 0x00416420; symbol: FUN_00416420; body bytes: 2245 */

void __thiscall
FUN_00416420(void *this,int param_1,int param_2,int param_3,undefined4 param_4,float param_5)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int *unaff_FS_OFFSET;
  int local_dc;
  int local_d8;
  int local_d4;
  int local_d0;
  void *local_cc;
  void *local_c8;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_c4;
  void *local_c0;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_bc;
  void *local_b8;
  int local_b4;
  uint local_b0;
  int local_ac;
  float local_a8;
  uint *local_a4;
  float local_a0;
  char **local_9c;
  int local_98;
  void *local_94;
  void *local_90;
  char **local_8c;
  char **local_88;
  int local_84;
  int local_80;
  uint *local_7c;
  char **local_78;
  int local_74;
  int local_70;
  ushort *local_6c;
  int local_68;
  int local_64;
  int local_60;
  int local_5c;
  ushort *local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  void *local_44;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_40 [16];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_30 [16];
  byte local_20;
  byte local_1f;
  byte local_1e;
  byte local_1d;
  undefined4 local_1c;
  ushort local_18 [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567e75;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_44 = this;
  iVar1 = FUN_004156c0((int)this);
  local_b0 = iVar1 * param_1;
  local_a4 = (uint *)FUN_004156a0((int)local_44);
  local_8c = (char **)FUN_0040d8c0("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\grpfont.cpp:615 u_char"
                                   ,local_b0);
  FUN_00543e20((uint *)local_8c,local_a4,local_b0);
  local_b4 = *(int *)((int)local_44 + 0x100);
  if (local_b4 == 0x15) {
    local_a8 = param_5 * param_5;
    FUN_0040c080(local_40,0x10);
    FUN_004141c0((undefined4 *)local_40);
    local_8 = 0;
    for (local_70 = 0; local_70 < (int)local_a8; local_70 = local_70 + 1) {
      for (local_74 = 0; local_74 < (int)local_a8; local_74 = local_74 + 1) {
        if (((float)(local_70 * local_70 + local_74 * local_74) < local_a8) &&
           ((local_70 != 0 || (local_74 != 0)))) {
          local_d4 = local_70;
          local_d0 = local_74;
          FUN_00416330(local_40,&local_d4);
        }
      }
    }
    local_9c = local_8c;
    FUN_004142a0(&local_1c,param_4);
    iVar1 = FUN_004156c0((int)local_44);
    local_98 = (int)(iVar1 + (iVar1 >> 0x1f & 3U)) >> 2;
    local_1c = local_1c ^ 0xff000000;
    local_50 = 0;
    local_ac = 0;
    while (local_50 < param_1) {
      local_7c = (uint *)(local_ac + param_2 * 4 + (int)local_a4);
      for (local_54 = param_2; local_54 < param_3; local_54 = local_54 + 1) {
        if (*(char *)((int)local_7c + 3) != '\0') {
          local_c4 = local_40;
          local_90 = (void *)FUN_00414f30((int)local_c4);
          local_c8 = (void *)FUN_00414f70((int)local_c4);
          for (; local_90 != local_c8; local_90 = (void *)((int)local_90 + 8)) {
            local_b8 = local_90;
            piVar2 = (int *)FUN_00414580(local_90,0);
            local_60 = *piVar2;
            piVar2 = (int *)FUN_00414580(local_b8,1);
            local_5c = *piVar2;
            if ((((((-1 < local_54 - local_60) && (-1 < local_50 - local_5c)) &&
                  (local_78 = local_9c + (local_50 - local_5c) * local_98 + (local_54 - local_60),
                  *(char *)((int)local_78 + 3) != -1)) ||
                 (((iVar3 = local_54 + local_60, iVar1 = FUN_00415800((int)local_44), iVar3 < iVar1
                   && (-1 < local_50 - local_5c)) &&
                  (local_78 = local_9c + (local_50 - local_5c) * local_98 + local_54 + local_60,
                  *(char *)((int)local_78 + 3) != -1)))) ||
                (((-1 < local_54 - local_60 && (local_50 + local_5c < param_1)) &&
                 (local_78 = local_9c + (local_50 + local_5c) * local_98 + (local_54 - local_60),
                 *(char *)((int)local_78 + 3) != -1)))) ||
               (((iVar3 = local_54 + local_60, iVar1 = FUN_00415800((int)local_44), iVar3 < iVar1 &&
                 (local_50 + local_5c < param_1)) &&
                (local_78 = local_9c + (local_50 + local_5c) * local_98 + local_54 + local_60,
                *(char *)((int)local_78 + 3) != -1)))) {
              *local_7c = local_1c;
              goto LAB_00416631;
            }
          }
          *(undefined1 *)((int)local_7c + 2) = local_1c._2_1_;
          *(undefined1 *)((int)local_7c + 1) = local_1c._1_1_;
          *(undefined1 *)local_7c = (undefined1)local_1c;
        }
LAB_00416631:
        local_7c = local_7c + 1;
      }
      local_50 = local_50 + 1;
      iVar1 = FUN_004156c0((int)local_44);
      local_ac = iVar1 + local_ac;
    }
    local_8 = 0xffffffff;
    FUN_00414440(local_40);
  }
  else if (local_b4 == 0x1a) {
    local_a0 = param_5 * param_5;
    FUN_0040c080(local_30,0x10);
    FUN_004141c0((undefined4 *)local_30);
    local_8 = 1;
    for (local_80 = 0; local_80 < (int)local_a0; local_80 = local_80 + 1) {
      for (local_84 = 0; local_84 < (int)local_a0; local_84 = local_84 + 1) {
        if (((float)(local_80 * local_80 + local_84 * local_84) < local_a0) &&
           ((local_80 != 0 || (local_84 != 0)))) {
          local_dc = local_80;
          local_d8 = local_84;
          FUN_00416330(local_30,&local_dc);
        }
      }
    }
    local_88 = local_8c;
    FUN_004142a0(&local_20,param_4);
    FUN_00414280(local_18);
    local_18[0] = (short)((int)(uint)local_1e >> 4) << 8 | (short)((int)(uint)local_1f >> 4) << 4 |
                  (ushort)((int)(uint)local_20 >> 4) |
                  ((ushort)((int)(uint)local_1d >> 4) ^ 0xf) << 0xc;
    for (local_48 = 0; local_48 < param_1; local_48 = local_48 + 1) {
      iVar1 = FUN_004156c0((int)local_44);
      local_58 = (ushort *)((int)local_a4 + param_2 * 2 + iVar1 * local_48);
      for (local_4c = param_2; local_4c < param_3; local_4c = local_4c + 1) {
        iVar1 = FUN_004156c0((int)local_44);
        local_6c = (ushort *)((int)local_88 + ((iVar1 * local_48) / 2 + local_4c) * 2);
        local_bc = local_30;
        local_94 = (void *)FUN_00414f30((int)local_bc);
        local_cc = (void *)FUN_00414f70((int)local_bc);
        for (; local_94 != local_cc; local_94 = (void *)((int)local_94 + 8)) {
          local_c0 = local_94;
          piVar2 = (int *)FUN_00414580(local_94,0);
          local_68 = *piVar2;
          piVar2 = (int *)FUN_00414580(local_c0,1);
          local_64 = *piVar2;
          if ((-1 < local_4c - local_68) && (-1 < local_48 - local_64)) {
            iVar3 = local_48 - local_64;
            iVar1 = FUN_004156c0((int)local_44);
            local_6c = (ushort *)((int)local_88 + ((iVar1 * iVar3) / 2 + (local_4c - local_68)) * 2)
            ;
            if (*local_6c >> 0xc == 0xf) goto LAB_00416af7;
LAB_00416c87:
            *local_58 = local_18[0];
            goto LAB_004169e6;
          }
LAB_00416af7:
          iVar3 = local_4c + local_68;
          iVar1 = FUN_00415800((int)local_44);
          if ((iVar3 < iVar1) && (-1 < local_48 - local_64)) {
            iVar3 = local_48 - local_64;
            iVar1 = FUN_004156c0((int)local_44);
            local_6c = (ushort *)((int)local_88 + ((iVar1 * iVar3) / 2 + local_4c + local_68) * 2);
            if (*local_6c >> 0xc != 0xf) goto LAB_00416c87;
          }
          if ((-1 < local_4c - local_68) && (local_48 + local_64 < param_1)) {
            iVar3 = local_48 + local_64;
            iVar1 = FUN_004156c0((int)local_44);
            local_6c = (ushort *)((int)local_88 + ((iVar1 * iVar3) / 2 + (local_4c - local_68)) * 2)
            ;
            if (*local_6c >> 0xc != 0xf) goto LAB_00416c87;
          }
          iVar3 = local_4c + local_68;
          iVar1 = FUN_00415800((int)local_44);
          if ((iVar3 < iVar1) && (local_48 + local_64 < param_1)) {
            iVar3 = local_48 + local_64;
            iVar1 = FUN_004156c0((int)local_44);
            local_6c = (ushort *)((int)local_88 + ((iVar1 * iVar3) / 2 + local_4c + local_68) * 2);
            if (*local_6c >> 0xc != 0xf) goto LAB_00416c87;
          }
        }
        *local_58 = *local_58 & 0xf0ff | (local_18[0] >> 8 & 0xf) << 8;
        *local_58 = *local_58 & 0xff0f | (local_18[0] >> 4 & 0xf) << 4;
        *local_58 = *local_58 & 0xfff0 | local_18[0] & 0xf;
LAB_004169e6:
        local_58 = local_58 + 1;
      }
    }
    local_8 = 0xffffffff;
    FUN_00414440(local_30);
  }
  FUN_00413b70(DAT_005b8894,local_8c);
  local_8c = (char **)0x0;
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

