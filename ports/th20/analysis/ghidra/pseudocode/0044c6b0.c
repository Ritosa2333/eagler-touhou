/* Entry: 0x0044c6b0; symbol: FUN_0044c6b0; body bytes: 241 */

int __thiscall FUN_0044c6b0(void *this,int param_1,int param_2)

{
  void *this_00;
  int iVar1;
  undefined4 *local_c;
  
  local_c = (undefined4 *)FUN_004494b0((void *)((int)this + 0x514),1);
  do {
    if (local_c == (undefined4 *)0x0) {
      return 0;
    }
    this_00 = (void *)FUN_0040c300(local_c);
    if ((this_00 != (void *)0x0) && (this_00 != this)) {
      if ((*(short *)((int)this_00 + 0x440) == param_1) || (param_1 == -1)) {
        if (param_2 == 0) {
          return (int)this_00;
        }
        param_2 = param_2 + -1;
      }
      iVar1 = FUN_004494b0((void *)((int)this_00 + 0x514),1);
      iVar1 = FUN_0040ff90(iVar1);
      if ((iVar1 != 0) && (iVar1 = FUN_0044c6b0(this_00,param_1,param_2), iVar1 != 0)) {
        return iVar1;
      }
      if ((*(short *)((int)this + 0x440) == -2) && (iVar1 = FUN_0040ff90((int)local_c), iVar1 == 0))
      {
        iVar1 = FUN_0040c300(local_c);
        return iVar1;
      }
    }
    local_c = (undefined4 *)FUN_0040ff90((int)local_c);
  } while( true );
}

