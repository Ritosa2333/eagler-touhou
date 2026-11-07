// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 54da60

void __fastcall FUN_0054da60(int param_1)

{
  byte bVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  float fVar4;
  float local_8;

  local_8 = (DAT_005aa2bc / DAT_005aa3d4) * DAT_005aa314;
  iVar2 = FUN_00402540();
  fVar4 = ((float)((double)iVar2 + (double)(&DAT_005aa660)[-(iVar2 >> 0x1f)]) /
           ((float)((double)DAT_005c6630 + (double)(&DAT_005aa660)[-(DAT_005c6630 >> 0x1f)]) /
            DAT_005aa280 - DAT_005aa230) - DAT_005aa230) * local_8 + *(float *)(param_1 + 0x70);
  if (fVar4 <= DAT_005aa2bc) {
    iVar2 = extraout_ECX;
    if (fVar4 < DAT_005aa4d4) {
      iVar3 = 0;
      do {
        iVar2 = iVar3 + 1;
        fVar4 = fVar4 + DAT_005aa2bc * DAT_005aa280;
        if (0x20 < iVar3) break;
        iVar3 = iVar2;
      } while (fVar4 < DAT_005aa4d4);
    }
  }
  else {
    iVar3 = 0;
    do {
      fVar4 = fVar4 - DAT_005aa2bc * DAT_005aa280;
      iVar2 = iVar3 + 1;
      if (0x20 < iVar3) break;
      iVar3 = iVar2;
    } while (DAT_005aa2bc < fVar4);
  }
  fVar4 = fVar4 + DAT_005aa2bc;
  if (fVar4 <= DAT_005aa2bc) {
    if (fVar4 < DAT_005aa4d4) {
      iVar3 = 0;
      do {
        iVar2 = iVar3 + 1;
        fVar4 = fVar4 + DAT_005aa2bc * DAT_005aa280;
        if (0x20 < iVar3) break;
        iVar3 = iVar2;
      } while (fVar4 < DAT_005aa4d4);
    }
  }
  else {
    iVar3 = 0;
    do {
      fVar4 = fVar4 - DAT_005aa2bc * DAT_005aa280;
      iVar2 = iVar3 + 1;
      if (0x20 < iVar3) break;
      iVar3 = iVar2;
    } while (DAT_005aa2bc < fVar4);
  }
  FUN_004cb960(&local_8,iVar2,0x8e,param_1 + 0x54,fVar4,0xffffffff,0,0);
  iVar2 = FUN_004cb8b0(local_8);
  bVar1 = FUN_00402540();
  *(byte *)(iVar2 + 0x4ae) = (bVar1 & 0x7f) + 0x7f;
  bVar1 = FUN_00402540();
  *(byte *)(iVar2 + 0x4ad) = (bVar1 & 0x3f) + 0x40;
  bVar1 = FUN_00402540();
  *(byte *)(iVar2 + 0x4ac) = (bVar1 & 0x3f) + 0x40;
  bVar1 = FUN_00402540();
  *(byte *)(iVar2 + 0x4af) = (bVar1 & 0x3f) + 0x60;
  FUN_0054fba0();
  return;
}


// Reference 005a03d4
// Address 54bf50

void __fastcall FUN_0054bf50(int param_1)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  float fVar3;
  float local_8;

  local_8 = (DAT_005aa2bc / DAT_005aa3d4) * DAT_005aa314;
  iVar1 = FUN_00402540();
  fVar3 = ((float)((double)iVar1 + (double)(&DAT_005aa660)[-(iVar1 >> 0x1f)]) /
           ((float)((double)DAT_005c6648 + (double)(&DAT_005aa660)[-(DAT_005c6648 >> 0x1f)]) /
            DAT_005aa280 - DAT_005aa230) - DAT_005aa230) * local_8 + *(float *)(param_1 + 0x70);
  if (fVar3 <= DAT_005aa2bc) {
    iVar1 = extraout_ECX;
    if (fVar3 < DAT_005aa4d4) {
      local_8 = DAT_005aa2bc * DAT_005aa280;
      iVar2 = 0;
      do {
        fVar3 = fVar3 + local_8;
        iVar1 = iVar2 + 1;
        if (0x20 < iVar2) break;
        iVar2 = iVar1;
      } while (fVar3 < DAT_005aa4d4);
    }
  }
  else {
    iVar2 = 0;
    do {
      fVar3 = fVar3 - DAT_005aa2bc * DAT_005aa280;
      iVar1 = iVar2 + 1;
      if (0x20 < iVar2) break;
      iVar2 = iVar1;
    } while (DAT_005aa2bc < fVar3);
  }
  FUN_004cb960(&local_8,iVar1,0x8e,param_1 + 0x54,fVar3,0xffffffff,0,0);
  FUN_0054fba0();
  return;
}


// Reference 005a03e4
// Address 54bd90

void __fastcall FUN_0054bd90(int param_1)

{
  byte bVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  float fVar4;
  float local_18 [5];

  local_18[0] = (DAT_005aa2bc / DAT_005aa3d4) * DAT_005aa314;
  iVar2 = FUN_00402540();
  fVar4 = ((float)((double)iVar2 + (double)(&DAT_005aa660)[-(iVar2 >> 0x1f)]) /
           ((float)((double)DAT_005c6648 + (double)(&DAT_005aa660)[-(DAT_005c6648 >> 0x1f)]) /
            DAT_005aa280 - DAT_005aa230) - DAT_005aa230) * local_18[0] + *(float *)(param_1 + 0x70);
  if (fVar4 <= DAT_005aa2bc) {
    iVar2 = extraout_ECX;
    if (fVar4 < DAT_005aa4d4) {
      local_18[0] = DAT_005aa2bc * DAT_005aa280;
      iVar3 = 0;
      do {
        fVar4 = fVar4 + local_18[0];
        iVar2 = iVar3 + 1;
        if (0x20 < iVar3) break;
        iVar3 = iVar2;
      } while (fVar4 < DAT_005aa4d4);
    }
  }
  else {
    iVar3 = 0;
    do {
      fVar4 = fVar4 - DAT_005aa2bc * DAT_005aa280;
      iVar2 = iVar3 + 1;
      if (0x20 < iVar3) break;
      iVar3 = iVar2;
    } while (DAT_005aa2bc < fVar4);
  }
  FUN_004cb960(local_18,iVar2,0x8e,param_1 + 0x54,fVar4,0xffffffff,0,0);
  iVar2 = FUN_004cb8b0(local_18[0]);
  local_18[0] = 3.0;
  local_18[1] = 3.0;
  local_18[2] = 1.0;
  local_18[3] = 1.0;
  FUN_004044b0(0x14,0,local_18 + 2,local_18);
  bVar1 = FUN_00402540();
  *(byte *)(iVar2 + 0x4ae) = (bVar1 & 0x7f) + 0x7f;
  bVar1 = FUN_00402540();
  *(byte *)(iVar2 + 0x4ad) = (bVar1 & 0x7f) + 0x40;
  bVar1 = FUN_00402540();
  *(byte *)(iVar2 + 0x4ac) = (bVar1 & 0x3f) + 0x40;
  FUN_0054fba0();
  return;
}


// Reference 005a03e8
// Reference 005a03ec
// Address 54c7c0

undefined4 __fastcall FUN_0054c7c0(int param_1)

{
  int iVar1;

  *(undefined4 *)(param_1 + 0xa4) = 1;
  if (*(int *)(param_1 + 0xa8) == 0) {
    FUN_004cb830(*(undefined4 *)(param_1 + 0x14),2);
    *(undefined4 *)(param_1 + 0xa8) = 1;
  }
  if (*(int *)(param_1 + 0x1c) != *(int *)(param_1 + 0x18)) {
    iVar1 = FUN_00520240(*(undefined4 *)(param_1 + 0xdc));
    if (iVar1 != 0) {
      return *(undefined4 *)(iVar1 + 0x98);
    }
  }
  return 0;
}


// Reference 005a03f0
// Address 54a7b0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0054a7b0(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  float fVar4;
  float fVar5;
  undefined4 *puVar6;
  int iVar7;
  int iVar8;
  float fVar9;
  int local_8;

  local_8 = param_1;
  puVar6 = (undefined4 *)
           FUN_005200f0(&local_8,param_1 + 0x54,0x3f800000,0x10,
                        (int)*(short *)(*(int *)(*(int *)(*(int *)(*(int *)(param_1 + 0x124) + 4) +
                                                         0x2238) + 0xf0 +
                                                ((int)*(uint *)(param_1 + 0xd8) >> 8) * 4) + 2 +
                                       (*(uint *)(param_1 + 0xd8) & 0xff) * 0x78));
  iVar7 = FUN_00520240(*puVar6);
  fVar5 = DAT_005aa4d4;
  fVar4 = DAT_005aa280;
  if (iVar7 != 0) {
    fVar9 = DAT_005aa4d4 / DAT_005aa280;
    *(undefined4 *)(iVar7 + 0xa4) = 2;
    *(undefined4 *)(iVar7 + 0x4c) = 0x3e99999a;
    if (fVar9 <= DAT_005aa2bc) {
      if (fVar9 < fVar5) {
        iVar8 = 0;
        do {
          fVar9 = fVar9 + DAT_005aa2bc * fVar4;
          if (0x20 < iVar8) break;
          iVar8 = iVar8 + 1;
        } while (fVar9 < fVar5);
      }
    }
    else {
      iVar8 = 0;
      do {
        fVar9 = fVar9 - DAT_005aa2bc * fVar4;
        if (0x20 < iVar8) break;
        iVar8 = iVar8 + 1;
      } while (DAT_005aa2bc < fVar9);
    }
    *(float *)(iVar7 + 0x50) = fVar9;
    FUN_004cb830(*(undefined4 *)(param_1 + 0x14),1);
    *(undefined4 *)(param_1 + 0x9c) = 2;
    iVar7 = FUN_00520240(*(undefined4 *)(param_1 + 0xdc));
    if (iVar7 != 0) {
      FUN_00520320();
      *(undefined4 *)(param_1 + 0x6c) = 0x3e99999a;
      uVar1 = *(undefined4 *)(param_1 + 0x58);
      uVar2 = *(undefined4 *)(param_1 + 0x5c);
      uVar3 = *(undefined4 *)(param_1 + 0x60);
      *(undefined4 *)(param_1 + 0xdc) = 0;
      *(undefined4 *)(iVar7 + 0x34) = *(undefined4 *)(param_1 + 0x54);
      *(undefined4 *)(iVar7 + 0x38) = uVar1;
      *(undefined4 *)(iVar7 + 0x3c) = uVar2;
      *(undefined4 *)(iVar7 + 0x40) = uVar3;
      uVar1 = *(undefined4 *)(param_1 + 0x68);
      uVar2 = *(undefined4 *)(param_1 + 0x6c);
      uVar3 = *(undefined4 *)(param_1 + 0x70);
      *(undefined4 *)(iVar7 + 0x44) = *(undefined4 *)(param_1 + 100);
      *(undefined4 *)(iVar7 + 0x48) = uVar1;
      *(undefined4 *)(iVar7 + 0x4c) = uVar2;
      *(undefined4 *)(iVar7 + 0x50) = uVar3;
      uVar1 = *(undefined4 *)(param_1 + 0x78);
      uVar2 = *(undefined4 *)(param_1 + 0x7c);
      uVar3 = *(undefined4 *)(param_1 + 0x80);
      *(undefined4 *)(iVar7 + 0x54) = *(undefined4 *)(param_1 + 0x74);
      *(undefined4 *)(iVar7 + 0x58) = uVar1;
      *(undefined4 *)(iVar7 + 0x5c) = uVar2;
      *(undefined4 *)(iVar7 + 0x60) = uVar3;
      uVar1 = *(undefined4 *)(param_1 + 0x88);
      uVar2 = *(undefined4 *)(param_1 + 0x8c);
      uVar3 = *(undefined4 *)(param_1 + 0x90);
      *(undefined4 *)(iVar7 + 100) = *(undefined4 *)(param_1 + 0x84);
      *(undefined4 *)(iVar7 + 0x68) = uVar1;
      *(undefined4 *)(iVar7 + 0x6c) = uVar2;
      *(undefined4 *)(iVar7 + 0x70) = uVar3;
      *(undefined8 *)(iVar7 + 0x74) = *(undefined8 *)(param_1 + 0x94);
      FUN_004bb8d0(0x41,(int)((*(float *)(param_1 + 0x54) * _DAT_005aa478) / DAT_005aa3dc));
      return *(undefined4 *)(param_1 + 0xac);
    }
  }
  return 0;
}


// Reference 005a03f4
