/* Entry: 0x004fe790; symbol: FUN_004fe790; body bytes: 75 */

int __cdecl FUN_004fe790(uint param_1)

{
  undefined4 local_8;
  
  local_8 = 0;
  while( true ) {
    if (param_1 % 5 != 0) break;
    local_8 = local_8 + 1;
    param_1 = param_1 / 5;
  }
  return local_8;
}

