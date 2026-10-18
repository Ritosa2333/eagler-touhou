/* Entry: 0x004d8740; symbol: FUN_004d8740; body bytes: 68 */

void * FUN_004d8740(void)

{
  undefined4 local_c;
  
  local_c = operator_new(0x2c);
  if (local_c == (void *)0x0) {
    local_c = (void *)0x0;
  }
  else {
    _memset(local_c,0,0x2c);
  }
  return local_c;
}

