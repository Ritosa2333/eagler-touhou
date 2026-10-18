/* Entry: 0x0041f830; symbol: FUN_0041f830; body bytes: 73 */

undefined4 * FUN_0041f830(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x2ef8);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x2ef8);
    local_c = FUN_0041f8a0(this);
  }
  return local_c;
}

