/* Entry: 0x00557290; symbol: FUN_00557290; body bytes: 67 */

int __cdecl FUN_00557290(ushort *param_1,ushort *param_2,int param_3)

{
  int iVar1;
  
  if (param_3 != 0) {
    iVar1 = param_3 + -1;
    for (; ((iVar1 != 0 && (*param_1 != 0)) && (*param_1 == *param_2)); param_1 = param_1 + 1) {
      param_2 = param_2 + 1;
      iVar1 = iVar1 + -1;
    }
    return (uint)*param_1 - (uint)*param_2;
  }
  return 0;
}

