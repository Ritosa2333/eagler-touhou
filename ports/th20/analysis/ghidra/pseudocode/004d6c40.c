/* Entry: 0x004d6c40; symbol: FUN_004d6c40; body bytes: 1596 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004d6c40(int *param_1)

{
  bool bVar1;
  int iVar2;
  void *pvVar3;
  int *piVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  float10 fVar8;
  int local_38;
  int local_30;
  int local_28;
  float *local_24;
  int local_20;
  int local_1c;
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (((uint)param_1[7] >> 5 & 1) == 0) {
    param_1[7] = param_1[7] | 0x20;
    param_1[7] = param_1[7] & 0xffffffbf;
    do {
      (**(code **)(*param_1 + 8))();
      if (param_1[0x1a6] == 0 && param_1[0x1a7] == 0) break;
      local_1c = 0;
      if ((param_1[0x1a6] & 1U) != 0) {
        local_1c = (**(code **)(*param_1 + 0x50))();
      }
      if (((uint)param_1[0x1a6] >> 2 & 1) != 0) {
        iVar2 = (**(code **)(*param_1 + 0x54))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)param_1[0x1a6] >> 3 & 1) != 0) {
        iVar2 = (**(code **)(*param_1 + 0x5c))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)param_1[0x1a6] >> 4 & 1) != 0) {
        iVar2 = FUN_0047c680(param_1 + 0x24,3);
        iVar2 = *(int *)(iVar2 + 0x3c);
        if (iVar2 == 0) {
          iVar2 = (**(code **)(*param_1 + 0x60))();
          local_1c = iVar2 + local_1c;
        }
        else if (iVar2 == 1) {
          iVar2 = (**(code **)(*param_1 + 0x68))();
          local_1c = iVar2 + local_1c;
        }
        else if (iVar2 == 4) {
          iVar2 = (**(code **)(*param_1 + 100))();
          local_1c = iVar2 + local_1c;
        }
      }
      if (((uint)param_1[0x1a6] >> 6 & 1) != 0) {
        iVar2 = (**(code **)(*param_1 + 0x6c))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)param_1[0x1a6] >> 0xc & 1) != 0) {
        iVar2 = (**(code **)(*param_1 + 0x70))();
        local_1c = iVar2 + local_1c;
      }
      if (param_1[0x1a6] < 0) {
        iVar2 = (**(code **)(*param_1 + 0x78))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)param_1[0x1a6] >> 8 & 1) != 0) {
        iVar2 = (**(code **)(*param_1 + 0x7c))();
        local_1c = iVar2 + local_1c;
      }
      if (((uint)param_1[0x1a6] >> 0x1e & 1) != 0) {
        pvVar3 = (void *)FUN_0047c680(param_1 + 0x24,5);
        bVar1 = FUN_00423590(pvVar3,0);
        if (bVar1) {
          param_1[0x1a6] = param_1[0x1a6] & 0xbfffffff;
          local_1c = local_1c + 1;
        }
        else {
          pvVar3 = (void *)FUN_0047c680(param_1 + 0x24,5);
          FUN_00429420(pvVar3);
        }
      }
      if (param_1[0x1b3] != 0) {
        param_1[0x1b3] = param_1[0x1b3] + -1;
      }
    } while (local_1c != 0);
    iVar2 = param_1[0x4ca];
    if (param_1[0x4c9] == 0) {
      if (((uint)param_1[7] >> 4 & 1) == 0) {
        local_38 = 0;
        for (local_20 = 0; local_20 < param_1[0x1c6]; local_20 = local_20 + 1) {
          fVar8 = FUN_00423bd0((int)(param_1 + 0x12));
          if ((float)fVar8 - (float)local_20 < _DAT_0056e0e8) {
            piVar4 = (int *)(local_20 * 0x20 + iVar2);
            *piVar4 = param_1[0x1be];
            piVar4[1] = param_1[0x1bf];
            piVar4[2] = param_1[0x1c0];
            puVar6 = (undefined4 *)(iVar2 + 0xc + local_20 * 0x20);
            *puVar6 = DAT_00571068;
            puVar6[1] = DAT_0057106c;
            puVar6[2] = DAT_00571070;
            *(int *)(iVar2 + 0x18 + local_20 * 0x20) = param_1[0x1c1];
            *(int *)(iVar2 + 0x1c + local_20 * 0x20) = param_1[0x1c3];
          }
          else {
            fVar8 = FUN_00423bd0((int)(param_1 + 0x12));
            FUN_004d62e0(param_1 + 0x4cc,(undefined4 *)(local_20 * 0x20 + iVar2),
                         (float *)(iVar2 + 0x1c + local_20 * 0x20),
                         (float *)(iVar2 + 0x18 + local_20 * 0x20),
                         (void *)((local_20 + -1) * 0x20 + iVar2),
                         *(float *)(iVar2 + 0x1c + (local_20 + -1) * 0x20),
                         *(float *)(iVar2 + 0x18 + (local_20 + -1) * 0x20),
                         (float)fVar8 - (float)local_20,local_38);
            local_38 = 1;
          }
        }
      }
    }
    else {
      iVar2 = param_1[0x1c6];
      while (local_28 = iVar2 + -1, 0 < local_28) {
        puVar6 = (undefined4 *)(param_1[0x4ca] + (iVar2 + -2) * 0x20);
        puVar7 = (undefined4 *)(param_1[0x4ca] + local_28 * 0x20);
        for (iVar5 = 8; iVar2 = local_28, iVar5 != 0; iVar5 = iVar5 + -1) {
          *puVar7 = *puVar6;
          puVar6 = puVar6 + 1;
          puVar7 = puVar7 + 1;
        }
      }
      *(int *)(param_1[0x4ca] + 0x1c) = param_1[0x1f];
      iVar2 = param_1[0x4ca];
      *(int *)(iVar2 + 0xc) = param_1[0x19];
      *(int *)(iVar2 + 0x10) = param_1[0x1a];
      *(int *)(iVar2 + 0x14) = param_1[0x1b];
      FUN_004296e0((void *)param_1[0x4ca],(float *)(param_1 + 0x19));
      *(int *)(param_1[0x4ca] + 0x18) = param_1[0x1c];
    }
    local_24 = (float *)param_1[0x4ca];
    bVar1 = FUN_00423590(param_1 + 0x1ab,0);
    if ((bVar1) && (((uint)param_1[0x1a6] >> 8 & 1) == 0)) {
      for (local_30 = 0; local_30 < param_1[0x1c6]; local_30 = local_30 + 1) {
        FUN_00422e10(local_14);
        FUN_00439330(local_14,(float *)local_14,(float)param_1[0x1c],(float)param_1[0x1d]);
        FUN_004296e0(local_14,(float *)(param_1 + 0x16));
        iVar2 = FUN_00485390(local_24,(float)param_1[0x1e],(float)param_1[0x1e]);
        if (iVar2 == 0) break;
        local_24 = local_24 + 8;
      }
    }
    else {
      FUN_00429420(param_1 + 0x1ab);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

