/* Entry: 0x0046b9a0; symbol: FUN_0046b9a0; body bytes: 117 */

undefined4 __fastcall FUN_0046b9a0(void *param_1)

{
  FUN_0041dce0(&DAT_005c4d40,0);
  FUN_004455c0(DAT_005c0028);
  *(byte *)((int)param_1 + 0x4b3) = *(byte *)((int)param_1 + 0x4b3) & 0xfc | 2;
  FUN_0046ba20(param_1,1);
  *(byte *)((int)param_1 + 0x4b3) = *(byte *)((int)param_1 + 0x4b3) & 0xfc;
  FUN_004455c0(DAT_005c0028);
  FUN_0041dce0(&DAT_005c4d40,2);
  return 1;
}

