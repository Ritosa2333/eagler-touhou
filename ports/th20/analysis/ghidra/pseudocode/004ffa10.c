/* Entry: 0x004ffa10; symbol: FUN_004ffa10; body bytes: 555 */

void __thiscall FUN_004ffa10(void *this,int param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  int iVar5;
  float *pfVar6;
  float10 fVar7;
  float fVar8;
  
  fVar7 = FUN_004292e0((float *)&DAT_005aefe4);
  *(float *)((int)this + 0x20c4) = (float)param_1 * (float)fVar7;
  fVar7 = FUN_004292e0((float *)&DAT_005aefe4);
  *(float *)((int)this + 0x20c8) = (float)param_2 * (float)fVar7;
  if (*(int *)((int)this + 0x67c) != 0) {
    *(undefined4 *)((int)this + 0x20d0) = *(undefined4 *)((int)this + 0x20c4);
    *(undefined4 *)((int)this + 0x20d4) = *(undefined4 *)((int)this + 0x20c8);
    *(undefined4 *)((int)this + 0x20d8) = *(undefined4 *)((int)this + 0x20cc);
  }
  *(int *)((int)this + 0x20dc) = (int)*(float *)((int)this + 0x20c4);
  *(int *)((int)this + 0x20e0) = (int)*(float *)((int)this + 0x20c8);
  iVar5 = *(int *)((int)this + 0x20dc);
  piVar1 = (int *)FUN_004ff690((int)this);
  *piVar1 = *piVar1 + iVar5;
  iVar5 = *(int *)((int)this + 0x20e0);
  iVar2 = FUN_004ff690((int)this);
  *(int *)(iVar2 + 4) = *(int *)(iVar2 + 4) + iVar5;
  uVar3 = FUN_004ff870((int)this);
  if (uVar3 == 0) {
    piVar1 = (int *)FUN_004ff690((int)this);
    if (*piVar1 < -0x5c00) {
      puVar4 = (undefined4 *)FUN_004ff690((int)this);
      *puVar4 = 0xffffa400;
    }
    else {
      piVar1 = (int *)FUN_004ff690((int)this);
      if (0x5c00 < *piVar1) {
        puVar4 = (undefined4 *)FUN_004ff690((int)this);
        *puVar4 = 0x5c00;
      }
    }
    iVar5 = FUN_004ff690((int)this);
    if (*(int *)(iVar5 + 4) < 0x1000) {
      iVar5 = FUN_004ff690((int)this);
      *(undefined4 *)(iVar5 + 4) = 0x1000;
    }
    else {
      iVar5 = FUN_004ff690((int)this);
      if ((int)((DAT_0056d7c0 - DAT_0056cd90) * DAT_0056cd98) < *(int *)(iVar5 + 4)) {
        fVar8 = (DAT_0056d7c0 - DAT_0056cd90) * DAT_0056cd98;
        iVar5 = FUN_004ff690((int)this);
        *(int *)(iVar5 + 4) = (int)fVar8;
      }
    }
  }
  piVar1 = (int *)FUN_004ff690((int)this);
  fVar8 = (float)*piVar1 / DAT_0056cd98;
  pfVar6 = (float *)FUN_00460850((int)this);
  *pfVar6 = fVar8;
  iVar5 = FUN_004ff690((int)this);
  fVar8 = (float)*(int *)(iVar5 + 4) / DAT_0056cd98;
  iVar5 = FUN_00460850((int)this);
  *(float *)(iVar5 + 4) = fVar8;
  return;
}

