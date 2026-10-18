/* Entry: 0x004c4b90; symbol: FUN_004c4b90; body bytes: 523 */

void __fastcall FUN_004c4b90(int param_1)

{
  float fVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  void *pvVar6;
  undefined3 extraout_var;
  float10 fVar7;
  uint local_8;
  
  iVar3 = FUN_00460830(0);
  iVar3 = FUN_00460850(iVar3);
  fVar1 = *(float *)(iVar3 + 4);
  iVar3 = FUN_00464080(0);
  iVar3 = FUN_004993b0(iVar3);
  iVar4 = FUN_00464080(0);
  iVar4 = FUN_004b8210(iVar4);
  if (iVar3 < iVar4) {
    local_8 = 100;
    pvVar6 = (void *)FUN_00464080(0);
    bVar2 = FUN_004e1410(pvVar6,1);
    if (CONCAT31(extraout_var,bVar2) != 0) {
      pvVar6 = (void *)FUN_00460830(0);
      FUN_004faca0(pvVar6,-1);
      pvVar6 = (void *)FUN_004bd5a0(0);
      FUN_00510710(pvVar6,(undefined4 *)(param_1 + 0xbe4),-1,0xffffff40);
      FUN_00426eb0(&DAT_005ba830,0xd,*(float *)(param_1 + 0xbe4));
    }
  }
  else {
    iVar3 = FUN_00460830(0);
    fVar7 = FUN_004b8050(iVar3);
    if ((fVar1 <= (float)fVar7) || (*(int *)(param_1 + 0xc24) == 3)) {
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_004c4b30(iVar3);
      local_8 = iVar3 - iVar3 % 10;
      if ((int)local_8 < 1) {
        local_8 = 10;
      }
    }
    else {
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_004c4b30(iVar3);
      iVar4 = FUN_00464080(0);
      iVar4 = FUN_004c4b30(iVar4);
      iVar5 = FUN_00460830(0);
      fVar7 = FUN_004b8050(iVar5);
      iVar3 = (iVar3 * 9) / 10 - (((iVar4 * 9) / 10) * ((int)fVar1 - (int)fVar7)) / 0x1c2;
      local_8 = iVar3 - iVar3 % 10;
      if ((int)local_8 < 1) {
        local_8 = 10;
      }
    }
    pvVar6 = (void *)FUN_004bd5a0(0);
    FUN_00510710(pvVar6,(undefined4 *)(param_1 + 0xbe4),local_8,0xffffffff);
  }
  FUN_00488550(&DAT_005ba568,local_8);
  return;
}

