/* Entry: 0x00538160; symbol: FUN_00538160; body bytes: 70 */

float * __thiscall FUN_00538160(void *this,float *param_1)

{
  FUN_004398a0(param_1);
                    /* WARNING: Load size is inaccurate */
  *param_1 = (float)*this / DAT_0056cd98;
  param_1[1] = (float)*(int *)((int)this + 4) / DAT_0056cd98;
  return param_1;
}

