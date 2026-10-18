/* Entry: 0x00532f40; symbol: FUN_00532f40; body bytes: 103 */

undefined4 __fastcall FUN_00532f40(int *param_1)

{
  FUN_004863f0(DAT_005c0028,0x17,'\0');
  param_1[0x15] = 0;
  FUN_0044fcd0(param_1 + 8);
  FUN_004117a0(param_1 + 8,0);
  FUN_004a2800((void *)param_1[0x10]);
  param_1[0x10] = 0;
  (**(code **)(*param_1 + 8))();
  return 0;
}

