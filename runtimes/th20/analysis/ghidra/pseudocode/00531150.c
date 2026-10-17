/* Entry: 0x00531150; symbol: FUN_00531150; body bytes: 137 */

undefined4 FUN_00531150(void *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  float10 fVar4;
  float fVar5;
  undefined4 uVar6;
  
  iVar3 = *(int *)((int)param_1 + 0x5c8);
  iVar1 = FUN_00460830(0);
  puVar2 = (undefined4 *)FUN_00460850(iVar1);
  FUN_0045dd60(param_1,puVar2);
  uVar6 = DAT_0056c8cc;
  iVar3 = FUN_004a0da0(iVar3);
  fVar4 = FUN_00423bd0(iVar3);
  fVar5 = (float)fVar4 * DAT_0056fb80;
  iVar3 = FUN_00534260();
  FUN_00470e50(param_1,fVar5 / (float)iVar3,uVar6);
  return 0;
}

