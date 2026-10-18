/* Entry: 0x004d9b50; symbol: FUN_004d9b50; body bytes: 104 */

undefined4 FUN_004d9b50(undefined4 param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  undefined1 *puVar3;
  
  uVar1 = FUN_00428fa0(0x5c4d40);
  if (uVar1 != 0) {
    FUN_00428c90(&DAT_005ba830,(char **)0x4,0,0x571b14);
  }
  FUN_00428c90(&DAT_005ba830,(char **)0x2,param_1,0x571b14);
  iVar2 = FUN_004640e0(DAT_005c6108);
  puVar3 = (undefined1 *)FUN_00411c10((void *)(iVar2 + 0x36),param_2);
  *puVar3 = 1;
  return 0;
}

