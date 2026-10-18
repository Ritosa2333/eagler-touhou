/* Entry: 0x004fcc60; symbol: FUN_004fcc60; body bytes: 45 */

void __fastcall FUN_004fcc60(int param_1)

{
  char cVar1;
  
  cVar1 = FUN_004fc420(*(byte *)(param_1 + 0xc));
  if (cVar1 == '\0') {
    func_0x004fcd20("Format specifier requires numeric argument.");
  }
  return;
}

