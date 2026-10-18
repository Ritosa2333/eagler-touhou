/* Entry: 0x0043f520; symbol: FUN_0043f520; body bytes: 45 */

uint __cdecl FUN_0043f520(byte param_1,byte param_2)

{
  undefined4 local_8;
  
  local_8 = (uint)param_1 * (uint)param_2 >> 7;
  if (0xff < local_8) {
    local_8 = 0xff;
  }
  return local_8 & 0xff;
}

