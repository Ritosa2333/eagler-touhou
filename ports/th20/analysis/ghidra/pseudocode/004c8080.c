/* Entry: 0x004c8080; symbol: FUN_004c8080; body bytes: 92 */

undefined4 * FUN_004c8080(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x1370);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x1370);
    _memset(this,0,0x1370);
    local_c = FUN_004c8410(this);
  }
  return local_c;
}

