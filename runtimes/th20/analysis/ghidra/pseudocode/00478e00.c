/* Entry: 0x00478e00; symbol: FUN_00478e00; body bytes: 61 */

undefined4 __thiscall FUN_00478e00(void *this,int param_1)

{
  undefined4 local_c;
  
                    /* WARNING: Load size is inaccurate */
  if ((*(int *)((int)this + 4) == *this) || (*(int *)((int)this + 4) % param_1 != 0)) {
    local_c = 0;
  }
  else {
    local_c = 1;
  }
  return local_c;
}

