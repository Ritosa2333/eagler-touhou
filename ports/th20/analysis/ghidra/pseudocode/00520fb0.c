/* Entry: 0x00520fb0; symbol: FUN_00520fb0; body bytes: 175 */

void __fastcall FUN_00520fb0(void *param_1)

{
  int local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422dd0(local_14,DAT_00571054,DAT_0056fa30,0);
  if (*(int *)((int)param_1 + 0x20) == 0) {
    FUN_004e1080(local_14);
    *(undefined4 *)((int)param_1 + 0x20) = 1;
    FUN_00423520((void *)((int)param_1 + 0x154),0);
  }
  else if ((*(int *)((int)param_1 + 0x20) == 1) && (DAT_005c60b8 == 0)) {
    FUN_0052ced0(param_1,1);
    FUN_0052cbf0(param_1,0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

