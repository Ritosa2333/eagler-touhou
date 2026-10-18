/* Entry: 0x00414200; symbol: FUN_00414200; body bytes: 124 */

void * __fastcall FUN_00414200(void *param_1)

{
  _memset(param_1,0,0x124);
  *(undefined4 *)((int)param_1 + 0x100) = 0xffffffff;
  *(undefined4 *)((int)param_1 + 0x104) = 0;
  *(undefined4 *)((int)param_1 + 0x108) = 0;
  *(undefined4 *)((int)param_1 + 0x114) = 0;
  *(undefined4 *)((int)param_1 + 0x11c) = 0;
  *(undefined4 *)((int)param_1 + 0x118) = 0;
  *(undefined4 *)((int)param_1 + 0x120) = 0;
  return param_1;
}

