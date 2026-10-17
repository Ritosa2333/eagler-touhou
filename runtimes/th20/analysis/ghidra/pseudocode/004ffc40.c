/* Entry: 0x004ffc40; symbol: FUN_004ffc40; body bytes: 64 */

void __thiscall FUN_004ffc40(void *this,undefined1 param_1)

{
  FUN_004fcdc0(this,*(int *)((int)this + 8) + 1);
  *(undefined1 *)(*(int *)((int)this + 4) + *(int *)((int)this + 8)) = param_1;
  *(int *)((int)this + 8) = *(int *)((int)this + 8) + 1;
  return;
}

