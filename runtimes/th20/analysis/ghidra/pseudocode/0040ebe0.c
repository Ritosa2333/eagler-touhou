/* Entry: 0x0040ebe0; symbol: FUN_0040ebe0; body bytes: 63 */

void __fastcall FUN_0040ebe0(int param_1)

{
  char cVar1;
  undefined4 local_8;
  
  cVar1 = FUN_00411170();
  if (cVar1 != '\0') {
    for (local_8 = 0; local_8 < 8; local_8 = local_8 + 1) {
      *(undefined2 *)(param_1 + local_8 * 2) = 0;
    }
  }
  return;
}

