/* Entry: 0x0040b4f0; symbol: FUN_0040b4f0; body bytes: 46 */

undefined8 __cdecl FUN_0040b4f0(undefined8 *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  undefined8 uVar2;
  
  uVar2 = *param_1;
  uVar1 = param_2[1];
  *(undefined4 *)param_1 = *param_2;
  *(undefined4 *)((int)param_1 + 4) = uVar1;
  return uVar2;
}

