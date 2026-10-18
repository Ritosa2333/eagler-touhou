/* Entry: 0x004199a0; symbol: FUN_004199a0; body bytes: 164 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004199a0(void)

{
  int *piVar1;
  int iVar2;
  
  FUN_00419760();
  piVar1 = (int *)FUN_00412730(0x5c4d40);
  iVar2 = (**(code **)(*piVar1 + 0x44))(piVar1,0,0,0,0);
  if (iVar2 < 0) {
    FUN_004dd840(&DAT_005c4d40);
    FUN_0041dbe0(DAT_005c0028);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x40))(piVar1,&DAT_005c4e24);
    FUN_0041d9f0(DAT_005c0028);
    FUN_004dbd70(0x5c4d40);
    FUN_0041a2c0();
    _DAT_005c586c = 2;
  }
  FUN_004193c0();
  return;
}

