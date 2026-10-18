/* Entry: 0x004fcc90; symbol: FUN_004fcc90; body bytes: 54 */

void __fastcall FUN_004fcc90(int param_1)

{
  char cVar1;
  
  cVar1 = FUN_004fc420(*(byte *)(param_1 + 0xc));
  if ((cVar1 == '\0') && (*(char *)(param_1 + 0xc) != '\n')) {
    func_0x004fcd20("Format specifier requires numeric or pointer argument.");
  }
  return;
}

