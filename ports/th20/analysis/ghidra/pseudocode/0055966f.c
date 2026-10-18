/* Entry: 0x0055966f; symbol: FUN_0055966f; body bytes: 44 */

char FUN_0055966f(char *param_1)

{
  char cVar1;
  
  if (*param_1 == '\0') {
    cVar1 = '\x01';
  }
  else if (param_1[1] == '\0') {
    cVar1 = '\x02';
  }
  else {
    cVar1 = (param_1[2] != '\0') + '\x03';
  }
  return cVar1;
}

