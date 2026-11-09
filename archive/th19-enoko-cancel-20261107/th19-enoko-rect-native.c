// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 4d2c30

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4
FUN_004d2c30(float param_1,float param_2,float param_3,float param_4,float param_5,float param_6)

{
  char cVar1;
  float *pfVar2;
  int iVar3;
  int *piVar4;
  float10 fVar5;
  float10 extraout_ST0;
  float in_XMM0_Da;
  float in_XMM1_Da;
  float in_XMM2_Da;
  float in_XMM3_Da;
  undefined4 *puStack_68;
  float local_64;
  float local_60;
  float local_5c;
  float fStack_58;
  float fStack_54;
  float local_50;
  float local_4c [5];
  float fStack_38;
  undefined4 *puStack_34;
  float fStack_30;
  float local_2c [5];
  float fStack_18;
  float fStack_14;
  float fStack_10;
  uint local_c;
  
  local_c = DAT_005c3d80 ^ (uint)&puStack_68;
  iVar3 = 4;
  pfVar2 = local_2c;
  do {
    *pfVar2 = 0.0;
    pfVar2[1] = 0.0;
    iVar3 = iVar3 + -1;
    pfVar2 = pfVar2 + 2;
  } while (iVar3 != 0);
  pfVar2 = local_4c;
  iVar3 = 4;
  do {
    *pfVar2 = 0.0;
    pfVar2[1] = 0.0;
    pfVar2 = pfVar2 + 2;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  local_64 = in_XMM3_Da;
  local_60 = in_XMM2_Da;
  local_5c = in_XMM0_Da;
  local_50 = in_XMM1_Da;
  fVar5 = (float10)FUN_0047a200((double)((in_XMM1_Da - param_3) * (in_XMM1_Da - param_3) +
                                        (in_XMM0_Da - param_2) * (in_XMM0_Da - param_2)));
  fStack_54 = (float)fVar5;
  fVar5 = (float10)FUN_0047a200((double)((local_64 / DAT_005aa280) * (local_64 / DAT_005aa280) +
                                        (local_60 / DAT_005aa280) * (local_60 / DAT_005aa280)));
  fStack_58 = (float)fVar5;
  FUN_0047a200((double)((param_4 / DAT_005aa280) * (param_4 / DAT_005aa280) +
                       (param_5 / DAT_005aa280) * (param_5 / DAT_005aa280)));
  puStack_68 = (undefined4 *)(float)extraout_ST0;
  if (fStack_54 < (float)puStack_68 + fStack_58) {
    local_2c[0] = (float)((uint)local_60 ^ DAT_005aa690) / DAT_005aa280;
    local_2c[1] = (float)((uint)local_64 ^ DAT_005aa690) / DAT_005aa280;
    local_2c[2] = (float)((uint)local_60 ^ DAT_005aa690) / DAT_005aa280;
    local_2c[3] = local_64 / DAT_005aa280;
    local_2c[4] = local_60 / DAT_005aa280;
    fStack_18 = local_64 / DAT_005aa280;
    fStack_14 = local_60 / DAT_005aa280;
    fStack_10 = (float)((uint)local_64 ^ DAT_005aa690) / DAT_005aa280;
    if (param_1 != 0.0) {
      FUN_004d5290();
    }
    local_4c[4] = param_4 / DAT_005aa280;
    fStack_38 = param_5 / DAT_005aa280;
    puStack_68 = (undefined4 *)(param_4 / DAT_005aa280);
    local_4c[0] = (float)((uint)param_4 ^ DAT_005aa690) / DAT_005aa280;
    local_4c[1] = (float)((uint)param_5 ^ DAT_005aa690) / DAT_005aa280;
    local_4c[2] = (float)((uint)param_4 ^ DAT_005aa690) / DAT_005aa280;
    local_4c[3] = param_5 / DAT_005aa280;
    fStack_30 = (float)((uint)param_5 ^ DAT_005aa690) / DAT_005aa280;
    fStack_58 = fStack_38;
    fStack_54 = local_4c[4];
    if (param_6 != 0.0) {
      puStack_34 = puStack_68;
      FUN_004d5290();
      puStack_68 = puStack_34;
    }
    local_2c[0] = local_2c[0] + local_5c;
    local_4c[0] = local_4c[0] + param_2;
    local_4c[1] = local_4c[1] + param_3;
    local_4c[4] = local_4c[4] + param_2;
    fStack_38 = fStack_38 + param_3;
    local_2c[1] = local_2c[1] + local_50;
    local_2c[2] = local_2c[2] + local_5c;
    local_4c[2] = local_4c[2] + param_2;
    local_4c[3] = local_4c[3] + param_3;
    fStack_30 = fStack_30 + param_3;
    local_2c[3] = local_2c[3] + local_50;
    local_2c[4] = local_2c[4] + local_5c;
    fStack_18 = fStack_18 + local_50;
    fStack_14 = fStack_14 + local_5c;
    fStack_10 = fStack_10 + local_50;
    puStack_34 = (undefined4 *)((float)puStack_68 + param_2);
    cVar1 = FUN_004d46d0();
    if ((cVar1 != '\0') || (cVar1 = FUN_004d46d0(), cVar1 != '\0')) {
      return 1;
    }
    puStack_68 = &DAT_00596d80;
    do {
      piVar4 = &DAT_00596d80;
      do {
        cVar1 = FUN_004d3ae0(local_4c[*piVar4 * 2],local_4c[*piVar4 * 2 + 1],local_4c[piVar4[1] * 2]
                             ,local_4c[piVar4[1] * 2 + 1]);
        if (cVar1 != '\0') {
          return 1;
        }
        piVar4 = piVar4 + 2;
      } while ((int)piVar4 < 0x596da0);
      puStack_68 = puStack_68 + 2;
    } while ((int)puStack_68 < 0x596da0);
  }
  return 0;
}


// Reference 00417abf
// Reference 0051e6de
// Reference 0041968f
// Reference 004196f8
// Reference 0041cb63
// Reference 0041e1e6
// Reference 0052ab18
// Reference 0052d4f8
