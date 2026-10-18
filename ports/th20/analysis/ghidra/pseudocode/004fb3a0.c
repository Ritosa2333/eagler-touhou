/* Entry: 0x004fb3a0; symbol: FUN_004fb3a0; body bytes: 115 */

void __thiscall FUN_004fb3a0(void *this,int param_1)

{
  bool bVar1;
  
  FUN_0045c300(this,param_1);
  bVar1 = FUN_00461070(this,0x1e);
  if (bVar1) {
    FUN_00423520(this,0x1e);
  }
  *(int *)((int)this + 0x38) = *(int *)((int)this + 0x38) + 1;
  *(undefined4 *)((int)this + 0x30) = *(undefined4 *)((int)this + 0x38);
  if (*(int *)((int)this + 0x3c) < *(int *)((int)this + 0x30)) {
    *(undefined4 *)((int)this + 0x3c) = *(undefined4 *)((int)this + 0x30);
  }
  return;
}

