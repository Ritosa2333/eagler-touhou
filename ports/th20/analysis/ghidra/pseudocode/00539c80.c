/* Entry: 0x00539c80; symbol: FUN_00539c80; body bytes: 67 */

undefined4 FUN_00539c80(void)

{
  CAsyncSocket *this;
  undefined4 local_c;
  
  this = (CAsyncSocket *)operator_new(0xc);
  if (this == (CAsyncSocket *)0x0) {
    local_c = 0;
  }
  else {
    FUN_0040c080(this,0xc);
    local_c = CAsyncSocket::CAsyncSocket(this);
  }
  return local_c;
}

