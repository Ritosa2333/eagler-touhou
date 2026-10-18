/* Entry: 0x004135d0; symbol: FUN_004135d0; body bytes: 70 */

void __cdecl FUN_004135d0(void *param_1,undefined4 *param_2)

{
  char **ppcVar1;
  char **local_8;
  
  *(undefined4 *)param_2[1] = 0;
  local_8 = (char **)*param_2;
  while (local_8 != (char **)0x0) {
    ppcVar1 = (char **)*local_8;
    FUN_00413690(param_1,local_8);
    local_8 = ppcVar1;
  }
  return;
}

