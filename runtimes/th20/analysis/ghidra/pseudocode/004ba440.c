/* Entry: 0x004ba440; symbol: FUN_004ba440; body bytes: 96 */

void __fastcall FUN_004ba440(int param_1)

{
  undefined4 *puVar1;
  int *piVar2;
  undefined4 local_8;
  
  puVar1 = (undefined4 *)FUN_004260b0((void *)(param_1 + 0x1c),0);
  *puVar1 = 0xffffffff;
  for (local_8 = 0; local_8 < 0x5a; local_8 = local_8 + 1) {
    piVar2 = (int *)FUN_004260f0((void *)(param_1 + 0x1994),local_8);
    FUN_00428810(piVar2);
  }
  return;
}

