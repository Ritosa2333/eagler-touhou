/* Entry: 0x0052a8a0; symbol: FUN_0052a8a0; body bytes: 3017 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0052a8a0(void *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined *puVar2;
  undefined4 uVar3;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar4;
  undefined4 *puVar5;
  uint uVar6;
  undefined4 local_a8;
  undefined4 local_a4;
  void *local_a0;
  int local_9c;
  int local_98;
  void *local_94;
  void *local_90;
  void *local_8c;
  undefined4 local_88;
  void *local_84;
  void *local_80;
  void *local_7c;
  void *local_78;
  uint local_74;
  void *local_70;
  uint local_6c;
  void *local_68;
  void *local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  void *local_44;
  undefined4 local_40;
  void *local_3c;
  uint local_38;
  uint local_34;
  uint local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  undefined4 local_1c;
  int local_18;
  int local_14;
  int local_10;
  byte local_9;
  void *local_8;
  
  local_1c = *(undefined4 *)((int)param_1 + 0x20);
  local_8 = param_1;
  switch(local_1c) {
  case 0:
    local_3c = (void *)((int)param_1 + 0x390);
    bVar1 = FUN_00449430(local_3c,0);
    if (bVar1) {
      local_40 = FUN_00437950(DAT_005c0698);
      puVar5 = FUN_00450c70(&local_a4,(uint *)0x0,0x13,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_8 + 0x390) = *puVar5;
    }
    FUN_0052ce50(local_8,1);
    local_44 = (void *)((int)local_8 + 0x24);
    FUN_0049c4e0(local_44,0x29);
    local_48 = (void *)((int)local_8 + 0x24);
    FUN_004bfbe0(local_48,1);
    FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005c6130);
    FUN_0052cf80(local_8,0x2f);
    iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    puVar2 = FUN_0052f540(iVar4);
    if (puVar2 != (undefined *)0x0) {
      *(undefined4 *)((int)local_8 + 0x58fc) = 0;
      uVar3 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      *(undefined4 *)((int)local_8 + 0x5900) = uVar3;
    }
    break;
  case 1:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
    if (DAT_005b889c == (void *)0x0) {
      local_20 = 0;
    }
    else {
      local_4c = DAT_005b889c;
      local_20 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_20 != 0) {
      FUN_004e2020((void *)((int)local_8 + 0x24),10);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_24 = 0;
    }
    else {
      local_50 = DAT_005b889c;
      local_24 = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_24 != 0) {
      FUN_004e2000((void *)((int)local_8 + 0x24),10);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_28 = 0;
    }
    else {
      local_54 = DAT_005b889c;
      local_28 = FUN_0041a280(DAT_005b889c,0x40);
    }
    if (local_28 != 0) {
      iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      if (iVar4 % 10 == 0) {
        FUN_004e2000((void *)((int)local_8 + 0x24),9);
      }
      else {
        local_58 = (void *)((int)local_8 + 0x24);
        FUN_004bf040(local_58);
      }
    }
    if (DAT_005b889c == (void *)0x0) {
      local_2c = 0;
    }
    else {
      local_5c = DAT_005b889c;
      local_2c = FUN_0041a280(DAT_005b889c,0x80);
    }
    if (local_2c != 0) {
      iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      if (iVar4 % 10 == 9) {
        FUN_004e2020((void *)((int)local_8 + 0x24),9);
      }
      else {
        local_60 = (void *)((int)local_8 + 0x24);
        FUN_004bf020(local_60);
      }
    }
    bVar1 = FUN_0045d030((int *)((int)local_8 + 0x24));
    if (CONCAT31(extraout_var,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      puVar2 = FUN_0052f540(iVar4);
      if (puVar2 != (undefined *)0x0) {
        *(undefined4 *)((int)local_8 + 0x58fc) = 0;
        uVar3 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        *(undefined4 *)((int)local_8 + 0x5900) = uVar3;
      }
    }
    if (DAT_005b889c == (void *)0x0) {
      local_30 = 0;
    }
    else {
      local_64 = DAT_005b889c;
      local_30 = FUN_00419c00(DAT_005b889c,0x80001);
    }
    if (local_30 != 0) {
      local_68 = (void *)((int)local_8 + 0x24);
      bVar1 = FUN_004dec50(local_68,0x11);
      if (CONCAT31(extraout_var_00,bVar1) == 0) {
        local_70 = (void *)((int)local_8 + 0x24);
        bVar1 = FUN_004dec50(local_70,0x21);
        if (CONCAT31(extraout_var_01,bVar1) != 0) {
          local_74 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
          iVar4 = FUN_0052ca70(DAT_005c6108,local_74);
          if (iVar4 != 0) {
            FUN_00426d70(&DAT_005ba830,7,0);
            FUN_004d99d0(DAT_0056e0ec);
            FUN_0052ce50(local_8,2);
            break;
          }
        }
      }
      else {
        local_6c = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        iVar4 = FUN_0052ca70(DAT_005c6108,local_6c);
        if (iVar4 != 0) {
          DAT_005afc98 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
          FUN_00426d70(&DAT_005ba830,7,0);
          FUN_004d99d0(DAT_0056e0ec);
          FUN_004a1010();
          return 1;
        }
      }
    }
    if (DAT_005b889c == (void *)0x0) {
      local_34 = 0;
    }
    else {
      local_78 = DAT_005b889c;
      local_34 = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_34 != 0) {
      FUN_0052ce50(local_8,3);
      FUN_00426d70(&DAT_005ba830,9,0);
      FUN_0052cc30(local_8,0x2f);
    }
    iVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    if (iVar4 == 0xd) {
      if (DAT_005b889c == (void *)0x0) {
        local_38 = 0;
      }
      else {
        local_7c = DAT_005b889c;
        local_38 = FUN_00419c00(DAT_005b889c,0x8010f);
      }
      if (local_38 != 0) {
        DAT_005c6850 = 0;
        DAT_005c6854 = 0;
      }
      FUN_00543e20((uint *)&DAT_005c6650,(uint *)&DAT_005c6750,0x100);
      local_18 = FUN_0051f310(&DAT_005c6750);
      if (local_18 == 2) {
        _memset(&DAT_005c6550,0,0x100);
        DAT_005c656e = DAT_005c6791;
        DAT_005c6580 = DAT_005c6792;
        DAT_005c657e = DAT_005c6793;
        DAT_005c6570 = DAT_005c6794;
        DAT_005c6562 = DAT_005c6795;
        DAT_005c6571 = DAT_005c6796;
        DAT_005c6572 = DAT_005c6797;
        DAT_005c6573 = DAT_005c6798;
        DAT_005c6567 = DAT_005c6799;
        DAT_005c6574 = DAT_005c679a;
        DAT_005c6575 = DAT_005c679b;
        DAT_005c6576 = DAT_005c679c;
        DAT_005c6582 = DAT_005c679d;
        DAT_005c6581 = DAT_005c679e;
        DAT_005c6568 = DAT_005c679f;
        DAT_005c6569 = DAT_005c67a0;
        DAT_005c6560 = DAT_005c67a1;
        DAT_005c6563 = DAT_005c67a2;
        DAT_005c656f = DAT_005c67a3;
        DAT_005c6564 = DAT_005c67a4;
        DAT_005c6566 = DAT_005c67a5;
        DAT_005c657f = DAT_005c67a6;
        DAT_005c6561 = DAT_005c67a7;
        DAT_005c657d = DAT_005c67a8;
        DAT_005c6565 = DAT_005c67a9;
        DAT_005c657c = DAT_005c67aa;
        FUN_00543e20((uint *)&DAT_005c6750,(uint *)&DAT_005c6550,0x100);
        local_18 = 1;
      }
      if (local_18 == 1) {
        for (local_10 = 0; local_10 < 0x100; local_10 = local_10 + 1) {
          (&DAT_005c6550)[local_10] =
               ((&DAT_005c6750)[local_10] ^ (&DAT_005c6650)[local_10]) & (&DAT_005c6750)[local_10];
        }
        if (DAT_005c6850 < 8) {
          if (((&DAT_005c6550)[(&DAT_005755cc)[DAT_005c6850]] & 0x80) == 0) {
            local_9 = 0;
            for (local_14 = 0; local_14 < 0x39; local_14 = local_14 + 1) {
              local_9 = local_9 | (&DAT_005c6550)[local_14];
            }
            if ((local_9 & 0x80) != 0) {
              DAT_005c6850 = 0;
            }
          }
          else {
            DAT_005c6850 = DAT_005c6850 + 1;
            DAT_005c6854 = 0;
          }
        }
        else {
          FUN_0051e1c0(DAT_005c6108);
          FUN_00426d70(&DAT_005ba830,0x11,0);
          DAT_005c6850 = 0;
        }
      }
      DAT_005c6854 = DAT_005c6854 + 1;
      if (300 < DAT_005c6854) {
        DAT_005c6850 = 0;
        DAT_005c6854 = 0;
      }
    }
    break;
  case 2:
    iVar4 = FUN_00437520(0);
    iVar4 = FUN_00438520(iVar4);
    if (iVar4 == 0) {
      local_80 = (void *)((int)local_8 + 0x154);
      FUN_00429420(local_80);
      return 1;
    }
    local_84 = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_004785e0(local_84,10);
    if (bVar1) {
      FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
      local_88 = FUN_00437520(0);
      puVar5 = FUN_0049dcf0(&local_a8,0,0,0);
      _DAT_005c4f04 = *puVar5;
      FUN_0051c6b0(&DAT_005c4f04,0);
    }
    local_8c = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_004235c0(local_8c,0x28);
    if (bVar1) {
      DAT_005c6128 = 6;
      DAT_005c6134 = FUN_004b7f70(0x5ba568);
      DAT_005c6138 = FUN_004b8340(0x5ba568);
      local_90 = (void *)((int)local_8 + 0x24);
      iVar4 = FUN_004dec30(local_90,0x12);
      FUN_0050a3e0(&DAT_005ba568,(int)(iVar4 + (iVar4 >> 0x1f & 7U)) >> 3);
      local_94 = (void *)((int)local_8 + 0x24);
      uVar6 = FUN_004dec30(local_94,0x12);
      uVar6 = uVar6 & 0x80000007;
      if ((int)uVar6 < 0) {
        uVar6 = (uVar6 - 1 | 0xfffffff8) + 1;
      }
      FUN_0052cf40(&DAT_005ba568,uVar6);
      local_98 = FUN_004b8340(0x5ba568);
      iVar4 = FUN_00464080(0);
      local_9c = FUN_00412730(iVar4);
      FUN_0051c920(DAT_005c6108,0,local_9c,local_98);
      FUN_0050a7f0(&DAT_005ba568,0xffffffff);
      DAT_005afcfc = 1;
      FUN_004be360(&DAT_005ba568,7);
      FUN_0052ced0(local_8,2);
      DAT_005c6130 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      FUN_004be250(&DAT_005c4d40,7);
    }
    break;
  case 3:
    local_a0 = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_a0,0x14);
    if (bVar1) {
      DAT_005c6130 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      FUN_0052ced0(local_8,10);
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
    }
  }
  *(int *)((int)local_8 + 0x58fc) = *(int *)((int)local_8 + 0x58fc) + 1;
  return 1;
}

