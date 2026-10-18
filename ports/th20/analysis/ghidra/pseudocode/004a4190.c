/* Entry: 0x004a4190; symbol: FUN_004a4190; body bytes: 1649 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004a4190(int param_1)

{
  float fVar1;
  float *pfVar2;
  int iVar3;
  void *extraout_ECX;
  void *extraout_ECX_00;
  float10 fVar4;
  undefined1 local_88 [12];
  float local_7c;
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_68;
  float local_64;
  float local_60;
  float local_5c;
  void *local_58;
  float local_54;
  float local_50;
  float local_4c;
  int local_48;
  int local_44;
  float local_40;
  float local_3c;
  undefined1 local_35;
  float *local_34;
  float *local_30;
  int local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_2c = param_1;
  if (*(int *)(param_1 + 0x2d4) != 0) {
    local_34 = *(float **)(**(int **)(param_1 + 0x2d4) + 0x18);
    FUN_00422e10(&local_20);
    FUN_00422e10(&local_14);
    local_40 = *(float *)(*(int *)(local_2c + 0x2d4) + 0xc);
    local_24 = *(float *)(*(int *)(local_2c + 0x2d4) + 0x14);
    local_28 = *(float *)(*(int *)(local_2c + 0x2d4) + 0x18);
    local_7c = DAT_0056d7bc;
    fVar1 = *(float *)(*(int *)(local_2c + 0x2d4) + 8);
    pfVar2 = (float *)(*(int *)(local_2c + 0x2d4) + 0xc);
    if (*pfVar2 <= fVar1 && fVar1 != *pfVar2) {
      fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
      local_4c = (float)fVar4;
      local_54 = local_4c * DAT_0056c8d0;
      local_50 = *(float *)(*(int *)(local_2c + 0x2d4) + 0xc);
      *(float *)(*(int *)(local_2c + 0x2d4) + 0xc) = local_50 + local_54;
    }
    local_35 = 0xff;
    pfVar2 = (float *)(*(int *)(local_2c + 0x2d4) + 0xc);
    if (DAT_0056cd94 < *pfVar2 || DAT_0056cd94 == *pfVar2) {
      pfVar2 = (float *)(*(int *)(local_2c + 0x2d4) + 0xc);
      if (*pfVar2 <= DAT_0056fa28 && DAT_0056fa28 != *pfVar2) {
        local_35 = (undefined1)
                   (int)(((*(float *)(*(int *)(local_2c + 0x2d4) + 0xc) - DAT_0056cd94) *
                         DAT_0056cd9c) / DAT_0056cd94);
      }
    }
    else {
      local_35 = 0;
    }
    pfVar2 = (float *)FUN_00499350(local_2c);
    local_20 = *pfVar2;
    local_1c = pfVar2[1];
    local_18 = pfVar2[2];
    local_58 = (void *)**(undefined4 **)(local_2c + 0x2d4);
    FUN_0049d850(local_58,(local_20 - local_40) - _DAT_0056ec8c,
                 (local_1c - local_40) - _DAT_0056ec8c,local_40 * DAT_0056c8d0 + DAT_00570560,
                 local_40 * DAT_0056c8d0 + DAT_00570560);
    iVar3 = FUN_0049df00(&DAT_005b6758,*(int *)(local_2c + 0x2e8));
    local_20 = (float)iVar3 + local_20;
    iVar3 = FUN_0049df20(&DAT_005b6758,*(int *)(local_2c + 0x2e8));
    local_1c = (float)iVar3 + local_1c;
    local_30 = *(float **)(**(int **)(local_2c + 0x2d4) + 0x14);
    for (local_44 = 0; local_44 < *(int *)**(undefined4 **)(local_2c + 0x2d4);
        local_44 = local_44 + 1) {
      for (local_48 = 0; local_48 < *(int *)(**(int **)(local_2c + 0x2d4) + 4);
          local_48 = local_48 + 1) {
        pfVar2 = (float *)FUN_00429440(local_34,local_88,&local_20);
        local_14 = *pfVar2;
        local_10 = pfVar2[1];
        local_c = pfVar2[2];
        fVar4 = FUN_00456130(&local_14);
        local_3c = local_40 * local_40 - (float)fVar4;
        if (local_3c < _DAT_0056e0e8) {
          *(undefined1 *)((int)local_30 + 0x13) = 0;
        }
        else {
          local_3c = local_3c / (local_40 * local_40);
          local_30[4] = *(float *)(*(int *)(local_2c + 0x2d4) + 0x10);
          *(char *)((int)local_30 + 0x12) =
               (char)(int)(DAT_0056cd9c -
                          (float)(int)(0xff - (uint)*(byte *)((int)local_30 + 0x12)) * local_3c);
          *(char *)((int)local_30 + 0x11) =
               (char)(int)(DAT_0056cd9c -
                          (float)(int)(0xff - (uint)*(byte *)((int)local_30 + 0x11)) * local_3c);
          *(char *)(local_30 + 4) =
               (char)(int)(DAT_0056cd9c -
                          (float)(int)(0xff - (uint)*(byte *)(local_30 + 4)) * local_3c);
          *(undefined1 *)((int)local_30 + 0x13) = 0xff;
          FUN_004532c0(&local_14,&local_14,local_3c * DAT_0056cd94);
          fVar4 = FUN_004292e0(&local_24);
          fVar4 = FUN_00439820(extraout_ECX,(float)fVar4);
          local_5c = (float)fVar4;
          local_14 = local_5c * local_3c * DAT_0056d7bc + local_14;
          fVar4 = FUN_004292e0(&local_28);
          fVar4 = FUN_00439820(extraout_ECX_00,(float)fVar4);
          local_60 = (float)fVar4;
          local_10 = local_60 * local_3c * DAT_0056d7bc + local_10;
          FUN_004296e0(local_30,&local_14);
          local_30[2] = 0.0;
          local_34[2] = 0.0;
        }
        FUN_004530c0(&local_24,DAT_0056e0f0 / DAT_0056cd94);
        FUN_00472060(&local_28,DAT_0056e0f0 / DAT_0056fa28);
        if (0.0 < *local_34) {
          if ((float)DAT_005b87f8 <= *local_34) {
            *local_30 = (float)DAT_005b87f8 - DAT_0056c8cc;
            *local_34 = (float)DAT_005b87f8 - DAT_0056c8cc;
          }
        }
        else {
          *local_30 = DAT_0056c8cc;
          *local_34 = DAT_0056c8cc;
        }
        if (0.0 < local_34[1]) {
          if ((float)DAT_005b87fc <= local_34[1]) {
            local_30[1] = (float)DAT_005b87fc - DAT_0056c8cc;
            local_34[1] = (float)DAT_005b87fc - DAT_0056c8cc;
          }
        }
        else {
          local_30[1] = DAT_0056c8cc;
          local_34[1] = DAT_0056c8cc;
        }
        FUN_0049df40(*local_34,local_34[1],local_34[2],local_30 + 5,local_30 + 6);
        local_34 = local_34 + 3;
        local_30 = local_30 + 7;
      }
    }
    local_64 = DAT_0056e0f0 / DAT_0056cd90;
    fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
    local_68 = (float)fVar4;
    local_6c = local_64 * local_68;
    FUN_004530c0((void *)(*(int *)(local_2c + 0x2d4) + 0x14),local_6c);
    local_70 = DAT_0056e0f0 / DAT_0056cd94;
    fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
    local_74 = (float)fVar4;
    local_78 = local_70 * local_74;
    FUN_004530c0((void *)(*(int *)(local_2c + 0x2d4) + 0x18),local_78);
    FUN_0049ddc0((int *)**(undefined4 **)(local_2c + 0x2d4));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

