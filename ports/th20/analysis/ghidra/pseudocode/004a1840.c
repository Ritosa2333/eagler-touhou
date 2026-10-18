/* Entry: 0x004a1840; symbol: FUN_004a1840; body bytes: 93 */

void __cdecl FUN_004a1840(undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  void *pvVar5;
  
  puVar1 = (undefined4 *)move<>(param_2);
  uVar2 = FUN_0040b2a0(puVar1);
  iVar3 = move<>(param_2);
  uVar4 = FUN_004a21d0(iVar3);
  pvVar5 = (void *)move<>(param_1);
  FUN_004a2b00(pvVar5,uVar4,uVar2);
  return;
}

