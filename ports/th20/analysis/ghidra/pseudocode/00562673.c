/* Entry: 0x00562673; symbol: FUN_00562673; body bytes: 35 */

bool __cdecl FUN_00562673(char *param_1,int param_2)

{
  if (param_2 == 0) {
    return true;
  }
  for (; *param_1 == '0'; param_1 = param_1 + 1) {
  }
  return *param_1 != '\0';
}

