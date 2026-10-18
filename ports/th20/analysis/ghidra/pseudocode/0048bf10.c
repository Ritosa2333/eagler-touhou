/* Entry: 0x0048bf10; symbol: FUN_0048bf10; body bytes: 239 */

void __thiscall FUN_0048bf10(void *this,float *param_1,char param_2)

{
  void *pvVar1;
  
  if ((*(int *)((int)this + 8) == 0) || (param_2 != '\0')) {
    if (*(int *)((int)this + 4) != 0) {
                    /* WARNING: Load size is inaccurate */
      pvVar1 = (void *)FUN_00498fd0(*this);
      FUN_004c3c90(pvVar1,*(int *)((int)this + 4),param_1,0xffffffff,DAT_0056e0f8 / DAT_0056c8d0,
                   DAT_0056fd34,0,0,0xffffffff);
    }
  }
  else {
                    /* WARNING: Load size is inaccurate */
    pvVar1 = (void *)FUN_00498fd0(*this);
    FUN_004c3c90(pvVar1,*(int *)((int)this + 8),param_1,0xffffffff,DAT_0056e0f8 / DAT_0056c8d0,
                 DAT_0056fd34,0,0,0xffffffff);
  }
  FUN_004a5350(param_1);
  *(undefined4 *)((int)this + 4) = 0;
  return;
}

