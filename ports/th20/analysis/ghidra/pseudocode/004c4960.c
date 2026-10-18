/* Entry: 0x004c4960; symbol: FUN_004c4960; body bytes: 453 */

void __fastcall FUN_004c4960(int param_1)

{
  float fVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  void *pvVar6;
  float10 fVar7;
  undefined4 local_8;
  
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_004c4b30(iVar2);
  iVar3 = FUN_00464080(0);
  iVar3 = FUN_004b82c0(iVar3);
  iVar4 = FUN_00464080(0);
  iVar4 = FUN_004c4b30(iVar4);
  iVar5 = FUN_00464080(0);
  iVar5 = FUN_004b8300(iVar5);
  iVar3 = iVar2 / 2 + (iVar3 * iVar4) / iVar5;
  fVar1 = *(float *)(param_1 + 0xbe8);
  iVar2 = FUN_00460830(0);
  fVar7 = FUN_004b8050(iVar2);
  if ((fVar1 <= (float)fVar7) || (*(int *)(param_1 + 0xc24) == 3)) {
    local_8 = iVar3 - iVar3 % 10;
    if ((int)local_8 < 1) {
      local_8 = 10;
    }
    pvVar6 = (void *)FUN_004bd5a0(0);
    FUN_00510710(pvVar6,(undefined4 *)(param_1 + 0xbe4),local_8,0xffffff00);
  }
  else {
    iVar2 = FUN_00460830(0);
    fVar7 = FUN_004b8050(iVar2);
    iVar2 = (iVar3 * 9) / 10 - (((iVar3 * 9) / 10) * (int)(fVar1 - (float)fVar7)) / 0x1c2;
    local_8 = iVar2 - iVar2 % 10;
    if ((int)local_8 < 1) {
      local_8 = 10;
    }
    pvVar6 = (void *)FUN_004bd5a0(0);
    FUN_00510710(pvVar6,(undefined4 *)(param_1 + 0xbe4),local_8,0xffffffff);
  }
  pvVar6 = (void *)FUN_00464080(0);
  FUN_004e14b0(pvVar6,local_8,(int)local_8 >> 0x1f);
  pvVar6 = (void *)FUN_00464080(0);
  FUN_004c46a0(pvVar6,1);
  return;
}

