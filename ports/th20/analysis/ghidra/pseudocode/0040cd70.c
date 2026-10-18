/* Entry: 0x0040cd70; symbol: FUN_0040cd70; body bytes: 115 */

void __cdecl FUN_0040cd70(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    uVar2 = move<>(param_1);
    puVar4 = (undefined4 *)move<>(4,uVar2);
    puVar5 = (undefined4 *)move<>(param_2);
    *puVar4 = *puVar5;
  }
  else {
    uVar2 = move<>(param_2);
    uVar3 = move<>(param_1);
    FUN_0040d7d0(uVar3,uVar2);
  }
  return;
}

