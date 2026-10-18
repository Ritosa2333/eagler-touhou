/* Entry: 0x00552590; symbol: FUN_00552590; body bytes: 30 */

void __cdecl FUN_00552590(char *param_1,int param_2)

{
  char cVar1;
  int iVar2;
  
  iVar2 = 0;
  cVar1 = *param_1;
  while ((cVar1 != '\0' && (iVar2 != param_2))) {
    iVar2 = iVar2 + 1;
    cVar1 = param_1[iVar2];
  }
  return;
}

