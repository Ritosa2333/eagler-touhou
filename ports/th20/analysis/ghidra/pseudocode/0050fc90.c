/* Entry: 0x0050fc90; symbol: FUN_0050fc90; body bytes: 80 */

int __thiscall FUN_0050fc90(void *this,int param_1)

{
  int local_c;
  int local_8;
  
  local_c = 0;
  for (local_8 = 0; local_8 < param_1 + -8; local_8 = local_8 + 1) {
    local_c = (uint)*(byte *)((int)this + local_8 + 8) + local_c;
  }
  return local_c;
}

