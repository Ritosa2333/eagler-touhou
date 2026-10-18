/* Entry: 0x0044b4e0; symbol: FUN_0044b4e0; body bytes: 80 */

int __cdecl FUN_0044b4e0(uint *param_1,int param_2)

{
  char cVar1;
  int iVar2;
  
  iVar2 = FUN_0041b8e0(param_1,param_2);
  while( true ) {
    if (iVar2 == param_2) {
      return param_2;
    }
    cVar1 = FUN_00419150(*(short *)(param_2 + -2));
    if (cVar1 != '\0') break;
    param_2 = param_2 + -2;
  }
  return param_2;
}

