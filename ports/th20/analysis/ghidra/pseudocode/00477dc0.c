/* Entry: 0x00477dc0; symbol: FUN_00477dc0; body bytes: 100 */

undefined4 __thiscall FUN_00477dc0(void *this,undefined4 param_1)

{
  int *piVar1;
  
  piVar1 = FUN_004122c0(0x21,FUN_00477fb0,this);
  *(int **)((int)this + 8) = piVar1;
  piVar1 = FUN_00412360(0x2c,FUN_00477fc0,this);
  *(int **)((int)this + 0xc) = piVar1;
  FUN_00423520((void *)((int)this + 0x1c),0);
  *(undefined4 *)((int)this + 0x14) = 0;
  FUN_004782d0(this,param_1);
  return 0;
}

