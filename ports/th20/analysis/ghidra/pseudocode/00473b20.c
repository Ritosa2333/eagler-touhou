/* Entry: 0x00473b20; symbol: FUN_00473b20; body bytes: 3153 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00473b20(float *param_1,float param_2,void *param_3)

{
  float *pfVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  float10 fVar5;
  undefined1 local_290 [12];
  undefined1 local_284 [12];
  undefined1 local_278 [12];
  undefined1 local_26c [12];
  double local_260;
  double local_258;
  double local_250;
  double local_248;
  double local_240;
  double local_238;
  double local_230;
  double local_228;
  float local_220;
  void *local_21c;
  void *local_218;
  float *local_214;
  void *local_210;
  int local_20c;
  int local_208;
  int local_204;
  int local_200;
  int local_1fc;
  int local_1f8;
  int local_1f4;
  int local_1f0;
  float local_1ec;
  float local_1e8;
  float local_1e4;
  float local_1e0;
  uint local_1dc;
  int local_1d8;
  undefined4 local_1d4 [16];
  float local_194;
  float local_190;
  float local_18c;
  float local_188 [48];
  float local_c8;
  float local_c4;
  float local_c0;
  float fStack_bc;
  float fStack_b8;
  float fStack_b4;
  float fStack_b0;
  float fStack_ac;
  float fStack_a8;
  float fStack_a4;
  float fStack_a0;
  float fStack_9c;
  float fStack_98;
  float fStack_94;
  float fStack_90;
  float fStack_8c;
  float fStack_88;
  float fStack_84;
  float fStack_80;
  float fStack_7c;
  float fStack_78;
  float fStack_74;
  float fStack_70;
  float fStack_6c;
  undefined4 uStack_68;
  float fStack_64;
  float fStack_60;
  undefined4 uStack_5c;
  float fStack_58;
  float fStack_54;
  undefined4 uStack_50;
  float fStack_4c;
  float fStack_48;
  undefined4 uStack_44;
  float fStack_40;
  float fStack_3c;
  undefined4 uStack_38;
  float fStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  float fStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  float fStack_1c;
  float fStack_18;
  undefined4 uStack_14;
  float fStack_10;
  float fStack_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040bc20(&local_c8,0xc,0x10,FUN_00422e10);
  FUN_00422e10(&local_194);
  FUN_0040bc20(local_188,0xc,0x10,FUN_00422e10);
  FUN_00447dc0(local_1d4);
  local_21c = (void *)(local_1d8 + 0x18);
  pfVar1 = (float *)FUN_00429570(local_21c,local_26c,param_1);
  local_c8 = *pfVar1;
  local_c4 = pfVar1[1];
  local_c0 = pfVar1[2];
  local_214 = &local_c8;
  local_218 = param_3;
  pfVar1 = (float *)FUN_00429570(param_3,local_278,(float *)((int)param_3 + 0x3c));
  pfVar1 = (float *)FUN_00429440(local_214,local_284,pfVar1);
  local_c8 = *pfVar1;
  local_c4 = pfVar1[1];
  local_c0 = pfVar1[2];
  fVar5 = FUN_00445720(&local_c8);
  local_220 = (float)fVar5;
  if ((local_220 <= param_2) && (*(int *)(local_1d8 + 4) == 0)) {
    local_210 = (void *)(local_1d8 + 0x24);
    pfVar1 = (float *)FUN_00452ff0(local_210,local_290,DAT_0056c8d0);
    local_194 = *pfVar1;
    local_190 = pfVar1[1];
    local_18c = pfVar1[2];
    local_c8 = *(float *)(local_1d8 + 0x18) + local_194;
    local_c4 = *(float *)(local_1d8 + 0x1c) + local_190;
    local_c0 = *(float *)(local_1d8 + 0x20) + local_18c;
    fStack_bc = *(float *)(local_1d8 + 0x18) + local_194;
    fStack_b8 = *(float *)(local_1d8 + 0x1c) + local_190;
    fStack_b4 = *(float *)(local_1d8 + 0x20) - local_18c;
    fStack_b0 = *(float *)(local_1d8 + 0x18) + local_194;
    fStack_ac = *(float *)(local_1d8 + 0x1c) - local_190;
    fStack_a8 = *(float *)(local_1d8 + 0x20) + local_18c;
    fStack_a4 = *(float *)(local_1d8 + 0x18) + local_194;
    fStack_a0 = *(float *)(local_1d8 + 0x1c) - local_190;
    fStack_9c = *(float *)(local_1d8 + 0x20) - local_18c;
    fStack_98 = *(float *)(local_1d8 + 0x18) - local_194;
    fStack_94 = *(float *)(local_1d8 + 0x1c) + local_190;
    fStack_90 = *(float *)(local_1d8 + 0x20) + local_18c;
    fStack_8c = *(float *)(local_1d8 + 0x18) - local_194;
    fStack_88 = *(float *)(local_1d8 + 0x1c) + local_190;
    fStack_84 = *(float *)(local_1d8 + 0x20) - local_18c;
    fStack_80 = *(float *)(local_1d8 + 0x18) - local_194;
    fStack_7c = *(float *)(local_1d8 + 0x1c) - local_190;
    fStack_78 = *(float *)(local_1d8 + 0x20) + local_18c;
    fStack_74 = *(float *)(local_1d8 + 0x18) - local_194;
    fStack_70 = *(float *)(local_1d8 + 0x1c) - local_190;
    fStack_6c = *(float *)(local_1d8 + 0x20) - local_18c;
    uStack_68 = *(undefined4 *)(local_1d8 + 0x18);
    fStack_64 = *(float *)(local_1d8 + 0x1c) - local_190;
    fStack_60 = *(float *)(local_1d8 + 0x20) - local_18c;
    uStack_5c = *(undefined4 *)(local_1d8 + 0x18);
    fStack_58 = *(float *)(local_1d8 + 0x1c) + local_190;
    fStack_54 = *(float *)(local_1d8 + 0x20) - local_18c;
    uStack_50 = *(undefined4 *)(local_1d8 + 0x18);
    fStack_4c = *(float *)(local_1d8 + 0x1c) - local_190;
    fStack_48 = *(float *)(local_1d8 + 0x20) + local_18c;
    uStack_44 = *(undefined4 *)(local_1d8 + 0x18);
    fStack_40 = *(float *)(local_1d8 + 0x1c) + local_190;
    fStack_3c = *(float *)(local_1d8 + 0x20) + local_18c;
    uStack_38 = *(undefined4 *)(local_1d8 + 0x18);
    fStack_34 = *(float *)(local_1d8 + 0x1c) - local_190;
    uStack_30 = *(undefined4 *)(local_1d8 + 0x20);
    uStack_2c = *(undefined4 *)(local_1d8 + 0x18);
    fStack_28 = *(float *)(local_1d8 + 0x1c) + local_190;
    uStack_24 = *(undefined4 *)(local_1d8 + 0x20);
    uStack_20 = *(undefined4 *)(local_1d8 + 0x18);
    fStack_1c = *(float *)(local_1d8 + 0x1c) - local_190;
    fStack_18 = *(float *)(local_1d8 + 0x20) - local_18c / DAT_0056c8d0;
    uStack_14 = *(undefined4 *)(local_1d8 + 0x18);
    fStack_10 = *(float *)(local_1d8 + 0x1c) + local_190;
    fStack_c = local_18c / DAT_0056c8d0 + *(float *)(local_1d8 + 0x20);
    puVar3 = &DAT_0056f7c8;
    puVar4 = local_1d4;
    for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar4 = puVar4 + 1;
    }
    D3DXMatrixTranslation(local_1d4,*param_1,param_1[1],param_1[2]);
    D3DXVec3ProjectArray
              (local_188,0xc,&local_c8,0xc,(int)param_3 + 0xe0,(int)param_3 + 0xa0,
               (int)param_3 + 0x60,local_1d4,0x10);
    local_1f0 = *(int *)((int)param_3 + 0xe0);
    local_228 = (double)local_1f0 + *(double *)(&DAT_0056cdd0 + (local_1f0 >> 0x1f) * -8);
    local_1e4 = DAT_0056cdac + 0.0 + DAT_0056d7bc + (float)local_228;
    local_1f4 = *(int *)((int)param_3 + 0xe4);
    local_230 = (double)local_1f4 + *(double *)(&DAT_0056cdd0 + (local_1f4 >> 0x1f) * -8);
    local_1ec = DAT_0056cda8 + 0.0 + DAT_0056d7bc + (float)local_230;
    local_1f8 = *(int *)((int)param_3 + 0xe0);
    local_238 = (double)local_1f8 + *(double *)(&DAT_0056cdd0 + (local_1f8 >> 0x1f) * -8);
    local_1e0 = (0.0 - DAT_0056d7bc) + (float)local_238;
    local_1fc = *(int *)((int)param_3 + 0xe4);
    local_240 = (double)local_1fc + *(double *)(&DAT_0056cdd0 + (local_1fc >> 0x1f) * -8);
    local_1e8 = (0.0 - DAT_0056d7bc) + (float)local_240;
    for (local_1dc = 0; local_1dc < 0x10; local_1dc = local_1dc + 1) {
      if ((_DAT_0056e0e8 <= local_188[local_1dc * 3 + 2]) &&
         (local_188[local_1dc * 3 + 2] <= DAT_0056c8cc)) {
        if (local_188[local_1dc * 3] <= local_1e4 && local_1e4 != local_188[local_1dc * 3]) {
          local_1e4 = local_188[local_1dc * 3];
        }
        if (local_1e0 < local_188[local_1dc * 3]) {
          local_1e0 = local_188[local_1dc * 3];
        }
        if (local_188[local_1dc * 3 + 1] <= local_1ec && local_1ec != local_188[local_1dc * 3 + 1])
        {
          local_1ec = local_188[local_1dc * 3 + 1];
        }
        if (local_1e8 < local_188[local_1dc * 3 + 1]) {
          local_1e8 = local_188[local_1dc * 3 + 1];
        }
      }
    }
    local_200 = *(int *)((int)param_3 + 0xe0);
    local_248 = (double)local_200 + *(double *)(&DAT_0056cdd0 + (local_200 >> 0x1f) * -8);
    if ((float)local_248 + _DAT_0056e0e8 <= local_1e0) {
      local_204 = *(int *)((int)param_3 + 0xe0);
      local_250 = (double)local_204 + *(double *)(&DAT_0056cdd0 + (local_204 >> 0x1f) * -8);
      if (local_1e4 <= DAT_0056cdac + 0.0 + (float)local_250) {
        local_208 = *(int *)((int)param_3 + 0xe4);
        local_258 = (double)local_208 + *(double *)(&DAT_0056cdd0 + (local_208 >> 0x1f) * -8);
        if ((float)local_258 + _DAT_0056e0e8 <= local_1e8) {
          local_20c = *(int *)((int)param_3 + 0xe4);
          local_260 = (double)local_20c + *(double *)(&DAT_0056cdd0 + (local_20c >> 0x1f) * -8);
        }
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

