/* Entry: 0x004fb8c0; symbol: FUN_004fb8c0; body bytes: 62 */

void __cdecl FUN_004fb8c0(char *param_1,char *param_2)

{
  for (; param_1 != param_2; param_1 = param_1 + 1) {
    if (('`' < *param_1) && (*param_1 < '{')) {
      *param_1 = *param_1 + -0x20;
    }
  }
  return;
}

