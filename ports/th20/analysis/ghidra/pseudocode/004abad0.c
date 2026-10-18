/* Entry: 0x004abad0; symbol: FUN_004abad0; body bytes: 73 */

undefined4 * FUN_004abad0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0x108);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x108);
    local_c = FUN_004abb80(this);
  }
  return local_c;
}

