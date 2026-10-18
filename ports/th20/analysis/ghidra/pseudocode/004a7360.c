/* Entry: 0x004a7360; symbol: FUN_004a7360; body bytes: 108 */

void __fastcall FUN_004a7360(void *param_1)

{
  _memset(param_1,0,0xa8);
  *(undefined4 *)((int)param_1 + 0xa4) = DAT_0056cd94;
  *(undefined4 *)((int)param_1 + 0xa0) = DAT_0056cd94;
  *(undefined4 *)((int)param_1 + 4) = 0;
  *(undefined4 *)((int)param_1 + 0x8c) = 0;
  FUN_00423520((void *)((int)param_1 + 0x90),0);
  return;
}

