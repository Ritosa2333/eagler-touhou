/* Entry: 0x00484a20; symbol: FUN_00484a20; body bytes: 2018 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00484a20(void *this,int param_1)

{
  bool bVar1;
  char cVar2;
  undefined3 extraout_var;
  int iVar3;
  undefined4 *puVar4;
  uint uVar5;
  float10 fVar6;
  undefined1 local_11c [12];
  undefined4 local_110;
  undefined4 local_10c;
  float local_108;
  undefined4 local_104;
  void *local_100;
  void *local_fc;
  void *local_f8;
  void *local_f4;
  float local_f0;
  float local_ec;
  void *local_e8;
  void *local_e4;
  void *local_e0;
  void *local_dc;
  undefined4 *local_d8;
  undefined4 *local_d4;
  void *local_d0;
  uint local_cc;
  void *local_c8;
  int local_c4;
  void *local_c0;
  void *local_bc;
  void *local_b8;
  undefined4 local_b4;
  void *local_b0;
  undefined4 local_ac;
  void *local_a8;
  void *local_a4;
  undefined4 local_a0;
  int local_9c;
  void *local_98;
  void *local_94;
  void *local_90;
  undefined4 *local_8c;
  int local_88;
  void *local_84;
  float local_80;
  undefined4 local_7c;
  void *local_78;
  void *local_74;
  void *local_70;
  int local_6c;
  int local_68;
  void *local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_40;
  undefined4 local_28 [3];
  undefined4 local_1c [3];
  float local_10 [2];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_68 = 0;
  local_88 = 0;
  local_e4 = *(void **)((int)this + 0x1c);
  local_64 = this;
  FUN_004863c0(local_e4,0);
  local_100 = *(void **)((int)local_64 + 0x1c);
  FUN_00470af0(local_100,(undefined4 *)&DAT_0056fc08);
  if (((*(uint *)((int)local_64 + 0x14) >> 1 & 1) == 0) ||
     (*(float *)((int)local_64 + 0x7c) <= _DAT_0056e0e8)) goto LAB_004851ef;
  FUN_0047c5f0((void *)((int)local_64 + 0x4d8),DAT_0056c8cc);
  if ((*(uint *)((int)local_64 + 0x14) >> 6 & 1) == 0) {
    if ((*(uint *)((int)local_64 + 0x14) >> 4 & 1) == 0) {
      local_104 = FUN_0040ff90(*(int *)((int)local_64 + 0x520));
      local_68 = FUN_004f8ce0((float *)((int)local_64 + 100),(float *)((int)local_64 + 0x7c),param_1
                             );
    }
    else {
      local_74 = (void *)FUN_0040ff90(*(int *)((int)local_64 + 0x520));
      local_108 = *(float *)((int)local_64 + 0x7c);
      local_68 = FUN_004f8ff0(local_74,(float *)((int)local_64 + 100),local_108,param_1);
    }
  }
  else if ((*(uint *)((int)local_64 + 0x14) >> 4 & 1) == 0) {
    local_78 = (void *)((int)local_64 + 0x7c);
    FUN_004293c0(local_78,local_10,*(float *)((int)local_64 + 0x48));
    local_7c = FUN_0040ff90(*(int *)((int)local_64 + 0x520));
    local_68 = FUN_004f8ce0((float *)((int)local_64 + 100),local_10,param_1);
  }
  else {
    local_84 = (void *)FUN_0040ff90(*(int *)((int)local_64 + 0x520));
    local_80 = *(float *)((int)local_64 + 0x7c) * *(float *)((int)local_64 + 0x48);
    local_68 = FUN_004f8ff0(local_84,(float *)((int)local_64 + 100),local_80,param_1);
  }
  if ((local_88 != 0) && (local_68 == 0)) {
    local_68 = 2;
  }
  if (local_68 == 0) {
    local_8c = (undefined4 *)((int)local_64 + 0x4c0);
    FUN_00423540(local_8c);
    local_90 = (void *)((int)local_64 + 0x4c0);
    bVar1 = FUN_004235c0(local_90,0x3c);
    if ((bVar1) && (*(int *)((int)local_64 + 0x4d4) == 0)) {
      *(undefined4 *)((int)local_64 + 0x4d4) = 3;
      FUN_00423520((void *)((int)local_64 + 0x4a0),0);
    }
  }
  if ((local_68 == 1) && (*(int *)((int)local_64 + 0x18) == 0)) {
LAB_00484ca1:
    *(undefined4 *)((int)local_64 + 0x50) = 3;
    local_94 = *(void **)((int)local_64 + 0x1c);
    FUN_004388f0(local_94,1);
    bVar1 = FUN_00485a10((undefined4 *)((int)local_64 + 0x5c));
    if (CONCAT31(extraout_var,bVar1) != 0) {
      local_98 = (void *)((int)local_64 + 0x5c);
      FUN_0044ef90(local_98,1);
    }
    if (-1 < *(int *)((int)local_64 + 0x34)) {
      iVar3 = FUN_0040c300(*(undefined4 **)((int)local_64 + 0x520));
      local_a0 = FUN_004859b0(iVar3);
      local_9c = *(int *)((int)local_64 + 0x34);
      puVar4 = FUN_004790e0(&local_110,(uint *)0x0,local_9c,(float *)((int)local_64 + 100),0,-1,
                            (undefined4 *)0x0);
      local_70 = (void *)FUN_0044ced0(puVar4);
      if ((*(uint *)((int)local_64 + 0x14) >> 0xb & 3) == 1) {
        FUN_00486370(local_70);
      }
      local_a4 = (void *)((int)local_64 + 0x70);
      fVar6 = FUN_004292e0((float *)&DAT_005aefe4);
      local_a8 = FUN_004292f0(local_a4,local_11c,(float)fVar6);
      FUN_004292f0(local_a8,local_1c,DAT_0056e734);
      FUN_004614a0(local_70,0x1e,6,(undefined4 *)&DAT_0056fc08,local_1c);
      if ((*(uint *)((int)local_64 + 0x14) >> 0xb & 3) == 2) {
        FUN_0047ba30(&local_60);
        local_60 = *(undefined4 *)((int)local_64 + 100);
        local_5c = *(undefined4 *)((int)local_64 + 0x68);
        local_58 = *(undefined4 *)((int)local_64 + 0x6c);
        local_40 = *(undefined4 *)((int)local_64 + 0x88);
        local_ac = FUN_0041cad0(*(int *)((int)local_64 + 0x520));
        FUN_0049db70(&local_10c,4,&local_60,0);
      }
    }
  }
  else {
    if (local_68 != 2) {
      local_f8 = (void *)((int)local_64 + 0x4b0);
      bVar1 = FUN_0047c620(local_f8,0);
      if (bVar1) {
        FUN_00486350((void *)((int)local_64 + 0x5c));
      }
      FUN_00423520((void *)((int)local_64 + 0x4b0),0);
      goto LAB_004851ef;
    }
    if (*(int *)((int)local_64 + 0x4d4) != 0) {
      local_b0 = (void *)((int)local_64 + 0x4a0);
      iVar3 = FUN_00478e00(local_b0,*(int *)((int)local_64 + 0x4d0));
      if (iVar3 != 0) {
        local_b8 = (void *)FUN_0040ff90(*(int *)((int)local_64 + 0x520));
        local_b4 = FUN_0040c300((undefined4 *)((int)local_64 + 0x88));
        FUN_004f8b90(local_b8,(float *)((int)local_64 + 100),local_b4);
        FUN_00423520((void *)((int)local_64 + 0x4a0),0);
        *(int *)((int)local_64 + 0x4d4) = *(int *)((int)local_64 + 0x4d4) + -1;
        iVar3 = FUN_00464080(0);
        cVar2 = FUN_00485a40(iVar3);
        if (cVar2 != '\0') {
          local_bc = (void *)FUN_00464080(0);
          FUN_00484850(local_bc,1);
          iVar3 = FUN_00464080(0);
          iVar3 = FUN_00485920(iVar3);
          if ((0x16 < iVar3) && (*(int *)((int)local_64 + 0x18) == 0)) {
            iVar3 = FUN_00464230(0);
            local_c0 = (void *)FUN_004859d0(iVar3);
            FUN_004865b0(local_c0,1);
            iVar3 = FUN_00464080(0);
            cVar2 = FUN_00464400(iVar3);
            if (cVar2 == '\0') {
              local_d0 = (void *)FUN_00464080(0);
              local_cc = FUN_00423ee0(0x5ba4a8);
              local_cc = local_cc % 10;
              FUN_00486430(local_d0,local_cc);
            }
            else {
              local_c8 = (void *)FUN_00464080(0);
              uVar5 = FUN_00423ee0(0x5ba4a8);
              local_c4 = uVar5 % 10 + 7;
              FUN_00486430(local_c8,local_c4);
            }
            goto LAB_00484ca1;
          }
        }
      }
    }
    local_d4 = (undefined4 *)((int)local_64 + 0x4a0);
    FUN_00423540(local_d4);
    local_d8 = (undefined4 *)((int)local_64 + 0x4b0);
    FUN_00423540(local_d8);
    FUN_00423520((void *)((int)local_64 + 0x4c0),0);
    if ((*(uint *)((int)local_64 + 0x94) >> 1 & 1) == 0) {
      local_dc = (void *)((int)local_64 + 0x4b0);
      bVar1 = FUN_00423590(local_dc,1);
      if (bVar1) {
        FUN_00486310((void *)((int)local_64 + 0x5c));
      }
      local_e0 = (void *)((int)local_64 + 0x4b0);
      local_6c = FUN_0045c2e0(local_e0,2);
      local_6c = 0xd0 - local_6c;
      if (0xff < local_6c) {
        local_6c = 0xff;
      }
      if (local_6c < 0x60) {
        local_6c = 0x60;
      }
      local_fc = *(void **)((int)local_64 + 0x1c);
      FUN_004863c0(local_fc,1);
      local_e8 = *(void **)((int)local_64 + 0x1c);
      FUN_004864b0(local_e8,0xff,(char)local_6c,0x80,0xff);
      fVar6 = FUN_004298b0(0x5ba4c4,DAT_0056c8cc);
      local_ec = (float)fVar6;
      fVar6 = FUN_004298b0(0x5ba4c4,DAT_0056c8cc);
      local_f0 = (float)fVar6;
      FUN_00422dd0(local_28,local_f0,local_ec,0);
      local_f4 = *(void **)((int)local_64 + 0x1c);
      FUN_00470af0(local_f4,local_28);
    }
  }
LAB_004851ef:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

