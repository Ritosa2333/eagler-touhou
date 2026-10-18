/* Entry: 0x00534f10; symbol: FUN_00534f10; body bytes: 141 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_00534f10(void *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  float fVar3;
  undefined4 uVar4;
  
  uVar4 = DAT_0056c8cc;
  iVar1 = FUN_004a0da0(*(int *)((int)param_1 + 0x5c8));
  iVar1 = FUN_0040ff90(iVar1);
  fVar3 = (float)iVar1 * DAT_0056fe7c;
  iVar1 = FUN_00534260();
  FUN_00470e50(param_1,fVar3 / (float)iVar1 + _DAT_00576430,uVar4);
  iVar1 = FUN_00460830(0);
  puVar2 = (undefined4 *)FUN_00460850(iVar1);
  FUN_0045dd60(param_1,puVar2);
  return 0;
}

