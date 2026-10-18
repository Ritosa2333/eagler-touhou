/* Entry: 0x0044c670; symbol: FUN_0044c670; body bytes: 63 */

int __thiscall FUN_0044c670(void *this,int param_1,int param_2)

{
  void *this_00;
  undefined4 local_c;
  
  this_00 = (void *)FUN_0044ced0((undefined4 *)this);
  if (this_00 == (void *)0x0) {
    local_c = 0;
  }
  else {
    local_c = FUN_0044c6b0(this_00,param_1,param_2);
  }
  return local_c;
}

