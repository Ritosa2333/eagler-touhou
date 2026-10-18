/* Entry: 0x005068a0; symbol: FUN_005068a0; body bytes: 78 */

uint __cdecl FUN_005068a0(int param_1,uint param_2,int param_3)

{
  char *local_8;
  
  local_8 = *(char **)(param_1 + 0x5d4 + param_3 * 4);
  while( true ) {
    if (*local_8 < '\0') {
      return (uint)local_8 & 0xffffff00;
    }
    if ((((int)local_8[0x28] & 0xfU) == param_2) && (local_8[0x29] == '\x02')) break;
    local_8 = local_8 + 0x78;
  }
  return 1;
}

