/* Entry: 0x004e1680; symbol: FUN_004e1680; body bytes: 130 */

bool __thiscall FUN_004e1680(void *this,int param_1)

{
  bool bVar1;
  
  if (*(int *)((int)this + 0x38) < *(int *)((int)this + 0x30)) {
    *(int *)((int)this + 0x30) = *(int *)((int)this + 0x30) - param_1;
    if (*(int *)((int)this + 0x30) < *(int *)((int)this + 0x38)) {
      *(undefined4 *)((int)this + 0x30) = *(undefined4 *)((int)this + 0x38);
    }
    bVar1 = (*(int *)((int)this + 0x30) + param_1) / *(int *)((int)this + 0x38) !=
            *(int *)((int)this + 0x30) / *(int *)((int)this + 0x38);
  }
  else {
    bVar1 = false;
  }
  return bVar1;
}

