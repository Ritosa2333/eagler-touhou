/* Entry: 0x004f70f0; symbol: FUN_004f70f0; body bytes: 65 */

int * __thiscall FUN_004f70f0(void *this,int *param_1,int *param_2)

{
  int iVar1;
  
                    /* WARNING: Load size is inaccurate */
  *(int *)this = *this + *param_2;
  *(int *)((int)this + 4) = *(int *)((int)this + 4) + param_2[1];
                    /* WARNING: Load size is inaccurate */
  iVar1 = *(int *)((int)this + 4);
  *param_1 = *this;
  param_1[1] = iVar1;
  return param_1;
}

