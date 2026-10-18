/* Entry: 0x005494cc; symbol: FUN_005494cc; body bytes: 106 */

void __cdecl FUN_005494cc(char *param_1,int *param_2)

{
  char cVar1;
  char *pcVar2;
  char *pcVar3;
  int iVar4;
  
  cVar1 = *param_1;
  while ((cVar1 != '\0' && (cVar1 != *(char *)**(undefined4 **)(*param_2 + 0x88)))) {
    param_1 = param_1 + 1;
    cVar1 = *param_1;
  }
  if (cVar1 != '\0') {
    do {
      param_1 = param_1 + 1;
      cVar1 = *param_1;
      pcVar2 = param_1;
      if ((cVar1 == '\0') || (cVar1 == 'e')) break;
    } while (cVar1 != 'E');
    do {
      pcVar3 = pcVar2;
      pcVar2 = pcVar3 + -1;
    } while (*pcVar2 == '0');
    pcVar3 = pcVar3 + -2;
    if (*pcVar2 != *(char *)**(undefined4 **)(*param_2 + 0x88)) {
      pcVar3 = pcVar2;
    }
    iVar4 = (int)param_1 - (int)pcVar3;
    do {
      cVar1 = pcVar3[iVar4];
      pcVar3 = pcVar3 + 1;
      *pcVar3 = cVar1;
    } while (cVar1 != '\0');
  }
  return;
}

