/* Entry: 0x00413c50; symbol: FUN_00413c50; body bytes: 73 */

void * FUN_00413c50(void)

{
  void *this;
  undefined4 local_c;
  
  this = operator_new(0x124);
  if (this == (void *)0x0) {
    local_c = (void *)0x0;
  }
  else {
    FUN_0040c080(this,0x124);
    local_c = FUN_00414200(this);
  }
  return local_c;
}

