/* Entry: 0x00489c10; symbol: FUN_00489c10; body bytes: 100 */

undefined4 * __cdecl FUN_00489c10(int param_1,int param_2,undefined4 *param_3)

{
  char cVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 *puVar4;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    param_3 = (undefined4 *)FUN_00489830();
  }
  else {
    for (; param_1 != param_2; param_1 = param_1 + 0x2c) {
      puVar2 = (undefined4 *)move<>(param_1);
      puVar4 = param_3;
      for (iVar3 = 0xb; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar4 = *puVar2;
        puVar2 = puVar2 + 1;
        puVar4 = puVar4 + 1;
      }
      param_3 = param_3 + 0xb;
    }
  }
  return param_3;
}

