/* Entry: 0x004c4da0; symbol: FUN_004c4da0; body bytes: 346 */

void __fastcall FUN_004c4da0(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  void *pvVar4;
  undefined3 extraout_var;
  uint local_c;
  
  iVar2 = FUN_0041caf0(*(int *)(param_1 + 0xc48));
  iVar2 = FUN_004993b0(iVar2);
  iVar3 = FUN_0041caf0(*(int *)(param_1 + 0xc48));
  iVar3 = FUN_004b8210(iVar3);
  if (iVar2 < iVar3) {
    local_c = 100;
    pvVar4 = (void *)FUN_0041caf0(*(int *)(param_1 + 0xc48));
    iVar2 = FUN_0041caf0(*(int *)(param_1 + 0xc48));
    iVar2 = FUN_004b81d0(iVar2);
    bVar1 = FUN_004e1410(pvVar4,iVar2);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      pvVar4 = (void *)FUN_0040ff90(*(int *)(param_1 + 0xc48));
      FUN_004faca0(pvVar4,-1);
      FUN_00426eb0(&DAT_005ba830,0xd,*(float *)(param_1 + 0xbe4));
      pvVar4 = (void *)FUN_00421810(*(int *)(param_1 + 0xc48));
      FUN_00510710(pvVar4,(undefined4 *)(param_1 + 0xbe4),-1,0xffffff40);
    }
  }
  else {
    local_c = 20000;
    FUN_00488550(&DAT_005ba568,20000);
    pvVar4 = (void *)FUN_00421810(*(int *)(param_1 + 0xc48));
    FUN_00510710(pvVar4,(undefined4 *)(param_1 + 0xbe4),20000,0xff808080);
    FUN_00426eb0(&DAT_005ba830,0xd,*(float *)(param_1 + 0xbe4));
  }
  FUN_00488550(&DAT_005ba568,local_c);
  return;
}

