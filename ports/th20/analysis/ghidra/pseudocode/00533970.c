/* Entry: 0x00533970; symbol: FUN_00533970; body bytes: 1775 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00533970(int param_1)

{
  byte bVar1;
  bool bVar2;
  int iVar3;
  void *extraout_ECX;
  void *extraout_ECX_00;
  float10 fVar4;
  float fVar5;
  float *local_44;
  int local_40;
  int local_3c;
  float *local_38;
  float *local_30;
  undefined4 local_28;
  float local_24;
  float local_20;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (*(int *)(param_1 + 0x40) != 0) {
    bVar2 = FUN_004235c0((void *)(param_1 + 0x44),0x50);
    if (bVar2) {
      FUN_004a2800(*(void **)(param_1 + 0x40));
      *(undefined4 *)(param_1 + 0x40) = 0;
    }
    else {
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_0041cab0(iVar3);
      if (iVar3 == 7) {
        local_44 = (float *)(**(code **)(**(int **)(param_1 + 0x28) + 0x60))();
      }
      else {
        iVar3 = FUN_00460830(0);
        local_44 = (float *)FUN_00460850(iVar3);
      }
      local_18 = *local_44;
      local_14 = local_44[1];
      local_10 = local_44[2];
      FUN_0049d850(*(void **)(param_1 + 0x40),local_18 - DAT_00570ad0,local_14 - DAT_00570ad0,
                   DAT_00570ad0 * DAT_0056c8d0,DAT_00570ad0 * DAT_0056c8d0);
      iVar3 = FUN_0049df00(&DAT_005b6758,*(int *)(param_1 + 0x7c));
      local_18 = (float)iVar3 + local_18;
      iVar3 = FUN_0049df20(&DAT_005b6758,*(int *)(param_1 + 0x7c));
      local_14 = (float)iVar3 + local_14;
      FUN_00447de0(&local_28);
      FUN_00447de0(&local_c);
      iVar3 = FUN_0040ff90(param_1 + 0x44);
      FUN_00452f20(&local_28,((float)iVar3 * DAT_0056e0f0) / DAT_0056e734);
      local_38 = *(float **)(*(int *)(param_1 + 0x40) + 0x18);
      local_30 = *(float **)(*(int *)(param_1 + 0x40) + 0x14);
      for (local_3c = 0; local_3c < **(int **)(param_1 + 0x40); local_3c = local_3c + 1) {
        for (local_40 = 0; local_40 < *(int *)(*(int *)(param_1 + 0x40) + 4);
            local_40 = local_40 + 1) {
          FUN_00429440(local_38,&local_24,&local_18);
          fVar4 = (float10)FUN_0047a2a0(&local_24);
          fVar5 = (float)((uint)(DAT_0056cda4 + DAT_0056cd94) ^ _DAT_0056d7d0) / DAT_0056c8d0;
          iVar3 = FUN_0049df00(&DAT_005b6758,*(int *)(param_1 + 0x7c));
          fVar5 = fVar5 + (float)iVar3;
          if ((((fVar5 < *local_38 || fVar5 == *local_38) &&
               (fVar5 = (DAT_0056cda4 + DAT_0056cd94) / DAT_0056c8d0,
               iVar3 = FUN_0049df00(&DAT_005b6758,*(int *)(param_1 + 0x7c)),
               *local_38 <= fVar5 + (float)iVar3)) &&
              (iVar3 = FUN_0049df20(&DAT_005b6758,*(int *)(param_1 + 0x7c)),
              (float)(iVar3 + -0x20) < local_38[1] || (float)(iVar3 + -0x20) == local_38[1])) &&
             (fVar5 = DAT_0056d7c0 + DAT_0056cd94,
             iVar3 = FUN_0049df20(&DAT_005b6758,*(int *)(param_1 + 0x7c)),
             local_38[1] <= fVar5 + (float)iVar3)) {
            if (DAT_00570ad0 < (float)fVar4) {
              *(undefined1 *)((int)local_30 + 0x13) = 0;
            }
            else {
              fVar5 = (float)fVar4 / DAT_00570ad0;
              local_30[4] = -NAN;
              if (DAT_00573160 < fVar5) {
                *(char *)((int)local_30 + 0x13) =
                     (char)(int)(DAT_0056cd9c -
                                ((fVar5 - DAT_00573160) * DAT_0056cd9c) / DAT_0056fbf8);
              }
              bVar2 = FUN_00461070((void *)(param_1 + 0x44),0x3c);
              if (bVar2) {
                bVar1 = *(byte *)((int)local_30 + 0x13);
                iVar3 = FUN_0040ff90(param_1 + 0x44);
                *(char *)((int)local_30 + 0x13) = (char)((int)((uint)bVar1 * (0x50 - iVar3)) / 0x14)
                ;
              }
              FUN_00452f20(&local_c,DAT_0056e0f0 * DAT_0056c8d0 * fVar5);
              iVar3 = FUN_0040ff90(param_1 + 0x44);
              FUN_00472060(&local_c,(((float)iVar3 * DAT_0056e0f0) / DAT_0056c8d0) / DAT_0056e048);
              fVar4 = FUN_004292e0(&local_c);
              fVar4 = FUN_00439820(extraout_ECX,(float)fVar4);
              local_24 = (float)fVar4 * fVar5 * DAT_0056e0ec * local_24;
              fVar4 = FUN_004292e0(&local_c);
              fVar4 = FUN_00439820(extraout_ECX_00,(float)fVar4);
              local_20 = (float)fVar4 * fVar5 * DAT_0056e0ec * local_20;
              FUN_004296e0(local_30,&local_24);
              fVar5 = (float)((uint)(DAT_0056cda4 + DAT_0056cd94) ^ _DAT_0056d7d0) / DAT_0056c8d0;
              iVar3 = FUN_0049df00(&DAT_005b6758,*(int *)(param_1 + 0x7c));
              fVar5 = fVar5 + (float)iVar3;
              if (fVar5 < *local_30 || fVar5 == *local_30) {
                fVar5 = (DAT_0056cda4 + DAT_0056cd94) / DAT_0056c8d0;
                iVar3 = FUN_0049df00(&DAT_005b6758,*(int *)(param_1 + 0x7c));
                if (fVar5 + (float)iVar3 < *local_30) {
                  fVar5 = (DAT_0056cda4 + DAT_0056cd94) / DAT_0056c8d0;
                  iVar3 = FUN_0049df00(&DAT_005b6758,*(int *)(param_1 + 0x7c));
                  *local_30 = fVar5 + (float)iVar3;
                }
              }
              else {
                fVar5 = (float)((uint)(DAT_0056cda4 + DAT_0056cd94) ^ _DAT_0056d7d0) / DAT_0056c8d0;
                iVar3 = FUN_0049df00(&DAT_005b6758,*(int *)(param_1 + 0x7c));
                *local_30 = fVar5 + (float)iVar3;
              }
              iVar3 = FUN_0049df20(&DAT_005b6758,*(int *)(param_1 + 0x7c));
              if ((float)(iVar3 + -0x20) < local_30[1] || (float)(iVar3 + -0x20) == local_30[1]) {
                fVar5 = DAT_0056d7c0 + DAT_0056cd94;
                iVar3 = FUN_0049df20(&DAT_005b6758,*(int *)(param_1 + 0x7c));
                if (fVar5 + (float)iVar3 < local_30[1]) {
                  fVar5 = DAT_0056d7c0 + DAT_0056cd94;
                  iVar3 = FUN_0049df20(&DAT_005b6758,*(int *)(param_1 + 0x7c));
                  local_30[1] = fVar5 + (float)iVar3;
                }
              }
              else {
                iVar3 = FUN_0049df20(&DAT_005b6758,*(int *)(param_1 + 0x7c));
                local_30[1] = (float)iVar3 + _DAT_00576360;
              }
              local_30[2] = 0.0;
              local_38[2] = 0.0;
            }
          }
          local_38 = local_38 + 3;
          local_30 = local_30 + 7;
        }
      }
      FUN_0049ddc0(*(int **)(param_1 + 0x40));
      FUN_00423540((undefined4 *)(param_1 + 0x44));
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

