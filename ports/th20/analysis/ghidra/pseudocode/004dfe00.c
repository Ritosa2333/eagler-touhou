/* Entry: 0x004dfe00; symbol: FUN_004dfe00; body bytes: 2279 */

undefined4 __fastcall FUN_004dfe00(undefined4 *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 uVar2;
  int iVar3;
  uint local_48;
  uint local_44;
  uint local_40;
  int local_30;
  int local_24;
  int local_20;
  int local_1c;
  
  if (DAT_005c4d28 != 0) {
    return 1;
  }
  switch(param_1[0x28]) {
  case 0:
    FUN_0049c4e0(param_1 + 5,6);
    FUN_004bed50(param_1 + 5,0);
    FUN_004e0f80(param_1,1);
  case 1:
    bVar1 = FUN_00461070(param_1 + 0x18,6);
    if (bVar1) {
      FUN_004e0f80(param_1,2);
    }
    break;
  case 2:
    FUN_004bfc00(param_1 + 5);
    if (DAT_005b88a4 == (void *)0x0) {
      local_1c = 0;
    }
    else {
      local_1c = FUN_0041a280(DAT_005b88a4,0x10);
    }
    if (local_1c != 0) {
      FUN_004bf040(param_1 + 5);
    }
    if (DAT_005b88a4 == (void *)0x0) {
      local_20 = 0;
    }
    else {
      local_20 = FUN_0041a280(DAT_005b88a4,0x20);
    }
    if (local_20 != 0) {
      FUN_004bf020(param_1 + 5);
    }
    bVar1 = FUN_0045d030(param_1 + 5);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      FUN_00423520(param_1 + 0x1f,8);
    }
    bVar1 = FUN_0046b1b0(param_1 + 5,2);
    if ((CONCAT31(extraout_var_00,bVar1) != 0) &&
       (iVar3 = FUN_00478e00(param_1 + 0x18,0x3c), iVar3 != 0)) {
      FUN_00426d70(&DAT_005ba830,2,0);
    }
    if (DAT_005b88a4 == (void *)0x0) {
      local_24 = 0;
    }
    else {
      local_24 = FUN_0041a280(DAT_005b88a4,0x40);
    }
    if (local_24 != 0) {
      iVar3 = FUN_0040c300(param_1 + 5);
      if (iVar3 == 0) {
        __aligned_offset_recalloc((void *)0x9,10,0,0,0);
        param_1[0x28] = 4;
        FUN_00423520(param_1 + 0x18,0);
        uVar2 = FUN_0041cc70(0x5b6758);
        switch(uVar2) {
        case 0:
        case 1:
        case 2:
          FUN_0041e030(&DAT_005b6758,9);
          break;
        case 3:
          FUN_0041e030(&DAT_005b6758,0);
          break;
        case 4:
          FUN_0041e030(&DAT_005b6758,1);
          break;
        case 5:
          FUN_0041e030(&DAT_005b6758,2);
          break;
        case 6:
          FUN_0041e030(&DAT_005b6758,2);
          break;
        case 7:
          FUN_0041e030(&DAT_005b6758,2);
          break;
        case 8:
          switch(DAT_005c4f8a) {
          case 0:
            FUN_0041e030(&DAT_005b6758,3);
            break;
          case 1:
            FUN_0041e030(&DAT_005b6758,4);
            break;
          case 2:
            FUN_0041e030(&DAT_005b6758,5);
            break;
          case 3:
            FUN_0041e030(&DAT_005b6758,6);
            break;
          case 4:
            FUN_0041e030(&DAT_005b6758,7);
            break;
          case 5:
            FUN_0041e030(&DAT_005b6758,5);
          }
          break;
        case 9:
          FUN_0041e030(&DAT_005b6758,8);
        }
      }
      else if (iVar3 == 1) {
        if (DAT_005c4f86 < '\x05') {
          DAT_005c4f86 = '\0';
        }
        else {
          DAT_005c4f86 = DAT_005c4f86 + -5;
        }
        FUN_004e0fc0();
      }
      else if (iVar3 == 2) {
        if (DAT_005c4f87 < '\x05') {
          DAT_005c4f87 = '\0';
        }
        else {
          DAT_005c4f87 = DAT_005c4f87 + -5;
        }
        FUN_004e0fc0();
      }
    }
    if (DAT_005b88a4 == (void *)0x0) {
      local_30 = 0;
    }
    else {
      local_30 = FUN_0041a280(DAT_005b88a4,0x80);
    }
    if (local_30 != 0) {
      iVar3 = FUN_0040c300(param_1 + 5);
      if (iVar3 == 0) {
        __aligned_offset_recalloc((void *)0x9,10,0,0,0);
        param_1[0x28] = 4;
        FUN_00423520(param_1 + 0x18,0);
        uVar2 = FUN_0041cc70(0x5b6758);
        switch(uVar2) {
        case 0:
          FUN_0041e030(&DAT_005b6758,3);
          break;
        case 1:
          FUN_0041e030(&DAT_005b6758,4);
          break;
        case 2:
          switch(DAT_005c4f8a) {
          case 0:
            FUN_0041e030(&DAT_005b6758,3);
            break;
          case 1:
            FUN_0041e030(&DAT_005b6758,4);
            break;
          case 2:
            FUN_0041e030(&DAT_005b6758,5);
            break;
          case 3:
            FUN_0041e030(&DAT_005b6758,6);
            break;
          case 4:
            FUN_0041e030(&DAT_005b6758,7);
            break;
          case 5:
            FUN_0041e030(&DAT_005b6758,5);
          }
          break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
          FUN_0041e030(&DAT_005b6758,8);
          break;
        case 8:
          FUN_0041e030(&DAT_005b6758,9);
          break;
        case 9:
          switch(DAT_005c4f8a) {
          case 0:
            FUN_0041e030(&DAT_005b6758,0);
            break;
          case 1:
            FUN_0041e030(&DAT_005b6758,1);
            break;
          case 2:
          case 3:
          case 4:
          case 5:
            FUN_0041e030(&DAT_005b6758,2);
          }
        }
      }
      else if (iVar3 == 1) {
        DAT_005c4f86 = DAT_005c4f86 + '\x05';
        if ('d' < DAT_005c4f86) {
          DAT_005c4f86 = 'd';
        }
        FUN_004e0fc0();
      }
      else if (iVar3 == 2) {
        DAT_005c4f87 = DAT_005c4f87 + '\x05';
        if ('d' < DAT_005c4f87) {
          DAT_005c4f87 = 'd';
        }
        FUN_004e0fc0();
      }
    }
    if (DAT_005b88a4 == (void *)0x0) {
      local_40 = 0;
    }
    else {
      local_40 = FUN_00419c00(DAT_005b88a4,0x106);
    }
    if (local_40 == 0) {
LAB_004e0443:
      if (param_1[0x27] != 0) {
        if (DAT_005b88a4 == (void *)0x0) {
          local_44 = 0;
        }
        else {
          local_44 = FUN_00419c00(DAT_005b88a4,0x100);
        }
        if (local_44 != 0) goto LAB_004e0524;
      }
      if (DAT_005b88a4 == (void *)0x0) {
        local_48 = 0;
      }
      else {
        local_48 = FUN_00419c00(DAT_005b88a4,0x80001);
      }
      if (local_48 == 0) break;
      iVar3 = FUN_0040c300(param_1 + 5);
      if (iVar3 == 3) {
        FUN_00423520(param_1 + 0x18,0);
        param_1[0x28] = 3;
        FUN_00423520(param_1 + 0x23,0x1e);
        FUN_00426d70(&DAT_005ba830,7,0);
        break;
      }
      if (iVar3 == 4) {
        DAT_005c4f86 = 'd';
        DAT_005c4f87 = 'P';
        FUN_00426d70(&DAT_005ba830,7,0);
        break;
      }
      if (iVar3 != 5) break;
    }
    else {
      iVar3 = FUN_0040c300(param_1 + 5);
      if (iVar3 != 5) {
        FUN_004bed50(param_1 + 5,5);
        goto LAB_004e0443;
      }
    }
LAB_004e0524:
    FUN_00426d70(&DAT_005ba830,9,0);
    FUN_004e0f80(param_1,5);
    break;
  case 3:
    bVar1 = FUN_00461070(param_1 + 0x18,0x1e);
    if ((bVar1) && (iVar3 = FUN_0040c300(param_1 + 5), iVar3 == 3)) {
      FUN_004c7cb0(param_1 + 0x1c);
      param_1[0x28] = 0;
    }
    break;
  case 4:
    bVar1 = FUN_00461070(param_1 + 0x18,3);
    if (bVar1) {
      FUN_0041de00(&DAT_005b6758,1);
      param_1[0x28] = 2;
      FUN_00423520(param_1 + 0x18,0);
    }
    break;
  case 5:
    bVar1 = FUN_004235c0(param_1 + 0x18,10);
    if (bVar1) {
      FUN_0041aa70(&DAT_005c4f08);
      FUN_004217c0(param_1);
      return 1;
    }
  }
  FUN_00423540(param_1 + 0x18);
  bVar1 = FUN_00461070(param_1 + 0x1f,0);
  if (bVar1) {
    FUN_00429420(param_1 + 0x1f);
  }
  bVar1 = FUN_00461070(param_1 + 0x23,0);
  if (bVar1) {
    FUN_00429420(param_1 + 0x23);
  }
  return 1;
}

