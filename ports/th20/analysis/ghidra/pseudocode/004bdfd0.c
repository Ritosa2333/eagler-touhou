/* Entry: 0x004bdfd0; symbol: FUN_004bdfd0; body bytes: 117 */

void __thiscall FUN_004bdfd0(void *this,float param_1)

{
  *(int *)((int)this + 0x214) = (int)(param_1 * DAT_0056e04c);
  if (*(int *)((int)this + 0x214) < *(int *)((int)this + 0x21c)) {
    if (*(int *)((int)this + 0x214) < *(int *)((int)this + 0x218)) {
      *(undefined4 *)((int)this + 0x214) = *(undefined4 *)((int)this + 0x218);
    }
  }
  else {
    *(undefined4 *)((int)this + 0x214) = *(undefined4 *)((int)this + 0x21c);
  }
  return;
}

