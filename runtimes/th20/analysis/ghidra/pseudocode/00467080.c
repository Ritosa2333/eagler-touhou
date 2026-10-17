/* Entry: 0x00467080; symbol: FUN_00467080; body bytes: 95 */

undefined4 * __cdecl FUN_00467080(int param_1,int param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  char cVar2;
  undefined4 *puVar3;
  ulonglong uVar4;
  
  cVar2 = FUN_00411170();
  if (cVar2 == '\0') {
    uVar4 = FUN_00467000();
    param_3 = (undefined4 *)uVar4;
  }
  else {
    while (param_1 != param_2) {
      param_2 = param_2 + -8;
      puVar3 = (undefined4 *)move<>(param_2);
      uVar1 = puVar3[1];
      param_3[-2] = *puVar3;
      param_3[-1] = uVar1;
      param_3 = param_3 + -2;
    }
  }
  return param_3;
}

