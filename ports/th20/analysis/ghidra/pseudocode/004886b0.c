/* Entry: 0x004886b0; symbol: FUN_004886b0; body bytes: 70 */

int __cdecl FUN_004886b0(int param_1)

{
  int local_c;
  int local_8;
  
  local_c = 0;
  for (local_8 = 0; local_8 < 0x71; local_8 = local_8 + 1) {
    if (*(int *)(&DAT_005af058 + local_8 * 4) == param_1) {
      local_c = local_c + 1;
    }
  }
  return local_c;
}

