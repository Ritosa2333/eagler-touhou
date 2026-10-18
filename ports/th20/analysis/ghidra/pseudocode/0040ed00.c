/* Entry: 0x0040ed00; symbol: FUN_0040ed00; body bytes: 101 */

void __cdecl FUN_0040ed00(uint param_1,uint param_2,uint param_3)

{
  uint local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_c = param_1 | 0xf;
  if ((local_c <= param_3) && (param_2 <= param_3 - (param_2 >> 1))) {
    local_10 = (param_2 >> 1) + param_2;
    _Max_value<>(&local_c,&local_10);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

