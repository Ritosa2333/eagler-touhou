/* Entry: 0x004a3f80; symbol: FUN_004a3f80; body bytes: 121 */

int __thiscall FUN_004a3f80(void *this,int param_1)

{
  int iVar1;
  
  *(int *)((int)this + 0x14) = *(int *)((int)this + 0x14) + param_1;
  if ((*(uint *)((int)this + 0x18) & 1) == 0) {
                    /* WARNING: Load size is inaccurate */
    iVar1 = *this - param_1;
    *(int *)this = iVar1;
  }
  else {
    *(int *)((int)this + 0xc) = *(int *)((int)this + 0xc) - param_1;
    *(int *)this = (*(int *)((int)this + 0xc) + *(int *)((int)this + 0x10) * -7) / 7 +
                   *(int *)((int)this + 0x10);
                    /* WARNING: Load size is inaccurate */
    iVar1 = *this;
  }
  return iVar1;
}

