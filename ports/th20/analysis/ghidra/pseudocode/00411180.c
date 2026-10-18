/* Entry: 0x00411180; symbol: FUN_00411180; body bytes: 56 */

int __cdecl FUN_00411180(char *param_1)

{
  char cVar1;
  char *local_c;
  
  local_c = param_1;
  do {
    cVar1 = *local_c;
    local_c = local_c + 1;
  } while (cVar1 != '\0');
  return (int)local_c - (int)(param_1 + 1);
}

