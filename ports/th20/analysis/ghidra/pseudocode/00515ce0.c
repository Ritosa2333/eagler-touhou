/* Entry: 0x00515ce0; symbol: FUN_00515ce0; body bytes: 73 */

int * __thiscall FUN_00515ce0(void *this,int param_1)

{
  int *piVar1;
  
  *(int *)this = param_1;
                    /* WARNING: Load size is inaccurate */
  piVar1 = (int *)FUN_00438520((int)*this + *(int *)(**this + 4));
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 4))();
  }
  return (int *)this;
}

