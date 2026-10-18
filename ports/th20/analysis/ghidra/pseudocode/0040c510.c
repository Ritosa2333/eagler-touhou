/* Entry: 0x0040c510; symbol: FUN_0040c510; body bytes: 62 */

undefined1 __fastcall FUN_0040c510(int *param_1)

{
  uint uVar1;
  
  if ((*param_1 != 0) && (uVar1 = FUN_0040bea0(*param_1), (uVar1 & 0xff) != 0)) {
    return 1;
  }
  return 0;
}

