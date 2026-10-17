/* Entry: 0x00456270; symbol: FUN_00456270; body bytes: 71 */

char __cdecl FUN_00456270(char *param_1,int param_2)

{
  char *local_c;
  char local_5;
  
  local_c = param_1;
  local_5 = '\0';
  while( true ) {
    if (param_2 == 0) break;
    local_5 = local_5 + *local_c;
    local_c = local_c + 1;
    param_2 = param_2 + -1;
  }
  return local_5;
}

