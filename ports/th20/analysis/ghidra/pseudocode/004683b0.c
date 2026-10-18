/* Entry: 0x004683b0; symbol: FUN_004683b0; body bytes: 155 */

char * __cdecl FUN_004683b0(char *param_1,char *param_2,char *param_3)

{
  char cVar1;
  char *pcVar2;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    cVar1 = FUN_004682a0(param_3);
    pcVar2 = param_2;
    if (cVar1 != '\0') {
      FUN_00425620(&param_1);
      FUN_00425620(&param_2);
      pcVar2 = (char *)FUN_00468450();
    }
  }
  else {
    for (; (pcVar2 = param_1, param_1 != param_2 && (*param_1 != *param_3)); param_1 = param_1 + 1)
    {
    }
  }
  return pcVar2;
}

