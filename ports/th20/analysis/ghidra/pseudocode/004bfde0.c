/* Entry: 0x004bfde0; symbol: FUN_004bfde0; body bytes: 73 */

undefined4 * FUN_004bfde0(void)

{
  undefined4 *this;
  undefined4 local_c;
  
  this = (undefined4 *)operator_new(0xc460);
  if (this == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0xc460);
    local_c = FUN_004bfee0(this);
  }
  return local_c;
}

