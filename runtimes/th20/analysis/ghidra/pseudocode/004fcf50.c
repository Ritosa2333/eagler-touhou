/* Entry: 0x004fcf50; symbol: FUN_004fcf50; body bytes: 73 */

void __fastcall FUN_004fcf50(void *param_1)

{
  undefined4 uVar1;
  
  if (*(uint *)((int)param_1 + 0x2c) < *(uint *)((int)param_1 + 0x30)) {
    uVar1 = FUN_004fcec0(param_1,*(byte **)((int)param_1 + 0x2c),*(byte **)((int)param_1 + 0x30));
    *(undefined4 *)((int)param_1 + 0x34) = uVar1;
  }
  else {
    *(undefined4 *)((int)param_1 + 0x34) = 0xffffffff;
  }
  return;
}

