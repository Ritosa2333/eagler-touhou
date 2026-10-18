/* Entry: 0x00529430; symbol: FUN_00529430; body bytes: 1772 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00529430(void *param_1)

{
  char cVar1;
  bool bVar2;
  undefined3 extraout_var;
  int iVar3;
  undefined4 *puVar4;
  undefined4 local_68;
  void *local_64;
  void *local_60;
  undefined4 local_5c;
  void *local_58;
  void *local_54;
  int local_50;
  int local_4c;
  int local_48;
  int local_44;
  void *local_40;
  void *local_3c;
  void *local_38;
  void *local_34;
  void *local_30;
  void *local_2c;
  void *local_28;
  void *local_24;
  uint local_20;
  uint local_1c;
  int local_18;
  int local_14;
  undefined4 local_10;
  undefined4 local_c;
  void *local_8;
  
  local_10 = *(undefined4 *)((int)param_1 + 0x20);
  local_8 = param_1;
  switch(local_10) {
  case 0:
    local_24 = (void *)((int)param_1 + 0x24);
    FUN_0049c4e0(local_24,6);
    FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005b0a50);
    FUN_0052cf80(local_8,0x2b);
    FUN_0052ce50(local_8,1);
    if (DAT_005c6128 == 4) {
      DAT_005c6128 = 1;
    }
  case 1:
    local_28 = (void *)((int)local_8 + 0x154);
    bVar2 = FUN_00461070(local_28,10);
    if (bVar2) {
      FUN_0052ce50(local_8,2);
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
    if (DAT_005b889c == (void *)0x0) {
      local_14 = 0;
    }
    else {
      local_2c = DAT_005b889c;
      local_14 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_14 != 0) {
      local_30 = (void *)((int)local_8 + 0x24);
      FUN_004bf040(local_30);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_18 = 0;
    }
    else {
      local_34 = DAT_005b889c;
      local_18 = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_18 != 0) {
      local_38 = (void *)((int)local_8 + 0x24);
      FUN_004bf020(local_38);
    }
    bVar2 = FUN_0045d030((int *)((int)local_8 + 0x24));
    if (CONCAT31(extraout_var,bVar2) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_1c = 0;
    }
    else {
      local_3c = DAT_005b889c;
      local_1c = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_1c == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_20 = 0;
      }
      else {
        local_40 = DAT_005b889c;
        local_20 = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if (local_20 != 0) {
        local_44 = FUN_0050fc50(DAT_005c6108);
        local_44 = local_44 + 0x76f8;
        local_48 = FUN_004640a0(0x5ba568);
        local_48 = local_48 * 0x90;
        local_4c = local_44 + local_48;
        local_50 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        local_50 = local_50 * 0x10;
        cVar1 = FUN_0052ca30(local_4c + local_50);
        if (cVar1 == '\0') {
          FUN_00426d70(&DAT_005ba830,0x10,0);
        }
        else {
          FUN_0052ce50(local_8,3);
          FUN_00426d70(&DAT_005ba830,7,0);
          FUN_00426d70(&DAT_005ba830,0x32,0);
          DAT_005b0a50 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
          local_c = 0;
          FUN_0052ceb0(&DAT_005ba568,0);
          iVar3 = FUN_0051f310(&DAT_005c6148);
          if (iVar3 == 0) {
            if ((DAT_005c6179 & 0x80) == 0) {
              if ((DAT_005c617a & 0x80) == 0) {
                if ((DAT_005c617b & 0x80) == 0) {
                  if ((DAT_005c617c & 0x80) == 0) {
                    if ((DAT_005c617d & 0x80) == 0) {
                      if ((DAT_005c617e & 0x80) == 0) {
                        if ((DAT_005c617f & 0x80) == 0) {
                          if ((DAT_005c6180 & 0x80) == 0) {
                            if ((DAT_005c6181 & 0x80) != 0) {
                              local_c = 9;
                            }
                          }
                          else {
                            local_c = 8;
                          }
                        }
                        else {
                          local_c = 7;
                        }
                      }
                      else {
                        local_c = 6;
                      }
                    }
                    else {
                      local_c = 5;
                    }
                  }
                  else {
                    local_c = 4;
                  }
                }
                else {
                  local_c = 3;
                }
              }
              else {
                local_c = 2;
              }
            }
            else {
              local_c = 1;
            }
          }
          else if ((DAT_005c614a & 0x80) == 0) {
            if ((DAT_005c614b & 0x80) == 0) {
              if ((DAT_005c614c & 0x80) == 0) {
                if ((DAT_005c614d & 0x80) == 0) {
                  if ((DAT_005c614e & 0x80) == 0) {
                    if ((DAT_005c614f & 0x80) == 0) {
                      if ((DAT_005c6150 & 0x80) == 0) {
                        if ((DAT_005c6151 & 0x80) == 0) {
                          if ((DAT_005c6152 & 0x80) != 0) {
                            local_c = 9;
                          }
                        }
                        else {
                          local_c = 8;
                        }
                      }
                      else {
                        local_c = 7;
                      }
                    }
                    else {
                      local_c = 6;
                    }
                  }
                  else {
                    local_c = 5;
                  }
                }
                else {
                  local_c = 4;
                }
              }
              else {
                local_c = 3;
              }
            }
            else {
              local_c = 2;
            }
          }
          else {
            local_c = 1;
          }
          FUN_0052ceb0(&DAT_005ba568,local_c);
          FUN_004d99d0(DAT_0056fe5c);
        }
      }
    }
    else {
      FUN_0052ce50(local_8,4);
      FUN_00426d70(&DAT_005ba830,9,0);
      DAT_005b0a50 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    }
    break;
  case 3:
    iVar3 = FUN_00437520(0);
    iVar3 = FUN_00438520(iVar3);
    if (iVar3 == 0) {
      local_54 = (void *)((int)local_8 + 0x154);
      FUN_00429420(local_54);
    }
    else {
      local_58 = (void *)((int)local_8 + 0x154);
      bVar2 = FUN_004785e0(local_58,10);
      if (bVar2) {
        FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
        local_5c = FUN_00437520(0);
        puVar4 = FUN_0049dcf0(&local_68,0,0,0);
        _DAT_005c4f04 = *puVar4;
        FUN_0051c6b0(&DAT_005c4f04,0);
      }
      local_60 = (void *)((int)local_8 + 0x154);
      bVar2 = FUN_004235c0(local_60,0x28);
      if (bVar2) {
        FUN_004c60b0((int)local_8 + 0x24);
        FUN_0052ced0(local_8,2);
        iVar3 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        FUN_004be360(&DAT_005ba568,iVar3 + 1);
        DAT_005afcfc = 0xffffffff;
        FUN_004be250(&DAT_005c4d40,7);
        DAT_005c6128 = 4;
        DAT_005b0a50 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      }
    }
    break;
  case 4:
    local_64 = (void *)((int)param_1 + 0x154);
    bVar2 = FUN_004235c0(local_64,6);
    if (bVar2) {
      FUN_0052cc30(local_8,0x2b);
      FUN_0052ced0(local_8,7);
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
    }
  }
  return 1;
}

