/* Entry: 0x004283b0; symbol: FUN_004283b0; body bytes: 100 */

char * __cdecl FUN_004283b0(char *param_1,char *param_2,int *param_3,int param_4)

{
  int iVar1;
  
  while( true ) {
    if (param_4 == 0) {
      return (char *)0x0;
    }
    *param_3 = *(int *)(param_1 + 4);
    iVar1 = _strncmp(param_1,param_2,4);
    if (iVar1 == 0) break;
    param_4 = param_4 - (*param_3 + 8);
    param_1 = param_1 + *param_3 + 8;
  }
  return param_1 + 8;
}

