/* Entry: 0x004fb940; symbol: FUN_004fb940; body bytes: 54 */

void __fastcall FUN_004fb940(int param_1)

{
  char cVar1;
  
  cVar1 = FUN_004fc450(*(byte *)(param_1 + 0xc));
  if ((cVar1 != '\0') || (*(char *)(param_1 + 0xc) == '\n')) {
    func_0x004fcd20("Precision not allowed for this argument type.");
  }
  return;
}

