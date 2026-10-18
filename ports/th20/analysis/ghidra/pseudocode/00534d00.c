/* Entry: 0x00534d00; symbol: FUN_00534d00; body bytes: 201 */

void FUN_00534d00(void)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  undefined4 *puVar4;
  undefined4 local_18;
  undefined4 local_14;
  float *local_10;
  void *local_c;
  int local_8;
  
  iVar2 = FUN_00460830(0);
  pfVar3 = (float *)FUN_00460850(iVar2);
  FUN_00426eb0(&DAT_005ba830,0x1c,*pfVar3);
  FUN_005348d0(local_8);
  local_c = (void *)(local_8 + 0x20);
  bVar1 = FUN_0047c650(local_c,0);
  if (bVar1) {
    FUN_00479040((void *)(local_8 + 0x20));
  }
  local_14 = *(undefined4 *)(local_8 + 0x24);
  iVar2 = FUN_00460830(0);
  local_10 = (float *)FUN_00460850(iVar2);
  puVar4 = FUN_004790e0(&local_18,(uint *)&DAT_005762c4,0x1d,local_10,0,-1,(undefined4 *)0x0);
  *(undefined4 *)(local_8 + 0x20) = *puVar4;
  (**(code **)(**(int **)(local_8 + 0x28) + 0x50))();
  *(undefined4 *)(local_8 + 0x54) = 1;
  return;
}

