/* Entry: 0x004b7c90; symbol: FUN_004b7c90; body bytes: 197 */

undefined * __cdecl FUN_004b7c90(byte *param_1)

{
  int iVar1;
  byte bVar2;
  int local_10;
  int local_c;
  char local_6;
  byte local_5;
  
  local_10 = 0;
  local_5 = 0x77;
  local_6 = '\a';
  do {
    bVar2 = *param_1 ^ local_5;
    (&DAT_005c4a20)[local_10] = bVar2;
    local_10 = local_10 + 1;
    param_1 = param_1 + 1;
    local_5 = local_5 + local_6;
    local_6 = local_6 + '\x10';
  } while (bVar2 != 0);
  for (local_c = 0; (&DAT_005c4a20)[local_c] != '\0'; local_c = local_c + 1) {
    iVar1 = FUN_004b6460((&DAT_005c4a20)[local_c]);
    if (iVar1 == 0) {
      if ((&DAT_005c4a20)[local_c] == '_') {
        (&DAT_005c4a20)[local_c] = 0x20;
      }
    }
    else {
      local_c = local_c + 1;
    }
  }
  return &DAT_005c4a20;
}

