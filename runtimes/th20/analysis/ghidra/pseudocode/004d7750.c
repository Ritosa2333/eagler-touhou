/* Entry: 0x004d7750; symbol: FUN_004d7750; body bytes: 1484 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004d7750(int *param_1)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  float10 fVar4;
  undefined1 local_a4 [12];
  undefined1 local_98 [12];
  undefined1 local_8c [12];
  int *local_80;
  int *local_7c;
  int *local_78;
  int *local_74;
  int *local_70;
  int *local_6c;
  void *local_68;
  float local_64;
  void *local_60;
  float local_5c;
  int *local_58;
  float local_54;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  float local_40;
  void *local_3c;
  int *local_38;
  void *local_34;
  int *local_30;
  int *local_2c;
  float local_28;
  int *local_24;
  int local_20;
  int local_1c;
  int *local_18;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = param_1;
  if (((uint)param_1[7] >> 5 & 1) == 0) {
    param_1[7] = param_1[7] | 0x20;
    param_1[7] = param_1[7] & 0xffffffbf;
    do {
      (**(code **)(*local_18 + 8))();
      local_24 = local_18;
      if (local_18[0x1a6] == 0 && local_18[0x1a7] == 0) break;
      local_1c = 0;
      if ((local_18[0x1a6] & 1U) != 0) {
        iVar2 = (**(code **)(*local_18 + 0x50))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)local_18[0x1a6] >> 2 & 1) != 0) {
        iVar2 = (**(code **)(*local_18 + 0x54))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)local_18[0x1a6] >> 0x15 & 1) != 0) {
        iVar2 = (**(code **)(*local_18 + 0x58))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)local_18[0x1a6] >> 3 & 1) != 0) {
        iVar2 = (**(code **)(*local_18 + 0x5c))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)local_18[0x1a6] >> 4 & 1) != 0) {
        local_2c = local_18 + 0x24;
        iVar2 = FUN_0047c680(local_2c,3);
        local_20 = *(int *)(iVar2 + 0x3c);
        if (local_20 == 0) {
          iVar2 = (**(code **)(*local_18 + 0x60))();
          local_1c = iVar2 + local_1c;
        }
        else if (local_20 == 1) {
          iVar2 = (**(code **)(*local_18 + 0x68))();
          local_1c = iVar2 + local_1c;
        }
        else if (local_20 == 4) {
          iVar2 = (**(code **)(*local_18 + 100))();
          local_1c = iVar2 + local_1c;
        }
      }
      if (((uint)local_18[0x1a6] >> 6 & 1) != 0) {
        iVar2 = (**(code **)(*local_18 + 0x6c))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)local_18[0x1a6] >> 0xc & 1) != 0) {
        iVar2 = (**(code **)(*local_18 + 0x70))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)local_18[0x1a6] >> 0x1e & 1) != 0) {
        local_30 = local_18 + 0x24;
        local_34 = (void *)FUN_0047c680(local_30,5);
        bVar1 = FUN_00423590(local_34,0);
        if (bVar1) {
          local_18[0x1a6] = local_18[0x1a6] & 0xbfffffff;
          local_1c = local_1c + 1;
        }
        else {
          local_38 = local_18 + 0x24;
          local_3c = (void *)FUN_0047c680(local_38,5);
          FUN_00429420(local_3c);
        }
      }
      if (local_18[0x1b3] != 0) {
        local_18[0x1b3] = local_18[0x1b3] + -1;
      }
    } while (local_1c != 0);
    if ((float)local_18[0x1c2] < (float)local_18[0x1d] ||
        (float)local_18[0x1c2] == (float)local_18[0x1d]) {
      fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
      local_4c = (float)fVar4;
      local_54 = local_4c * (float)local_18[0x1f] * (float)local_18[0x21] * (float)local_18[0x22];
      local_50 = (float)local_18[0x20];
      local_18[0x20] = (int)(local_50 + local_54);
      local_58 = local_18 + 0x19;
      fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
      local_60 = FUN_004292f0(local_58,local_8c,(float)fVar4);
      local_5c = (float)local_18[0x21];
      local_68 = FUN_004292f0(local_60,local_98,local_5c);
      local_64 = (float)local_18[0x22];
      pfVar3 = (float *)FUN_004292f0(local_68,local_a4,local_64);
      FUN_004296e0(local_18 + 0x16,pfVar3);
      if ((_DAT_0056e0e8 < (float)local_18[0x1c4]) &&
         ((float)local_18[0x1c4] <= (float)local_18[0x20] + (float)local_18[0x1d] &&
          (float)local_18[0x20] + (float)local_18[0x1d] != (float)local_18[0x1c4])) {
        local_28 = (float)local_18[0x1c4] - (float)local_18[0x20];
        local_18[0x1d] = (int)local_28;
        local_18[0x1c2] = (int)local_28;
        if ((float)local_18[0x1d] <= 0.0) goto LAB_004d7d0e;
      }
    }
    else {
      fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
      local_40 = (float)fVar4;
      local_48 = local_40 * (float)local_18[0x1f] * (float)local_18[0x21] * (float)local_18[0x22];
      local_44 = (float)local_18[0x1d];
      local_18[0x1d] = (int)(local_44 + local_48);
      if ((float)local_18[0x1c2] <= (float)local_18[0x1d] &&
          (float)local_18[0x1d] != (float)local_18[0x1c2]) {
        local_18[0x1d] = local_18[0x1c2];
      }
    }
    local_6c = local_18 + 0x1ab;
    bVar1 = FUN_00423590(local_6c,0);
    if (bVar1) {
      local_70 = local_18 + 0x1af;
      bVar1 = FUN_00423590(local_70,0);
      if (bVar1) {
        FUN_00422e10(local_14);
        FUN_00439330(local_14,local_14,(float)local_18[0x1c],(float)local_18[0x1d]);
        FUN_004296e0(local_14,(float *)(local_18 + 0x16));
        iVar2 = FUN_00485390((float *)(local_18 + 0x16),(float)local_18[0x1e],(float)local_18[0x1e])
        ;
        if (iVar2 != 0) {
          FUN_00485390(local_14,(float)local_18[0x1e],(float)local_18[0x1e]);
        }
        goto LAB_004d7d0e;
      }
    }
    local_74 = local_18 + 0x1ab;
    bVar1 = FUN_00461070(local_74,0);
    if (bVar1) {
      local_78 = local_18 + 0x1ab;
      FUN_00429420(local_78);
    }
    local_7c = local_18 + 0x1af;
    bVar1 = FUN_00461070(local_7c,0);
    if (bVar1) {
      local_80 = local_18 + 0x1af;
      FUN_00429420(local_80);
    }
  }
LAB_004d7d0e:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

