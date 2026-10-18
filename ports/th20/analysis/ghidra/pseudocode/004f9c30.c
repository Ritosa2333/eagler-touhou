/* Entry: 0x004f9c30; symbol: FUN_004f9c30; body bytes: 3106 */

undefined4 __fastcall FUN_004f9c30(void *param_1)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 local_114;
  undefined4 local_110;
  undefined4 local_10c;
  undefined4 local_108;
  undefined4 local_104;
  default_delete<class_std::_Facet_base> *local_100;
  default_delete<class_std::_Facet_base> *local_fc;
  default_delete<class_std::_Facet_base> *local_f8;
  default_delete<class_std::_Facet_base> *local_f4;
  void *local_f0;
  void *local_ec;
  void *local_e8;
  void *local_e4;
  void *local_e0;
  void *local_dc;
  void *local_d8;
  void *local_d4;
  undefined4 local_d0;
  void *local_cc;
  void *local_c8;
  void *local_c4;
  void *local_c0;
  void *local_bc;
  void *local_b8;
  void *local_b4;
  void *local_b0;
  void *local_ac;
  void *local_a8;
  void *local_a4;
  void *local_a0;
  void *local_9c;
  void *local_98;
  void *local_94;
  void *local_90;
  void *local_8c;
  void *local_88;
  void *local_84;
  undefined4 *local_80;
  void *local_7c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_78;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_74;
  int local_70;
  int local_6c;
  int local_68;
  int local_64;
  uint local_60;
  uint local_5c;
  uint local_58;
  uint local_54;
  uint local_50;
  uint local_4c;
  uint local_48;
  uint local_44;
  uint local_40;
  uint local_3c;
  uint local_38;
  void *local_34;
  default_delete<class_std::_Facet_base> *local_30;
  default_delete<class_std::_Facet_base> *local_2c;
  int local_28;
  undefined1 local_21;
  int local_20;
  int local_1c;
  int local_18;
  void *local_14;
  
  local_1c = 0;
  local_20 = 0;
  local_14 = param_1;
  local_18 = FUN_00498fb0(DAT_005b8898,*(int *)((int)param_1 + 0x14854));
  uVar2 = FUN_004ff870((int)local_14);
  if (uVar2 != 0) {
    local_7c = (void *)((int)local_14 + 0x2070);
    bVar1 = FUN_004785e0(local_7c,0);
    if (bVar1) {
      FUN_004bd0c0((int)local_14);
    }
    local_80 = (undefined4 *)((int)local_14 + 0x2070);
    FUN_00423540(local_80);
    uVar2 = FUN_004ff870((int)local_14);
    if (uVar2 == 1) {
      local_84 = (void *)((int)local_14 + 0x2070);
      bVar1 = FUN_004785e0(local_84,1);
      if (bVar1) {
        FUN_00426d70(&DAT_005ba830,0x51,0);
      }
      local_1c = -0x100;
      local_20 = 0;
      local_88 = (void *)((int)local_14 + 0x2070);
      bVar1 = FUN_004785e0(local_88,8);
      if (bVar1) {
        local_1c = 0xc000;
        FUN_004fb290(local_14,1);
        FUN_004fb340(local_14,1);
      }
    }
    else {
      local_8c = (void *)((int)local_14 + 0x2070);
      bVar1 = FUN_004785e0(local_8c,1);
      if (bVar1) {
        FUN_00426d70(&DAT_005ba830,0x50,0);
      }
      local_1c = 0x100;
      local_20 = 0;
      local_90 = (void *)((int)local_14 + 0x2070);
      bVar1 = FUN_004785e0(local_90,8);
      if (bVar1) {
        local_1c = -0xc000;
        FUN_004fb290(local_14,1);
        FUN_004fb340(local_14,1);
      }
    }
    local_94 = (void *)((int)local_14 + 0x2070);
    FUN_004785e0(local_94,8);
    local_98 = (void *)((int)local_14 + 0x2070);
    bVar1 = FUN_004235c0(local_98,0x10);
    if (!bVar1) goto LAB_004fa4e1;
    FUN_004ff8f0((int)local_14);
    FUN_004fff10(local_14,0);
  }
  if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
    local_38 = 0;
  }
  else {
    local_9c = (void *)(&DAT_005b889c)[local_18];
    local_38 = FUN_004973a0(local_9c,0x50);
  }
  if (local_38 == 0x50) {
    *(undefined4 *)((int)local_14 + 0x67c) = 5;
  }
  else {
    if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
      local_3c = 0;
    }
    else {
      local_a0 = (void *)(&DAT_005b889c)[local_18];
      local_3c = FUN_004973a0(local_a0,0x60);
    }
    if (local_3c == 0x60) {
      *(undefined4 *)((int)local_14 + 0x67c) = 7;
    }
    else {
      if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
        local_40 = 0;
      }
      else {
        local_a4 = (void *)(&DAT_005b889c)[local_18];
        local_40 = FUN_004973a0(local_a4,0x90);
      }
      if (local_40 == 0x90) {
        *(undefined4 *)((int)local_14 + 0x67c) = 6;
      }
      else {
        if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
          local_44 = 0;
        }
        else {
          local_a8 = (void *)(&DAT_005b889c)[local_18];
          local_44 = FUN_004973a0(local_a8,0xa0);
        }
        if (local_44 == 0xa0) {
          *(undefined4 *)((int)local_14 + 0x67c) = 8;
        }
        else {
          if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
            local_48 = 0;
          }
          else {
            local_ac = (void *)(&DAT_005b889c)[local_18];
            local_48 = FUN_004973a0(local_ac,0x20);
          }
          if (local_48 == 0) {
            if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
              local_4c = 0;
            }
            else {
              local_b0 = (void *)(&DAT_005b889c)[local_18];
              local_4c = FUN_004973a0(local_b0,0x10);
            }
            if (local_4c == 0) {
              if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
                local_50 = 0;
              }
              else {
                local_b4 = (void *)(&DAT_005b889c)[local_18];
                local_50 = FUN_004973a0(local_b4,0x40);
              }
              if (local_50 == 0) {
                if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
                  local_54 = 0;
                }
                else {
                  local_b8 = (void *)(&DAT_005b889c)[local_18];
                  local_54 = FUN_004973a0(local_b8,0x80);
                }
                if (local_54 == 0) {
                  *(undefined4 *)((int)local_14 + 0x67c) = 0;
                }
                else {
                  *(undefined4 *)((int)local_14 + 0x67c) = 4;
                }
              }
              else {
                *(undefined4 *)((int)local_14 + 0x67c) = 3;
              }
            }
            else {
              *(undefined4 *)((int)local_14 + 0x67c) = 1;
            }
          }
          else {
            *(undefined4 *)((int)local_14 + 0x67c) = 2;
          }
        }
      }
    }
  }
  iVar3 = FUN_00412730(*(int *)((int)local_14 + 0x14858));
  if (iVar3 == 0) {
LAB_004fa1fc:
    *(undefined1 *)((int)local_14 + 0x204c) = 0;
    *(undefined4 *)((int)local_14 + 0x20e4) = 0x1e;
  }
  else {
    iVar3 = FUN_00412730(*(int *)((int)local_14 + 0x14858));
    iVar3 = FUN_00478080(iVar3);
    if (iVar3 == 0) goto LAB_004fa1fc;
    local_bc = (void *)((int)local_14 + 0x654);
    bVar1 = FUN_004235c0(local_bc,4);
    if (!bVar1) goto LAB_004fa1fc;
    if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
      local_58 = 0;
    }
    else {
      local_c0 = (void *)(&DAT_005b889c)[local_18];
      local_58 = FUN_004973a0(local_c0,8);
    }
    local_5c = (uint)(local_58 != 0);
    if (*(byte *)((int)local_14 + 0x204c) != local_5c) {
      *(undefined4 *)((int)local_14 + 0x630) = *(undefined4 *)((int)local_14 + 0x620);
      *(undefined4 *)((int)local_14 + 0x634) = *(undefined4 *)((int)local_14 + 0x624);
    }
    if ((local_18 < 0) || ((&DAT_005b889c)[local_18] == 0)) {
      local_60 = 0;
    }
    else {
      local_c4 = (void *)(&DAT_005b889c)[local_18];
      local_60 = FUN_004973a0(local_c4,8);
    }
    local_21 = local_60 != 0;
    *(undefined1 *)((int)local_14 + 0x204c) = local_21;
  }
  local_1c = *(int *)(&UNK_00572c48 + *(int *)((int)local_14 + 0x67c) * 8);
  local_20 = *(int *)(&UNK_00572c4c + *(int *)((int)local_14 + 0x67c) * 8);
  bVar1 = FUN_004ff5e0((int)local_14);
  if (bVar1) {
    local_c8 = (void *)((int)local_14 + 0x60c);
    bVar1 = FUN_00449430(local_c8,0);
    if (bVar1) {
      local_cc = (void *)FUN_0041cad0(*(int *)((int)local_14 + 0x14858));
      local_d0 = FUN_0045d100(local_cc,0);
      puVar4 = FUN_00450c70(&local_114,(uint *)"effect",0x13,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_14 + 0x60c) = *puVar4;
    }
    local_34 = (void *)FUN_0044ced0((undefined4 *)((int)local_14 + 0x60c));
    if (local_34 != (void *)0x0) {
      uVar2 = FUN_004ff800((int)local_14);
      if (uVar2 == 0) {
        FUN_004864f0(local_34,DAT_0056c8cc,DAT_0056c8cc);
      }
      else {
        FUN_004864f0(local_34,(*(float *)((int)local_14 + 0x2238) - DAT_0056c8cc) * DAT_0056c8d0 +
                              DAT_0056c8cc,
                     (*(float *)((int)local_14 + 0x2238) - DAT_0056c8cc) * DAT_0056c8d0 +
                     DAT_0056c8cc);
      }
    }
    if (*(int *)((int)local_14 + 0x67c) < 5) {
      local_64 = *(int *)((int)local_14 + 0x20b8);
    }
    else {
      local_64 = *(int *)((int)local_14 + 0x20c0);
    }
    local_1c = local_1c * local_64;
    if (*(int *)((int)local_14 + 0x67c) < 5) {
      local_68 = *(int *)((int)local_14 + 0x20b8);
    }
    else {
      local_68 = *(int *)((int)local_14 + 0x20c0);
    }
    local_20 = local_20 * local_68;
  }
  else {
    bVar1 = FUN_0047a340((undefined4 *)((int)local_14 + 0x60c));
    if (bVar1) {
      FUN_0044ee90(*(undefined4 *)((int)local_14 + 0x60c),1);
      FUN_004117a0((void *)((int)local_14 + 0x60c),0);
    }
    if (*(int *)((int)local_14 + 0x67c) < 5) {
      local_6c = *(int *)((int)local_14 + 0x20b4);
    }
    else {
      local_6c = *(int *)((int)local_14 + 0x20bc);
    }
    local_1c = local_1c * local_6c;
    if (*(int *)((int)local_14 + 0x67c) < 5) {
      local_70 = *(int *)((int)local_14 + 0x20b4);
    }
    else {
      local_70 = *(int *)((int)local_14 + 0x20bc);
    }
    local_20 = local_20 * local_70;
  }
  local_1c = (int)((float)((int)(*(float *)((int)local_14 + 0x20f0) * DAT_0056cd98) + local_1c) *
                  *(float *)((int)local_14 + 0x20ec));
  local_20 = (int)((float)((int)(*(float *)((int)local_14 + 0x20f4) * DAT_0056cd98) + local_20) *
                  *(float *)((int)local_14 + 0x20ec));
LAB_004fa4e1:
  if ((local_1c < 0) && (-1 < *(int *)((int)local_14 + 0x674))) {
    local_d4 = *(void **)((int)local_14 + 0x1c);
    FUN_004382b0(local_d4,(void *)((int)local_14 + 0x28),*(int *)((int)local_14 + 0x22a4),0);
  }
  else if ((local_1c == 0) && (*(int *)((int)local_14 + 0x674) < 0)) {
    local_d8 = *(void **)((int)local_14 + 0x1c);
    FUN_004382b0(local_d8,(void *)((int)local_14 + 0x28),*(int *)((int)local_14 + 0x22a8),0);
  }
  if ((local_1c < 1) || (0 < *(int *)((int)local_14 + 0x674))) {
    if ((local_1c == 0) && (0 < *(int *)((int)local_14 + 0x674))) {
      local_e0 = *(void **)((int)local_14 + 0x1c);
      FUN_004382b0(local_e0,(void *)((int)local_14 + 0x28),*(int *)((int)local_14 + 0x22b0),0);
    }
  }
  else {
    local_dc = *(void **)((int)local_14 + 0x1c);
    FUN_004382b0(local_dc,(void *)((int)local_14 + 0x28),*(int *)((int)local_14 + 0x22ac),0);
  }
  *(int *)((int)local_14 + 0x674) = local_1c;
  *(int *)((int)local_14 + 0x678) = local_20;
  FUN_004ffa10(local_14,local_1c,local_20);
  bVar1 = FUN_0047a340((undefined4 *)((int)local_14 + 0x60c));
  if (bVar1) {
    local_e4 = (void *)((int)local_14 + 0x60c);
    puVar4 = (undefined4 *)FUN_00460850((int)local_14);
    FUN_004645e0(local_e4,puVar4);
  }
  if ((local_20 != 0) || (local_1c != 0)) {
    local_28 = 0;
    while (iVar3 = FUN_004fffe0(), local_28 < iVar3 + -1) {
      local_e8 = (void *)((int)local_14 + 0x20fc);
      iVar3 = FUN_004fffe0();
      puVar4 = (undefined4 *)FUN_0045d670(local_e8,(iVar3 + -2) - local_28);
      local_108 = *puVar4;
      local_104 = puVar4[1];
      local_ec = (void *)((int)local_14 + 0x20fc);
      iVar3 = FUN_004fffe0();
      puVar4 = (undefined4 *)FUN_0045d670(local_ec,(iVar3 + -1) - local_28);
      *puVar4 = local_108;
      puVar4[1] = local_104;
      local_28 = local_28 + 1;
    }
    puVar4 = (undefined4 *)FUN_004ff690((int)local_14);
    local_110 = *puVar4;
    local_10c = puVar4[1];
    local_f0 = (void *)((int)local_14 + 0x20fc);
    puVar4 = (undefined4 *)FUN_0045d670(local_f0,0);
    *puVar4 = local_110;
    puVar4[1] = local_10c;
  }
  if ((*(uint *)((int)local_14 + 0x14) >> 1 & 1) != 0) {
    *(int *)((int)local_14 + 0x20e8) = *(int *)((int)local_14 + 0x20e8) + 1;
  }
  local_74 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)((int)local_14 + 0x684);
  local_2c = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(local_74);
  local_f4 = (default_delete<class_std::_Facet_base> *)FUN_004fce60((int)local_74);
  for (; local_2c != local_f4; local_2c = local_2c + 300) {
    local_f8 = local_2c;
    FUN_004fa860((int *)local_2c);
  }
  local_78 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)((int)local_14 + 0x123c);
  local_30 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(local_78);
  local_fc = (default_delete<class_std::_Facet_base> *)FUN_004fce80((int)local_78);
  for (; local_30 != local_fc; local_30 = local_30 + 300) {
    local_100 = local_30;
    FUN_004fa860((int *)local_30);
  }
  if (0x1d < *(int *)((int)local_14 + 0x20e8)) {
    *(undefined4 *)((int)local_14 + 0x680) = 0;
  }
  return 0;
}

