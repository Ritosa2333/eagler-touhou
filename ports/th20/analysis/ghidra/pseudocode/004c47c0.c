/* Entry: 0x004c47c0; symbol: FUN_004c47c0; body bytes: 307 */

void __fastcall FUN_004c47c0(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  void *pvVar4;
  undefined3 extraout_var;
  uint local_8;
  
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_004993b0(iVar2);
  iVar3 = FUN_00464080(0);
  iVar3 = FUN_004b8210(iVar3);
  if (iVar3 <= iVar2) {
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_004c4b30(iVar2);
    local_8 = iVar2 - iVar2 % 10;
    if ((int)local_8 < 1) {
      local_8 = 10;
    }
    pvVar4 = (void *)FUN_004bd5a0(0);
    FUN_00510710(pvVar4,(undefined4 *)(param_1 + 0xbe4),local_8,0xff40ff40);
    FUN_00488550(&DAT_005ba568,local_8);
  }
  pvVar4 = (void *)FUN_00464080(0);
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_004b8210(iVar2);
  bVar1 = FUN_004e1410(pvVar4,iVar2);
  if (CONCAT31(extraout_var,bVar1) != 0) {
    pvVar4 = (void *)FUN_00460830(0);
    FUN_004faca0(pvVar4,-1);
    pvVar4 = (void *)FUN_004bd5a0(0);
    FUN_00510710(pvVar4,(undefined4 *)(param_1 + 0xbe4),-1,0xffffff40);
    FUN_00426eb0(&DAT_005ba830,0xd,*(float *)(param_1 + 0xbe4));
  }
  return;
}

