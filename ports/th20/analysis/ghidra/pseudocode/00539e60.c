/* Entry: 0x00539e60; symbol: FUN_00539e60; body bytes: 97 */

char * FUN_00539e60(char *param_1)

{
  size_t sVar1;
  char *pcVar2;
  
  sVar1 = _strlen(param_1);
  pcVar2 = (char *)FUN_0041f610(sVar1 + 1);
  if (pcVar2 != (char *)0x0) {
    sVar1 = _strlen(param_1);
    FUN_00548610(pcVar2,sVar1 + 1,(int)param_1);
  }
  return pcVar2;
}

