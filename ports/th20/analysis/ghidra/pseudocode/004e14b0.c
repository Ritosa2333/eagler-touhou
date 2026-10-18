/* Entry: 0x004e14b0; symbol: FUN_004e14b0; body bytes: 88 */

void __thiscall FUN_004e14b0(void *this,uint param_1,uint param_2)

{
  uint uVar1;
  undefined8 uVar2;
  
  uVar2 = __aulldiv(param_1,param_2,10,0);
                    /* WARNING: Load size is inaccurate */
  uVar1 = *this;
  *(uint *)this = (uint)uVar2 + *this;
  *(uint *)((int)this + 4) =
       (int)((ulonglong)uVar2 >> 0x20) + *(int *)((int)this + 4) + (uint)CARRY4((uint)uVar2,uVar1);
                    /* WARNING: Load size is inaccurate */
  if ((*(int *)((int)this + 4) != 0) || (999999999 < *this)) {
    *(undefined4 *)this = 999999999;
    *(undefined4 *)((int)this + 4) = 0;
  }
  return;
}

