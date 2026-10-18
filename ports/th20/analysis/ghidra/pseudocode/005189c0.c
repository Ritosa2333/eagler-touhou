/* Entry: 0x005189c0; symbol: FUN_005189c0; body bytes: 3704 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_005189c0(int param_1)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  undefined3 extraout_var;
  int iVar5;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  float10 fVar6;
  int local_90;
  int local_8c;
  int local_84;
  int local_80;
  int local_7c;
  int local_74;
  int local_70;
  int local_6c;
  int local_68;
  int local_64;
  int local_60;
  uint local_5c;
  int local_58;
  float local_50;
  float local_4c;
  float local_44;
  float local_40;
  undefined4 local_3c;
  float local_38 [3];
  undefined1 local_2c [12];
  undefined1 local_20 [12];
  undefined1 local_14 [12];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (*(int *)(param_1 + 0x210f4) == 3) {
    iVar5 = FUN_0040c300((undefined4 *)(param_1 + 0x28));
    if (iVar5 == 0) {
      FUN_004b8620(DAT_005c0698,2,1);
      FUN_004c6180(DAT_005c0698,0xc);
      FUN_00515b50(local_38,DAT_00574fa0,DAT_0056e04c);
      FUN_00515b50(local_2c,DAT_0056f10c,DAT_00573fe4);
      FUN_00515b50(local_20,DAT_00570af4,DAT_00573fe4);
      FUN_00515b50(local_14,DAT_0057061c,DAT_00572660);
      for (local_60 = 0; local_60 < 4; local_60 = local_60 + 1) {
        iVar5 = FUN_00464080(0);
        FUN_00412730(iVar5);
        iVar5 = FUN_0051ba40((int)DAT_005c6108);
        FUN_004c6160(DAT_005c0698,*(undefined4 *)(&DAT_00574eec + iVar5 * 4));
        FUN_00470ab0(DAT_005c0698,0xff000000);
        FUN_00470ad0(DAT_005c0698,0xffffffff);
        iVar5 = FUN_00464080(0);
        FUN_00412730(iVar5);
        iVar5 = FUN_0051ba40((int)DAT_005c6108);
        iVar3 = FUN_00464080(0);
        iVar3 = FUN_00412730(iVar3);
        if (local_60 == 3) {
          FUN_004b8620(DAT_005c0698,1,1);
        }
        FUN_0040ddd0(&stack0xfffffee8,
                     (char *)(param_1 + 0xf4 + (local_60 + iVar3 * 0x24 + iVar5 * 4) * 0x100));
        FUN_0046c210(DAT_005c0698,local_38 + local_60 * 3);
      }
      FUN_0051a090((int)DAT_005c0698);
    }
    else {
      FUN_004b8620(DAT_005c0698,0,1);
      iVar5 = FUN_00464080(0);
      FUN_00412730(iVar5);
      FUN_0040c300((undefined4 *)(param_1 + 0x28));
      iVar5 = FUN_0051ba40((int)DAT_005c6108);
      iVar3 = FUN_0040c300((undefined4 *)(param_1 + 0x28));
      iVar4 = FUN_00464080(0);
      iVar4 = FUN_00412730(iVar4);
      local_58 = iVar3 + iVar4 * 0x24 + -1 + iVar5 * 4;
      iVar5 = FUN_004640a0(0x5ba568);
      if ((iVar5 == 4) && (iVar5 = FUN_0040c300((undefined4 *)(param_1 + 0x28)), iVar5 == 1)) {
        iVar5 = FUN_00464080(0);
        FUN_00412730(iVar5);
        FUN_0040c300((undefined4 *)(param_1 + 0x28));
        iVar5 = FUN_0051ba40((int)DAT_005c6108);
        iVar3 = FUN_00464080(0);
        iVar3 = FUN_00412730(iVar3);
        local_58 = iVar5 + 0x48 + iVar3 * 8;
      }
      FUN_004c6180(DAT_005c0698,0x10);
      FUN_00470ad0(DAT_005c0698,0xffffffff);
      FUN_004c6160(DAT_005c0698,0xff000000);
      iVar5 = FUN_00464080(0);
      FUN_00412730(iVar5);
      FUN_0040c300((undefined4 *)(param_1 + 0x28));
      iVar5 = FUN_0051ba40((int)DAT_005c6108);
      FUN_00470ab0(DAT_005c0698,*(undefined4 *)(&DAT_00574eec + iVar5 * 4));
      FUN_00422e10(&local_50);
      FUN_0044cc70((void *)(param_1 + 0xd0),&local_50);
      fVar6 = FUN_00438290(0x5b6758);
      FUN_004c0240(&local_50,(float)fVar6 * DAT_0056e0ec);
      FUN_004645e0((void *)(param_1 + 0xd4),&local_50);
      FUN_004c0240(&local_50,DAT_0056c8d0);
      local_70 = 0;
      bVar1 = FUN_0046b1b0((void *)(param_1 + 0x28),1);
      if (CONCAT31(extraout_var,bVar1) != 0) {
        local_80 = 0;
        for (local_74 = 0; local_74 < 5; local_74 = local_74 + 1) {
          if (*(char *)(param_1 + 0x58f4 + local_58 * 0x500 + local_74 * 0x100) != '\0') {
            local_80 = local_80 + 1;
          }
        }
        local_4c = (float)((5 - local_80) * 0x14) + local_4c;
      }
      FUN_0040ddd0(&stack0xfffffee8,(char *)(param_1 + 0xf4 + local_58 * 0x100));
      FUN_0046c210(DAT_005c0698,&local_50);
      FUN_004c6180(DAT_005c0698,3);
      FUN_004c6160(DAT_005c0698,0xffffffff);
      FUN_00470ab0(DAT_005c0698,0xff000000);
      local_4c = local_4c + DAT_00570560;
      iVar5 = FUN_00464080(0);
      FUN_00412730(iVar5);
      FUN_0040c300((undefined4 *)(param_1 + 0x28));
      iVar5 = FUN_00464100((int)DAT_005c6108);
      if (iVar5 == 8) {
        iVar5 = FUN_00464080(0);
        FUN_00412730(iVar5);
        iVar5 = FUN_0051ba40((int)DAT_005c6108);
        iVar3 = FUN_0040c300((undefined4 *)(param_1 + 0x28));
        iVar4 = FUN_00464080(0);
        iVar4 = FUN_00412730(iVar4);
        iVar5 = iVar3 + iVar4 * 0x24 + -1 + iVar5 * 4;
        if (*(char *)(param_1 + 0x58f4 + iVar5 * 0x500) != '\0') {
          FUN_0040ddd0(&stack0xfffffee8,(char *)(param_1 + 0x58f4 + iVar5 * 0x500));
          FUN_0046c210(DAT_005c0698,&local_50);
        }
        local_4c = local_4c + _DAT_0056ec8c;
        if (*(char *)(param_1 + iVar5 * 0x500 + 0x5af4) != '\0') {
          FUN_0040ddd0(&stack0xfffffee8,(char *)(param_1 + iVar5 * 0x500 + 0x5af4));
          FUN_0046c210(DAT_005c0698,&local_50);
        }
        local_4c = local_4c + DAT_00570560;
        FUN_004c6160(DAT_005c0698,0xffc0c0c0);
        iVar5 = FUN_0040c300((undefined4 *)(param_1 + 0x28));
        if (iVar5 == 4) {
          for (local_6c = 0; local_6c < 2; local_6c = local_6c + 1) {
            bVar1 = FUN_00461070((void *)(param_1 + 0x18),local_6c);
            if (bVar1) {
              if (*(char *)(param_1 + 0x58f4 + local_58 * 0x500 + local_6c * 0x100) != '\0') {
                FUN_0040ddd0(&stack0xfffffee8,
                             (char *)(param_1 + 0x58f4 + local_58 * 0x500 + local_6c * 0x100));
                FUN_0046c210(DAT_005c0698,&local_50);
              }
              local_4c = local_4c + _DAT_0056ec8c;
            }
          }
          local_70 = 5;
        }
        else {
          for (local_68 = 0; local_68 < 1; local_68 = local_68 + 1) {
            bVar1 = FUN_00461070((void *)(param_1 + 0x18),local_68);
            if (bVar1) {
              if (*(char *)(param_1 + 0x58f4 + local_58 * 0x500 + local_68 * 0x100) != '\0') {
                FUN_0040ddd0(&stack0xfffffee8,
                             (char *)(param_1 + 0x58f4 + local_58 * 0x500 + local_68 * 0x100));
                FUN_0046c210(DAT_005c0698,&local_50);
              }
              local_4c = local_4c + _DAT_0056ec8c;
            }
          }
          local_70 = 4;
        }
      }
      else {
        for (local_64 = 0; local_64 < 5; local_64 = local_64 + 1) {
          bVar1 = FUN_00461070((void *)(param_1 + 0x18),local_64);
          if (bVar1) {
            if (*(char *)(param_1 + 0x58f4 + local_58 * 0x500 + local_64 * 0x100) != '\0') {
              FUN_0040ddd0(&stack0xfffffee8,
                           (char *)(param_1 + 0x58f4 + local_58 * 0x500 + local_64 * 0x100));
              FUN_0046c210(DAT_005c0698,&local_50);
              local_70 = local_70 + 1;
            }
            local_4c = local_4c + _DAT_0056ec8c;
          }
        }
      }
      FUN_004c6180(DAT_005c0698,2);
      FUN_004b8620(DAT_005c0698,1,1);
      FUN_00470ad0(DAT_005c0698,0xffffffff);
      FUN_00470ab0(DAT_005c0698,0xff000000);
      FUN_004506b0((void *)(param_1 + 0xd4),DAT_0056e050 / DAT_0056c8d0,
                   ((float)local_70 * DAT_00570560 + DAT_0056fa30) / DAT_0056c8d0);
    }
  }
  else if (((*(int *)(param_1 + 0x210f4) == 4) &&
           (bVar1 = FUN_00461070((void *)(param_1 + 0x18),0x14), bVar1)) &&
          (iVar5 = FUN_0040c300((undefined4 *)(param_1 + 0x28)), iVar5 != 0)) {
    local_44 = *(float *)(param_1 + 0xe8);
    local_40 = *(float *)(param_1 + 0xec);
    local_3c = *(undefined4 *)(param_1 + 0xf0);
    bVar1 = FUN_005163c0((void *)(param_1 + 0x28),1);
    if (CONCAT31(extraout_var_00,bVar1) != 0) {
      FUN_004b8620(DAT_005c0698,2,1);
      FUN_004c6180(DAT_005c0698,0);
      FUN_00470ad0(DAT_005c0698,0xffa0a0a0);
      FUN_004c6160(DAT_005c0698,0xffffffff);
      local_44 = local_44 + DAT_0056f108;
      local_40 = local_40 - _DAT_00574f98;
      FUN_004c0240(&local_44,DAT_0056c8d0);
      FUN_00470ab0(DAT_005c0698,0xc0000000);
      FUN_0040ddd0(&stack0xfffffee8,&DAT_00574f88);
      FUN_0046c210(DAT_005c0698,&local_44);
      local_44 = *(float *)(param_1 + 0xe8);
      local_40 = *(float *)(param_1 + 0xec);
      local_3c = *(undefined4 *)(param_1 + 0xf0);
    }
    local_44 = local_44 - DAT_0056f108;
    bVar1 = FUN_0046b1b0((void *)(param_1 + 0x28),1);
    if (CONCAT31(extraout_var_01,bVar1) == 0) {
      local_8c = 0;
    }
    else {
      local_8c = 0x14;
    }
    local_40 = local_40 - (float)(0xd2 - local_8c);
    FUN_004c0240(&local_44,DAT_0056c8d0);
    iVar5 = FUN_00464080(0);
    local_84 = FUN_00412730(iVar5);
    local_84 = local_84 * 0x24;
    local_5c = 0;
    while( true ) {
      bVar1 = FUN_0046b1b0((void *)(param_1 + 0x28),1);
      if (CONCAT31(extraout_var_02,bVar1) == 0) {
        local_90 = 9;
      }
      else {
        local_90 = 8;
      }
      if (local_90 <= (int)local_5c) break;
      local_7c = FUN_004bd610(DAT_005c6108,local_5c);
      iVar5 = FUN_0051b970(DAT_005c6108,local_5c);
      local_7c = local_7c - iVar5;
      FUN_004b8620(DAT_005c0698,1,1);
      FUN_004c6180(DAT_005c0698,5);
      if (9 < local_7c) {
        local_7c = 9;
      }
      if (*(uint *)(param_1 + 0xc0) == local_5c) {
        FUN_00470ad0(DAT_005c0698,0xffffffa0);
      }
      else if (((local_7c == 0) &&
               (bVar1 = FUN_0046b1b0((void *)(param_1 + 0x28),1),
               CONCAT31(extraout_var_03,bVar1) == 0)) && (local_5c != 8)) {
        FUN_00470ad0(DAT_005c0698,0xff606060);
      }
      else {
        FUN_00470ad0(DAT_005c0698,0xffffffff);
      }
      iVar5 = FUN_004640a0(0x5ba568);
      if ((iVar5 == 4) &&
         (bVar1 = FUN_0046b1b0((void *)(param_1 + 0x28),1), CONCAT31(extraout_var_04,bVar1) != 0)) {
        iVar5 = FUN_00464080(0);
        iVar5 = FUN_00412730(iVar5);
        cVar2 = FUN_0051bc10(DAT_005c6108,iVar5,local_5c);
        if (cVar2 == '\0') {
          FUN_00470ad0(DAT_005c0698,0xff606060);
        }
      }
      FUN_004c6160(DAT_005c0698,0xffffffff);
      FUN_00470ab0(DAT_005c0698,0xc0000000);
      FUN_0040ddd0(&stack0xfffffee8,(char *)(param_1 + 0xf4 + local_84 * 0x100));
      FUN_0046c210(DAT_005c0698,&local_44);
      local_44 = local_44 + DAT_0056ed14;
      bVar1 = FUN_0046b1b0((void *)(param_1 + 0x28),1);
      if (CONCAT31(extraout_var_05,bVar1) == 0) {
        if (local_5c == 8) {
          FUN_0040ddd0(&stack0xfffffee8,&DAT_00574f94);
          FUN_0046c210(DAT_005c0698,&local_44);
        }
        else {
          FUN_0046c210(DAT_005c0698,&local_44);
        }
      }
      else {
        FUN_0040ddd0(&stack0xfffffee8,&DAT_00574f90);
        FUN_0046c210(DAT_005c0698,&local_44);
      }
      local_44 = local_44 - DAT_0056ed14;
      local_40 = local_40 + DAT_0056f7b4;
      local_84 = local_84 + 4;
      local_5c = local_5c + 1;
    }
    FUN_004c6180(DAT_005c0698,2);
    FUN_004b8620(DAT_005c0698,1,1);
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_00470ab0(DAT_005c0698,0xff000000);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

