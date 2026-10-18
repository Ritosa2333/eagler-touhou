/* Entry: 0x00411910; symbol: FUN_00411910; body bytes: 67 */

undefined4 * FUN_00411910(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x2c);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x2c);
    local_c = FUN_00411a70(this);
  }
  return local_c;
}

