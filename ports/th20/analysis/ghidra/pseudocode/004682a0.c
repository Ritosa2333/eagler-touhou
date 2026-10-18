/* Entry: 0x004682a0; symbol: FUN_004682a0; body bytes: 60 */

undefined1 __cdecl FUN_004682a0(char *param_1)

{
  undefined1 local_c;
  
  if ((*param_1 < -0x80) || ('\x7f' < *param_1)) {
    local_c = 0;
  }
  else {
    local_c = 1;
  }
  return local_c;
}

