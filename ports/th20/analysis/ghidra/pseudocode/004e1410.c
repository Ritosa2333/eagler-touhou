/* Entry: 0x004e1410; symbol: FUN_004e1410; body bytes: 146 */

bool __thiscall FUN_004e1410(void *this,int param_1)

{
  bool bVar1;
  
  if (*(int *)((int)this + 0x30) < *(int *)((int)this + 0x34)) {
    *(int *)((int)this + 0x30) = *(int *)((int)this + 0x30) + param_1;
    if (*(int *)((int)this + 0x34) < *(int *)((int)this + 0x30)) {
      *(undefined4 *)((int)this + 0x30) = *(undefined4 *)((int)this + 0x34);
      FUN_004b90e0(DAT_005c06a4,2,0);
    }
    bVar1 = (*(int *)((int)this + 0x30) - param_1) / *(int *)((int)this + 0x38) !=
            *(int *)((int)this + 0x30) / *(int *)((int)this + 0x38);
  }
  else {
    bVar1 = false;
  }
  return bVar1;
}

