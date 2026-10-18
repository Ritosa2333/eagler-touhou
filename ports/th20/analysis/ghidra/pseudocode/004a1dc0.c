/* Entry: 0x004a1dc0; symbol: FUN_004a1dc0; body bytes: 70 */

void __cdecl FUN_004a1dc0(void *param_1,undefined4 *param_2)

{
  char **ppcVar1;
  char **local_8;
  
  *(undefined4 *)param_2[1] = 0;
  local_8 = (char **)*param_2;
  while (local_8 != (char **)0x0) {
    ppcVar1 = (char **)*local_8;
    FUN_004a1e80(param_1,local_8);
    local_8 = ppcVar1;
  }
  return;
}

