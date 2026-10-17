/* Entry: 0x0045f050; symbol: FUN_0045f050; body bytes: 79 */

void * __cdecl FUN_0045f050(void *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  
  uVar1 = move<>(param_2);
  puVar2 = (undefined4 *)move<>(uVar1);
  uVar1 = *puVar2;
  uVar3 = move<>(param_3);
  FUN_0045ee50(param_1,uVar3,uVar1);
  return param_1;
}

