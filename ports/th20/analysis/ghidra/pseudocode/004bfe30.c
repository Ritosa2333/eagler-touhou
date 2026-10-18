/* Entry: 0x004bfe30; symbol: FUN_004bfe30; body bytes: 73 */

void * FUN_004bfe30(void)

{
  void *this;
  undefined4 local_c;
  
  this = operator_new(0xc4);
  if (this == (void *)0x0) {
    local_c = (void *)0x0;
  }
  else {
    FUN_0040c080(this,0xc4);
    local_c = FUN_004bffb0(this);
  }
  return local_c;
}

