/* Entry: 0x004f30c0; symbol: FUN_004f30c0; body bytes: 33 */

undefined4 * __cdecl FUN_004f30c0(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  
  uVar1 = param_2[1];
  *param_1 = *param_2;
  param_1[1] = uVar1;
  return param_1;
}

