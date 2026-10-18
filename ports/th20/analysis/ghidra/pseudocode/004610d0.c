/* Entry: 0x004610d0; symbol: FUN_004610d0; body bytes: 111 */

undefined4 __fastcall FUN_004610d0(int param_1)

{
  bool bVar1;
  
  bVar1 = FUN_00461070((void *)(param_1 + 0x38),0);
  if (bVar1) {
    FUN_00429420((void *)(param_1 + 0x38));
    bVar1 = FUN_00423590((void *)(param_1 + 0x38),0);
    if (bVar1) {
      return 0xffffffff;
    }
  }
  FUN_00423540((undefined4 *)(param_1 + 0x28));
  return 0;
}

