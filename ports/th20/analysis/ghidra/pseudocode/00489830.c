/* Entry: 0x00489830; symbol: FUN_00489830; body bytes: 113 */

void FUN_00489830(void)

{
  uint *puVar1;
  int iVar2;
  undefined4 *puVar3;
  uint uVar4;
  
  puVar1 = (uint *)FUN_00425620((undefined4 *)&stack0x00000004);
  iVar2 = FUN_00425620((undefined4 *)&stack0x00000008);
  uVar4 = iVar2 - (int)puVar1;
  puVar3 = (undefined4 *)move<>(&stack0x0000000c);
  FUN_00452450(puVar1,*puVar3,uVar4);
  return;
}

