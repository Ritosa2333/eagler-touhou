/* Entry: 0x00540f3b; symbol: FUN_00540f3b; body bytes: 36 */

void FUN_00540f3b(int param_1,int param_2)

{
  if (param_2 != 0) {
    do {
      if ((&DAT_00576df8)[*(byte *)(param_1 + -1 + param_2)] == '\0') {
        return;
      }
      param_2 = param_2 + -1;
    } while (param_2 != 0);
  }
  return;
}

