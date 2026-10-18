/* Entry: 0x0040e1a0; symbol: FUN_0040e1a0; body bytes: 89 */

void __thiscall FUN_0040e1a0(void *this,int param_1)

{
  int local_10;
  undefined1 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *(undefined ***)this = &PTR_FUN_0056c47c;
  *(undefined4 *)((int)this + 4) = 0;
  *(undefined4 *)((int)this + 8) = 0;
  local_10 = param_1;
  local_c = 1;
  ___std_exception_copy(&local_10,(int *)((int)this + 4));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

