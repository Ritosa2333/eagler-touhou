/* Entry: 0x0049e930; symbol: FUN_0049e930; body bytes: 67 */

undefined4 * FUN_0049e930(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x28);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x28);
    local_c = FUN_0049ec30(this);
  }
  return local_c;
}

