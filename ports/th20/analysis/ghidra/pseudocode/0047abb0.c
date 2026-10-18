/* Entry: 0x0047abb0; symbol: FUN_0047abb0; body bytes: 96 */

undefined4 * __cdecl FUN_0047abb0(undefined4 *param_1,int param_2,undefined4 *param_3)

{
  char cVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    param_3 = (undefined4 *)FUN_0047ab60(param_1,param_2);
  }
  else {
    for (; param_2 != 0; param_2 = param_2 + -1) {
      puVar3 = param_1;
      puVar4 = param_3;
      for (iVar2 = 0xb; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar4 = *puVar3;
        puVar3 = puVar3 + 1;
        puVar4 = puVar4 + 1;
      }
      param_3 = param_3 + 0xb;
      param_1 = param_1 + 0xb;
    }
  }
  return param_3;
}

