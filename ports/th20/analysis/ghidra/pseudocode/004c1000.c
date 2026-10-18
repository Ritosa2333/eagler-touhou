/* Entry: 0x004c1000; symbol: FUN_004c1000; body bytes: 48 */

bool __fastcall FUN_004c1000(int param_1)

{
  uint uVar1;
  
  uVar1 = FUN_0040c300((undefined4 *)(param_1 + 0x8c));
  return (uVar1 & 0x1000000) != 0;
}

