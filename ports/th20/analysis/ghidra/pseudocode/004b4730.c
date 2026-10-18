/* Entry: 0x004b4730; symbol: FUN_004b4730; body bytes: 4179 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004b4730(int param_1)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  float *pfVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  undefined3 extraout_var;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined1 local_114 [12];
  undefined1 local_108 [12];
  undefined1 local_fc [12];
  undefined1 local_f0 [12];
  int local_e4;
  int local_e0;
  undefined8 local_dc;
  void *local_d4;
  void *local_d0;
  int local_cc;
  uint local_c8;
  undefined4 local_c4;
  undefined4 local_c0;
  void *local_bc;
  float local_b8;
  float local_b4;
  void *local_b0;
  void *local_ac;
  void *local_a8;
  uint local_a4;
  uint local_a0;
  void *local_9c;
  void *local_98;
  uint local_94;
  uint local_90;
  uint local_8c;
  void *local_88;
  void *local_84;
  uint local_80;
  void *local_7c;
  void *local_78;
  uint local_74;
  uint local_70;
  void *local_6c;
  void *local_68;
  int local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  float local_54;
  int local_50;
  int local_4c;
  undefined1 local_46;
  undefined1 local_45;
  undefined1 local_44;
  undefined1 local_43;
  undefined1 local_42;
  undefined1 local_41;
  undefined1 local_40;
  undefined1 local_3f;
  undefined1 local_3e;
  undefined1 local_3d;
  undefined1 local_3c;
  undefined1 local_3b;
  undefined1 local_3a;
  undefined1 local_39;
  int local_38;
  int local_34;
  int local_30;
  float local_2c;
  float local_28;
  float local_20;
  float local_1c;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_38 = param_1;
  if (*(int *)(param_1 + 0x138) != 0) {
    FUN_00422dd0(&local_20,DAT_0056ed14,DAT_00570ad4,0);
    local_50 = FUN_0044ced0((undefined4 *)(local_38 + 0x114));
    iVar2 = FUN_00478ea0(0);
    if (iVar2 == 0) {
      *(undefined4 *)(local_38 + 0x138) = 0;
      FUN_0044fcd0((undefined4 *)(local_38 + 0x114));
    }
    else if (local_50 == 0) {
      *(undefined4 *)(local_38 + 0x138) = 0;
    }
    else {
      iVar2 = FUN_0045d0e0(local_50);
      local_3f = *(undefined1 *)(iVar2 + 3);
      FUN_00488940(DAT_005c0698,local_3f);
      FUN_00488a00(DAT_005c0698,2);
      FUN_00488b00(DAT_005c0698,4);
      iVar2 = FUN_00478ea0(0);
      local_4c = FUN_00426370(iVar2);
      local_4c = local_4c / 0x3c;
      if (999 < local_4c) {
        local_4c = 999;
      }
      FUN_0046c990(DAT_005c0698,&local_20,L"㌥⹤");
      FUN_004b8d90(DAT_005c0698,DAT_00570ab0,DAT_00570ab0);
      local_20 = local_20 + _DAT_00570acc;
      local_1c = local_1c + DAT_0056f00c;
      iVar2 = FUN_00478ea0(0);
      iVar2 = FUN_00426370(iVar2);
      local_cc = ((iVar2 % 0x3c) * 100) / 0x3c;
      FUN_0046c990(DAT_005c0698,&local_20,(wchar_t *)"%.2ds");
      local_20 = DAT_0056ed14;
      local_1c = DAT_0056fb80;
      FUN_00470ad0(DAT_005c0698,0xff808080);
      iVar2 = FUN_0045d0e0(local_50);
      local_39 = *(undefined1 *)(iVar2 + 3);
      FUN_00488940(DAT_005c0698,local_39);
      FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
      local_d0 = (void *)FUN_00478ea0(0);
      FUN_004b7ec0(local_d0,&local_30,&local_34);
      FUN_0046c990(DAT_005c0698,&local_20,L"㌥⹤");
      FUN_004b8d90(DAT_005c0698,DAT_00570ab0,DAT_00570ab0);
      local_20 = local_20 + _DAT_00570acc;
      local_1c = local_1c + DAT_0056f00c;
      FUN_0046c990(DAT_005c0698,&local_20,(wchar_t *)"%.2ds");
      FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
      FUN_00488940(DAT_005c0698,0xff);
      FUN_00488b00(DAT_005c0698,0);
      FUN_00488a00(DAT_005c0698,0);
      FUN_00470ad0(DAT_005c0698,0xffffffff);
    }
  }
  FUN_00488b00(DAT_005c0698,10);
  FUN_00470ad0(DAT_005c0698,0xff000000);
  FUN_00470ab0(DAT_005c0698,0xffffffff);
  FUN_00488a00(DAT_005c0698,3);
  FUN_00422dd0(&local_14,DAT_00570af4,DAT_00570ac8,0);
  FUN_00470ad0(DAT_005c0698,0xff707070);
  FUN_00470ab0(DAT_005c0698,0x80ffffff);
  local_d4 = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_d4,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_3a = *(undefined1 *)(iVar2 + 3);
  FUN_00488940(DAT_005c0698,local_3a);
  local_58 = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_58,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_3b = *(undefined1 *)(iVar2 + 3);
  FUN_004b8600(DAT_005c0698,local_3b);
  local_dc = FUN_004b8030(0x5ba568);
  local_e4 = FUN_004b8010(0x5ba568);
  local_e0 = local_e4 >> 0x1f;
  FUN_004b8620(DAT_005c0698,2,1);
  FUN_0046d020(DAT_005c0698,&local_14,(uint)local_dc,(int)(uint)local_dc >> 0x1f,local_e4);
  local_10 = (float)DAT_0056fa28;
  FUN_00470ad0(DAT_005c0698,0xff001080);
  FUN_00470ab0(DAT_005c0698,0xd0ffffff);
  local_5c = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_5c,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_3c = *(undefined1 *)(iVar2 + 3);
  FUN_00488940(DAT_005c0698,local_3c);
  local_60 = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_60,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_3d = *(undefined1 *)(iVar2 + 3);
  FUN_004b8600(DAT_005c0698,local_3d);
  local_64 = FUN_004b7fa0(0x5ba568);
  local_c8 = *(uint *)(local_38 + 0x160);
  local_c4 = 0;
  FUN_0046d020(DAT_005c0698,&local_14,local_c8,0,local_64);
  FUN_004b8620(DAT_005c0698,1,1);
  uVar4 = FUN_004e15c0();
  if (uVar4 < 100) {
    local_54 = 0.0;
  }
  else {
    local_54 = DAT_0056f7a0;
  }
  pfVar5 = (float *)FUN_00422dd0(local_f0,DAT_00570aec - local_54,DAT_00570ad0,0);
  local_14 = *pfVar5;
  local_10 = pfVar5[1];
  local_c = pfVar5[2];
  FUN_004b8d90(DAT_005c0698,DAT_00570ab0,DAT_00570ab0);
  FUN_00470ad0(DAT_005c0698,0xff000000);
  FUN_00470ab0(DAT_005c0698,0xffffffff);
  local_68 = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_68,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_3e = *(undefined1 *)(iVar2 + 3);
  FUN_00488940(DAT_005c0698,local_3e);
  local_6c = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_6c,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_46 = *(undefined1 *)(iVar2 + 3);
  FUN_004b8600(DAT_005c0698,local_46);
  uVar4 = FUN_004e15c0();
  if (uVar4 < 999999) {
    iVar2 = FUN_00464080(0);
    uVar4 = FUN_004b8150(iVar2);
    uVar10 = 3;
    local_70 = uVar4;
    FUN_0046cd40(DAT_005c0698,&local_14,uVar4,3,' ');
    local_14 = local_14 + _DAT_00570ac0;
    uVar6 = FUN_004e15c0();
    local_74 = uVar6;
    FUN_0040ddd0(&stack0xfffffedc,"/");
    FUN_0046cfa0(DAT_005c0698,&local_14,uVar4,uVar10,uVar6);
  }
  else {
    FUN_0046c990(DAT_005c0698,&local_14,L"⼭-〰");
  }
  pfVar5 = (float *)FUN_00422dd0(local_fc,DAT_00570aec,DAT_00570ad8,0);
  local_14 = *pfVar5;
  local_10 = pfVar5[1];
  local_c = pfVar5[2];
  local_78 = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_78,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_40 = *(undefined1 *)(iVar2 + 3);
  FUN_00488940(DAT_005c0698,local_40);
  local_7c = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_7c,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_41 = *(undefined1 *)(iVar2 + 3);
  FUN_004b8600(DAT_005c0698,local_41);
  FUN_004b8d90(DAT_005c0698,DAT_00570ab0,DAT_00570ab0);
  iVar2 = FUN_00464080(0);
  uVar4 = FUN_004b7e70(iVar2);
  uVar10 = 3;
  local_80 = uVar4;
  FUN_0046cd40(DAT_005c0698,&local_14,uVar4,3,' ');
  local_14 = local_14 + _DAT_00570ac0;
  uVar6 = 3;
  FUN_0040ddd0(&stack0xfffffedc,"/");
  FUN_0046cfa0(DAT_005c0698,&local_14,uVar4,uVar10,uVar6);
  FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
  pfVar5 = (float *)FUN_00422dd0(local_108,DAT_00570ae8,DAT_00570adc,0);
  local_14 = *pfVar5;
  local_10 = pfVar5[1];
  local_c = pfVar5[2];
  FUN_00470ad0(DAT_005c0698,0xff800000);
  FUN_00470ab0(DAT_005c0698,0x80ffd0d0);
  local_84 = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_84,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_42 = *(undefined1 *)(iVar2 + 3);
  FUN_00488940(DAT_005c0698,local_42);
  local_88 = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_88,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_43 = *(undefined1 *)(iVar2 + 3);
  FUN_004b8600(DAT_005c0698,local_43);
  iVar2 = FUN_00464080(0);
  local_8c = FUN_004b81a0(iVar2);
  FUN_0040ddd0(&stack0xfffffee0,".");
  FUN_0046cd80(DAT_005c0698,&local_14,local_8c);
  FUN_004b8d90(DAT_005c0698,DAT_00570ab0,DAT_00570ab0);
  local_14 = local_14 + _DAT_0056ec8c;
  local_10 = local_10 + DAT_0056f7a0;
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_004993b0(iVar2);
  iVar7 = FUN_00464080(0);
  iVar7 = FUN_004b81d0(iVar7);
  iVar8 = FUN_00464080(0);
  iVar8 = FUN_004b81d0(iVar8);
  local_90 = ((iVar2 % iVar7) * 100) / iVar8;
  uVar10 = 0x4b5125;
  FUN_0046cd40(DAT_005c0698,&local_14,local_90,2,'0');
  FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
  local_10 = local_10 - DAT_0056f7a0;
  local_14 = local_14 + DAT_0056f7ac;
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_004b8210(iVar2);
  iVar7 = FUN_00464080(0);
  iVar7 = FUN_004b81d0(iVar7);
  local_94 = iVar2 / iVar7;
  uVar9 = 0x4b51b0;
  FUN_0040ddd0(&stack0xfffffee0,".");
  uVar4 = local_94;
  FUN_0040ddd0(&stack0xfffffed4,"/");
  FUN_0046cf00(DAT_005c0698,&local_14,uVar10,uVar9,uVar4);
  local_14 = local_14 + DAT_0056cd94;
  local_10 = local_10 + DAT_0056f7a0;
  FUN_004b8d90(DAT_005c0698,DAT_00570ab0,DAT_00570ab0);
  FUN_0040ddd0(&stack0xfffffee0,"00");
  FUN_0046ce90(DAT_005c0698,&local_14);
  FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
  FUN_004b8620(DAT_005c0698,2,1);
  FUN_00470ad0(DAT_005c0698,0xff701070);
  FUN_00470ab0(DAT_005c0698,0x80f0c0f0);
  local_98 = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_98,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_44 = *(undefined1 *)(iVar2 + 3);
  FUN_00488940(DAT_005c0698,local_44);
  local_9c = (void *)(local_38 + 0x50);
  piVar3 = (int *)FUN_00414580(local_9c,0);
  iVar2 = FUN_0045d0e0(*piVar3);
  local_45 = *(undefined1 *)(iVar2 + 3);
  FUN_004b8600(DAT_005c0698,local_45);
  pfVar5 = (float *)FUN_00422dd0(local_114,DAT_00570af0,DAT_00570ae4,0);
  local_14 = *pfVar5;
  local_10 = pfVar5[1];
  local_c = pfVar5[2];
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_004b82c0(iVar2);
  iVar7 = FUN_00464080(0);
  iVar7 = FUN_004b8300(iVar7);
  local_a0 = iVar2 / iVar7;
  FUN_0040ddd0(&stack0xfffffee0,".");
  FUN_0046cd80(DAT_005c0698,&local_14,local_a0);
  FUN_004b8d90(DAT_005c0698,DAT_00570ab0,DAT_00570ab0);
  local_14 = local_14 + DAT_0056f7a8;
  local_10 = local_10 + DAT_0056f7a0;
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_004b82c0(iVar2);
  iVar7 = FUN_00464080(0);
  iVar7 = FUN_004b8300(iVar7);
  iVar8 = FUN_00464080(0);
  iVar8 = FUN_004b8300(iVar8);
  local_a4 = ((iVar2 % iVar7) * 100) / iVar8;
  FUN_0046cd40(DAT_005c0698,&local_14,local_a4,2,'0');
  FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
  local_10 = local_10 - DAT_0056f7a0;
  local_14 = local_14 + DAT_0056f7ac;
  FUN_004b8620(DAT_005c0698,1,1);
  FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  FUN_00470ab0(DAT_005c0698,0xff000000);
  FUN_00488940(DAT_005c0698,0xff);
  FUN_00488b00(DAT_005c0698,0);
  FUN_00488a00(DAT_005c0698,0);
  iVar2 = FUN_00478060(0);
  if ((iVar2 != 0) && (-1 < *(int *)(local_38 + 0x1c4))) {
    local_a8 = (void *)FUN_00478060(0);
    iVar2 = FUN_00485660(local_a8,0);
    if (iVar2 != 0) {
      iVar2 = FUN_00478060(0);
      uVar4 = FUN_004b83f0(iVar2);
      if (uVar4 == 0) {
        bVar1 = FUN_00478160(local_38);
        if (CONCAT31(extraout_var,bVar1) == 0) {
          iVar2 = FUN_004b8430(DAT_005c60bc);
          if (iVar2 == 0) {
            uVar4 = FUN_004b8450(DAT_005ba828);
            if (uVar4 == 0) {
              local_ac = (void *)(local_38 + 0x88);
              piVar3 = (int *)FUN_00414580(local_ac,0);
              iVar2 = FUN_0044cb60(*piVar3);
              local_b4 = *(float *)(iVar2 + 4) - DAT_0056f7a0;
              local_b0 = (void *)(local_38 + 0x88);
              piVar3 = (int *)FUN_00414580(local_b0,0);
              pfVar5 = (float *)FUN_0044cb60(*piVar3);
              local_b8 = *pfVar5 + DAT_0056cd90;
              FUN_00422dd0(&local_2c,local_b8,local_b4,0);
              local_bc = (void *)(local_38 + 0x88);
              piVar3 = (int *)FUN_00414580(local_bc,0);
              local_c0 = FUN_004456e0(*piVar3);
              FUN_00470ad0(DAT_005c0698,local_c0);
              FUN_00488b00(DAT_005c0698,4);
              FUN_00488a00(DAT_005c0698,2);
              FUN_0046c990(DAT_005c0698,&local_2c,L".");
              local_2c = local_2c + DAT_0056d7bc;
              local_28 = local_28 + DAT_0056f00c;
              FUN_004b8d90(DAT_005c0698,DAT_00570ab0,DAT_00570ab0);
              FUN_0046c990(DAT_005c0698,&local_2c,L"⸥搲");
              FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
              FUN_00470ad0(DAT_005c0698,0xffffffff);
              FUN_00488a00(DAT_005c0698,0);
              FUN_00488b00(DAT_005c0698,0);
            }
          }
        }
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

