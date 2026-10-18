/* Entry: 0x00515690; symbol: FUN_00515690; body bytes: 61 */

undefined4 * __cdecl FUN_00515690(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_2c8 [176];
  undefined1 local_5;
  
  local_5 = 0;
  puVar1 = FUN_00514750(local_2c8,param_2);
  puVar3 = param_1;
  for (iVar2 = 0xb0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *puVar1;
    puVar1 = puVar1 + 1;
    puVar3 = puVar3 + 1;
  }
  return param_1;
}

