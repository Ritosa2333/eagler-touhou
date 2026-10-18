/* Entry: 0x00533720; symbol: FUN_00533720; body bytes: 90 */

void __thiscall FUN_00533720(void *this,float *param_1,int param_2,int param_3)

{
  void *this_00;
  
                    /* WARNING: Load size is inaccurate */
  *(int *)this = *this + param_2;
                    /* WARNING: Load size is inaccurate */
  while (*(int *)((int)this + 4) < *this) {
    this_00 = (void *)FUN_00498fd0(0);
    FUN_004c45b0(this_00,param_1,1,param_3);
                    /* WARNING: Load size is inaccurate */
    *(int *)this = *this - *(int *)((int)this + 4);
  }
  return;
}

