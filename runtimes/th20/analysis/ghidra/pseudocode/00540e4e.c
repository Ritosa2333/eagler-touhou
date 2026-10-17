/* Entry: 0x00540e4e; symbol: FUN_00540e4e; body bytes: 45 */

undefined4 * __thiscall FUN_00540e4e(void *this,byte param_1)

{
  *(undefined ***)this = &PTR_FUN_0056c47c;
  FUN_00544a5a((undefined4 *)((int)this + 4));
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return (undefined4 *)this;
}

