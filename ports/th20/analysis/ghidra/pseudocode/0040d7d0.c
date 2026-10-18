/* Entry: 0x0040d7d0; symbol: FUN_0040d7d0; body bytes: 47 */

undefined4 * __cdecl FUN_0040d7d0(undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  puVar1 = (undefined4 *)move<>(4,param_1);
  puVar2 = (undefined4 *)move<>(param_2);
  *puVar1 = *puVar2;
  return puVar1;
}

