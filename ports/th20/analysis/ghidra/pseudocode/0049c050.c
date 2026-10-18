/* Entry: 0x0049c050; symbol: FUN_0049c050; body bytes: 109 */

undefined1 __fastcall FUN_0049c050(int param_1)

{
  bool bVar1;
  
  if (((((*(uint *)(param_1 + 0x350) >> 5 & 1) == 0) && ((*(uint *)(param_1 + 0x350) & 1) == 0)) &&
      ((*(uint *)(param_1 + 0x350) >> 4 & 1) == 0)) &&
     (bVar1 = FUN_00461070((void *)(param_1 + 0x310),0), !bVar1)) {
    return 0;
  }
  return 1;
}

