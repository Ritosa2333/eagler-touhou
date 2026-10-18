/* Entry: 0x00426ef0; symbol: FUN_00426ef0; body bytes: 346 */

void __thiscall FUN_00426ef0(void *this,int param_1)

{
  float fVar1;
  
                    /* WARNING: Load size is inaccurate */
  if (*this != 0) {
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
    (**(code **)(**this + 0x48))(*this);
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
    (**(code **)(**this + 0x34))(*this,0);
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
    (**(code **)(**this + 0x40))(*this,param_1);
    *(int *)((int)this + 0x10) = param_1;
    if (DAT_005c0010 == 0) {
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
      (**(code **)(**this + 0x3c))(*this,0xffffd8f0);
    }
    else {
      fVar1 = DAT_0056c8cc - (float)DAT_005c0010 / DAT_0056e04c;
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
      (**(code **)(**this + 0x3c))
                (*this,(int)((float)(*(short *)(*(int *)((int)this + 8) + 8) + 5000) *
                            (DAT_0056c8cc - fVar1 * fVar1 * fVar1)) + -5000);
    }
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
    (**(code **)(**this + 0x30))(*this,0,0,*(undefined4 *)(*(int *)((int)this + 8) + 0xc));
  }
  return;
}

