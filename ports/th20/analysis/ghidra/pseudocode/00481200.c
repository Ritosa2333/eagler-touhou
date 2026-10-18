/* Entry: 0x00481200; symbol: FUN_00481200; body bytes: 1404 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00481200(int param_1)

{
  bool bVar1;
  void *pvVar2;
  int iVar3;
  float10 fVar4;
  float fVar5;
  float local_4c;
  float local_48;
  int local_44;
  int local_40;
  float local_38 [2];
  float local_30;
  float local_2c;
  float local_28 [8];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  pvVar2 = (void *)FUN_0047c680((void *)(param_1 + 0xa0),0xb);
  FUN_00429420(pvVar2);
  iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),0xb);
  if (*(int *)(iVar3 + 0x30) != 0) {
    iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
    fVar5 = *(float *)(iVar3 + 0x48);
    iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
    iVar3 = FUN_004854d0((float *)(param_1 + 100),*(float *)(iVar3 + 0x4c),fVar5);
    if (iVar3 != 0) {
      FUN_004398a0(&local_30);
      FUN_004398a0(local_38);
      local_28[0] = 0.0;
      local_28[1] = 0.0;
      local_28[2] = 0.0;
      local_28[3] = 0.0;
      local_28[4] = 0.0;
      local_28[5] = 0.0;
      local_28[6] = 0.0;
      local_28[7] = 0.0;
      fVar5 = DAT_0056c8cc;
      fVar4 = FUN_004292e0((float *)(param_1 + 0x84));
      FUN_00439330(local_38,local_38,(float)fVar4,fVar5);
      iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
      local_30 = (DAT_0056fd6c - *(float *)(iVar3 + 0x4c)) / DAT_0056c8d0 -
                 *(float *)(param_1 + 100);
      fVar5 = DAT_0056d7c0 / DAT_0056c8d0;
      iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
      local_2c = (fVar5 + (DAT_0056fd70 - *(float *)(iVar3 + 0x48)) / DAT_0056c8d0) -
                 *(float *)(param_1 + 0x68);
      FUN_0043e910(&local_30,&local_30);
      fVar4 = FUN_00480d30(local_38,&local_30);
      local_28[0] = (float)fVar4;
      fVar4 = FUN_00480d70(local_38,&local_30);
      local_28[4] = (float)fVar4;
      iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
      local_30 = (DAT_0056cda4 + *(float *)(iVar3 + 0x4c)) / DAT_0056c8d0 -
                 *(float *)(param_1 + 100);
      fVar5 = DAT_0056d7c0 / DAT_0056c8d0;
      iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
      local_2c = (fVar5 + (DAT_0056fd70 - *(float *)(iVar3 + 0x48)) / DAT_0056c8d0) -
                 *(float *)(param_1 + 0x68);
      FUN_0043e910(&local_30,&local_30);
      fVar4 = FUN_00480d30(local_38,&local_30);
      local_28[1] = (float)fVar4;
      fVar4 = FUN_00480d70(local_38,&local_30);
      local_28[5] = (float)fVar4;
      iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
      local_30 = (DAT_0056fd6c - *(float *)(iVar3 + 0x4c)) / DAT_0056c8d0 -
                 *(float *)(param_1 + 100);
      fVar5 = DAT_0056d7c0 / DAT_0056c8d0;
      iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
      local_2c = (fVar5 + (DAT_0056d7c0 + *(float *)(iVar3 + 0x48)) / DAT_0056c8d0) -
                 *(float *)(param_1 + 0x68);
      FUN_0043e910(&local_30,&local_30);
      fVar4 = FUN_00480d30(local_38,&local_30);
      local_28[2] = (float)fVar4;
      fVar4 = FUN_00480d70(local_38,&local_30);
      local_28[6] = (float)fVar4;
      iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
      local_30 = (DAT_0056cda4 + *(float *)(iVar3 + 0x4c)) / DAT_0056c8d0 -
                 *(float *)(param_1 + 100);
      fVar5 = DAT_0056d7c0 / DAT_0056c8d0;
      iVar3 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
      local_2c = (fVar5 + (DAT_0056d7c0 + *(float *)(iVar3 + 0x48)) / DAT_0056c8d0) -
                 *(float *)(param_1 + 0x68);
      FUN_0043e910(&local_30,&local_30);
      fVar4 = FUN_00480d30(local_38,&local_30);
      local_28[3] = (float)fVar4;
      fVar4 = FUN_00480d70(local_38,&local_30);
      local_28[7] = (float)fVar4;
      local_48 = DAT_0056fd7c;
      for (local_40 = 0; local_40 < 4; local_40 = local_40 + 1) {
        if (((local_28[local_40] <= 0.0) && (local_48 < local_28[local_40 + 4])) &&
           (_DAT_0056e0e8 <= local_28[local_40 + 4])) {
          local_48 = local_28[local_40 + 4];
        }
      }
      local_4c = DAT_0056fd7c;
      for (local_44 = 0; local_44 < 4; local_44 = local_44 + 1) {
        if (((_DAT_0056e0e8 <= local_28[local_44]) && (local_4c < local_28[local_44 + 4])) &&
           (_DAT_0056e0e8 <= local_28[local_44 + 4])) {
          local_4c = local_28[local_44 + 4];
        }
      }
      if ((local_48 < DAT_0056fd78) || (local_4c < DAT_0056fd78)) {
        *(uint *)(param_1 + 0x90) = *(uint *)(param_1 + 0x90) & 0xfffffeff;
        goto LAB_0048176e;
      }
    }
  }
  pvVar2 = (void *)FUN_0047c680((void *)(param_1 + 0xa0),0xb);
  bVar1 = FUN_00423590(pvVar2,0);
  if (bVar1) {
    *(uint *)(param_1 + 0x90) = *(uint *)(param_1 + 0x90) & 0xfffffeff;
  }
LAB_0048176e:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

