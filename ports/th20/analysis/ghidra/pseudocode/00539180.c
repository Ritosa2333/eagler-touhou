/* Entry: 0x00539180; symbol: FUN_00539180; body bytes: 103 */

void __cdecl FUN_00539180(int param_1,int param_2)

{
  *(undefined4 *)(&DAT_005c8b38 + param_2 * 0xc) = *(undefined4 *)(&DAT_005c8b38 + param_1 * 0xc);
  if (*(int *)(&DAT_005c8b40 + *(int *)(&DAT_005c8b38 + param_1 * 0xc) * 0xc) == param_1) {
    *(int *)(&DAT_005c8b40 + *(int *)(&DAT_005c8b38 + param_1 * 0xc) * 0xc) = param_2;
  }
  else {
    *(int *)(&DAT_005c8b3c + *(int *)(&DAT_005c8b38 + param_1 * 0xc) * 0xc) = param_2;
  }
  *(undefined4 *)(&DAT_005c8b38 + param_1 * 0xc) = 0;
  return;
}

