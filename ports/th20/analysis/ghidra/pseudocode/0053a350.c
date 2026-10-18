/* Entry: 0x0053a350; symbol: FUN_0053a350; body bytes: 101 */

undefined4 * __thiscall FUN_0053a350(void *this,byte *param_1)

{
  int iVar1;
  int local_10;
  undefined4 *local_8;
  
                    /* WARNING: Load size is inaccurate */
  if (*this != 0) {
                    /* WARNING: Load size is inaccurate */
    local_8 = *this;
    for (local_10 = *(int *)((int)this + 4); 0 < local_10; local_10 = local_10 + -1) {
      iVar1 = FUN_00555750(param_1,(byte *)*local_8);
      if (iVar1 == 0) {
        return local_8;
      }
      local_8 = local_8 + 4;
    }
  }
  return (undefined4 *)0x0;
}

