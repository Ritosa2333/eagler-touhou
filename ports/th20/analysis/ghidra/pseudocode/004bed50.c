/* Entry: 0x004bed50; symbol: FUN_004bed50; body bytes: 157 */

int __thiscall FUN_004bed50(void *this,int param_1)

{
  char cVar1;
  int local_10;
  int local_c;
  
  if (*(int *)((int)this + 8) == 0) {
    *(int *)this = param_1;
  }
  else {
    if (param_1 < *(int *)((int)this + 8)) {
      if (param_1 < 0) {
        local_c = 0;
      }
      else {
        local_c = param_1;
      }
      local_10 = local_c;
    }
    else {
      local_10 = *(int *)((int)this + 8) + -1;
    }
    *(int *)this = local_10;
    while (cVar1 = FUN_0046a4d0(), cVar1 != '\0') {
                    /* WARNING: Load size is inaccurate */
      *(int *)this = *this + 1;
                    /* WARNING: Load size is inaccurate */
      if (*(int *)((int)this + 8) <= *this) {
        *(undefined4 *)this = 0;
      }
    }
  }
                    /* WARNING: Load size is inaccurate */
  return *this;
}

