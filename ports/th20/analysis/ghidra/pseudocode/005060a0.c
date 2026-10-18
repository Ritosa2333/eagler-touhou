/* Entry: 0x005060a0; symbol: FUN_005060a0; body bytes: 82 */

int __thiscall FUN_005060a0(void *this,int param_1)

{
  undefined4 local_c;
  
  if (param_1 < 100) {
    local_c = FUN_004ba3e0((void *)((int)this + 0x684),param_1);
  }
  else {
    local_c = FUN_004ba3e0((void *)((int)this + 0x123c),param_1 + -100);
  }
  return local_c;
}

