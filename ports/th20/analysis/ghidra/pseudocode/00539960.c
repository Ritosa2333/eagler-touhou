/* Entry: 0x00539960; symbol: FUN_00539960; body bytes: 52 */

int __cdecl FUN_00539960(int param_1)

{
  int local_8;
  
  for (local_8 = *(int *)(&DAT_005c8b3c + param_1 * 0xc);
      *(int *)(&DAT_005c8b40 + local_8 * 0xc) != 0;
      local_8 = *(int *)(&DAT_005c8b40 + local_8 * 0xc)) {
  }
  return local_8;
}

