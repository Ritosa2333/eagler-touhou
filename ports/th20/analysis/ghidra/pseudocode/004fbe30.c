/* Entry: 0x004fbe30; symbol: FUN_004fbe30; body bytes: 127 */

char * __thiscall FUN_004fbe30(void *this,char *param_1,char *param_2,char param_3)

{
  int iVar1;
  
  if ((*(int *)((int)this + 4) == 1) || (*(int *)((int)this + 4) == 4)) {
    param_1 = FUN_004683b0(param_1,param_2,&param_3);
  }
  else {
    for (; (param_1 != param_2 && (*param_1 != param_3)); param_1 = param_1 + iVar1) {
      iVar1 = FUN_004fbdb0(this,param_1,(int)param_2);
      if (iVar1 < 0) {
        func_0x004fcd20("Invalid encoded character in format string.");
      }
    }
  }
  return param_1;
}

