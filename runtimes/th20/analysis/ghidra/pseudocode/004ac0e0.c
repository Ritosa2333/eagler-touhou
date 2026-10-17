/* Entry: 0x004ac0e0; symbol: FUN_004ac0e0; body bytes: 113 */

int FUN_004ac0e0(void)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined8 uVar3;
  undefined1 local_10 [8];
  int local_8;
  
  puVar2 = (undefined4 *)FUN_004ac170(local_10);
  uVar1 = puVar2[1];
  *(undefined4 *)(local_8 + 0xd0) = *puVar2;
  *(undefined4 *)(local_8 + 0xd4) = uVar1;
  uVar3 = FUN_004ac200((COleCurrency *)(local_8 + 0xd0));
  *(undefined8 *)(local_8 + 0xd8) = uVar3;
  FUN_004ac270((tm *)(local_8 + 0xe0),(uint *)(local_8 + 0xd8));
  return local_8 + 0xd0;
}

