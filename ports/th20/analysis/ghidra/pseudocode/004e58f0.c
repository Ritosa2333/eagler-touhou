/* Entry: 0x004e58f0; symbol: FUN_004e58f0; body bytes: 135 */

void __fastcall FUN_004e58f0(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  
  FUN_004bd6e0(0x5ba568);
  *(uint *)(DAT_005ba828 + 0xe8) = *(uint *)(DAT_005ba828 + 0xe8) & 0xffffffef;
  FUN_004292a0(&DAT_005aefe4,*(undefined4 *)(param_1 + 0x1c4));
  bVar1 = FUN_00478160(DAT_005c06a4);
  if (CONCAT31(extraout_var,bVar1) != 0) {
    iVar2 = FUN_004b8070(DAT_005c06a4);
    FUN_004b8880(iVar2);
  }
  FUN_004b87f0();
  DAT_005b8858 = *(undefined4 *)(param_1 + 0xec);
  return;
}

