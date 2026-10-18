/* Entry: 0x004c5090; symbol: FUN_004c5090; body bytes: 46 */

void __cdecl FUN_004c5090(undefined4 param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  uVar1 = move<>(param_1);
  puVar2 = (undefined4 *)move<>(4,uVar1);
  *puVar2 = *(undefined4 *)*param_2;
  return;
}

