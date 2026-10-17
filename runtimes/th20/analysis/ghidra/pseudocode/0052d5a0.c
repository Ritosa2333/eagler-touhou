/* Entry: 0x0052d5a0; symbol: FUN_0052d5a0; body bytes: 30 */

void __cdecl FUN_0052d5a0(undefined4 *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)move<>(param_2);
  uVar1 = puVar2[1];
  *param_1 = *puVar2;
  param_1[1] = uVar1;
  return;
}

