/* Entry: 0x004bfa00; symbol: FUN_004bfa00; body bytes: 198 */

int __thiscall FUN_004bfa00(void *this,int param_1)

{
  char cVar1;
  int iVar2;
  
  if (*(int *)((int)this + 8) < 1) {
                    /* WARNING: Load size is inaccurate */
    iVar2 = *this;
  }
  else {
    do {
                    /* WARNING: Load size is inaccurate */
      *(int *)this = *this + param_1;
                    /* WARNING: Load size is inaccurate */
      while (*(int *)((int)this + 8) <= *this) {
        if (*(int *)((int)this + 0x48) == 0) {
          *(int *)this = *(int *)((int)this + 8) + -1;
        }
        else {
                    /* WARNING: Load size is inaccurate */
          *(int *)this = *this - (*(int *)((int)this + 8) - *(int *)((int)this + 0xc));
        }
      }
                    /* WARNING: Load size is inaccurate */
      while (*this < *(int *)((int)this + 0xc)) {
        if (*(int *)((int)this + 0x48) == 0) {
          *(undefined4 *)this = *(undefined4 *)((int)this + 0xc);
        }
        else {
                    /* WARNING: Load size is inaccurate */
          *(int *)this = (*(int *)((int)this + 8) - *(int *)((int)this + 0xc)) + *this;
        }
      }
      cVar1 = FUN_0046a4d0();
    } while (cVar1 != '\0');
                    /* WARNING: Load size is inaccurate */
    iVar2 = *this;
  }
  return iVar2;
}

