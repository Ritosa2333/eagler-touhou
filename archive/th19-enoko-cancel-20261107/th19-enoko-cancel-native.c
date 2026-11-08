// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 54aae0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0054aae0(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  float10 fVar5;
  float10 fVar6;
  float fVar7;
  float fVar8;
  undefined8 uVar9;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  
  uVar4 = *(uint *)(param_1 + 0xd8) & 0xff;
  iVar1 = *(int *)(param_1 + 0x1c);
  iVar2 = *(int *)(*(int *)(*(int *)(*(int *)(param_1 + 0x124) + 4) + 0x2238) + 0xf0 +
                  ((int)*(uint *)(param_1 + 0xd8) >> 8) * 4);
  iVar3 = *(int *)(iVar2 + 0x40 + uVar4 * 0x78);
  if (iVar1 == iVar3) {
    *(uint *)(param_1 + 0x98) = *(uint *)(param_1 + 0x98) | 0x20;
    *(undefined4 *)(param_1 + 0x6c) = 0;
    FUN_004cb830(*(undefined4 *)(param_1 + 0x14),2);
    fVar7 = DAT_005aa390;
    if (*(int *)(param_1 + 0x11c) == 0) {
      fVar7 = DAT_005aa520;
    }
    FUN_004bb8d0(0x1b,(int)((fVar7 * _DAT_005aa478) / DAT_005aa3dc));
  }
  else if ((iVar3 < iVar1) && (iVar1 <= iVar3 + 10)) {
    fVar7 = *(float *)(param_1 + 0x70);
    fVar5 = (float10)FUN_0047a070((double)fVar7);
    fVar6 = (float10)FUN_0047a0c0((double)fVar7);
    fVar7 = (float)fVar5 * DAT_005aa2d4;
    fVar8 = (float)fVar6 * DAT_005aa2d4;
    *(float *)(param_1 + 0x5c) = *(float *)(param_1 + 0x5c) + 0.0;
    *(float *)(param_1 + 0x54) = (fVar8 - (float)fVar5 * 0.0) + *(float *)(param_1 + 0x54);
    *(float *)(param_1 + 0x58) = (float)fVar6 * 0.0 + fVar7 + *(float *)(param_1 + 0x58);
  }
  iVar1 = *(int *)(iVar2 + 0x40 + uVar4 * 0x78);
  if ((iVar1 + 10 <= *(int *)(param_1 + 0x1c)) && (*(int *)(param_1 + 0x1c) < iVar1 + 0x14)) {
    uVar9 = 0;
    local_4c = 0x41c00000;
    local_48 = 0x42600000;
    local_44 = 0;
    FUN_004ee0d0(param_1 + 0x54,&local_4c,*(undefined4 *)(param_1 + 0x70),0,0);
    FUN_005303f0(param_1 + 0x54,&local_4c,uVar9);
  }
  return 0;
}


// Reference 005a0434
// Address 54aca0

undefined4 __fastcall FUN_0054aca0(int param_1)

{
  *(undefined1 *)(param_1 + 0x114) = 1;
  return 0;
}


// Reference 005a0494
// Address 54acb0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0054acb0(int param_1)

{
  int iVar1;
  float fVar2;
  
  iVar1 = FUN_00402540();
  fVar2 = (DAT_005aa2bc / _DAT_005aa310) *
          ((float)((double)iVar1 + (double)(&DAT_005aa660)[-(iVar1 >> 0x1f)]) /
           ((float)((double)DAT_005c6630 + (double)(&DAT_005aa660)[-(DAT_005c6630 >> 0x1f)]) /
            DAT_005aa280 - DAT_005aa230) - DAT_005aa230) + *(float *)(param_1 + 0x70);
  if (fVar2 <= DAT_005aa2bc) {
    if (fVar2 < DAT_005aa4d4) {
      iVar1 = 0;
      do {
        fVar2 = fVar2 + DAT_005aa2bc * DAT_005aa280;
        if (0x20 < iVar1) break;
        iVar1 = iVar1 + 1;
      } while (fVar2 < DAT_005aa4d4);
    }
  }
  else {
    iVar1 = 0;
    while( true ) {
      fVar2 = fVar2 - DAT_005aa2bc * DAT_005aa280;
      if (0x20 < iVar1) break;
      iVar1 = iVar1 + 1;
      if (fVar2 <= DAT_005aa2bc) {
        *(float *)(param_1 + 0x70) = fVar2;
        return 0;
      }
    }
  }
  *(float *)(param_1 + 0x70) = fVar2;
  return 0;
}


// Reference 005a0490
