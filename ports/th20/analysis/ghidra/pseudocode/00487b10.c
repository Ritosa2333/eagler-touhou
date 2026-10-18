/* Entry: 0x00487b10; symbol: FUN_00487b10; body bytes: 121 */

undefined4 __thiscall FUN_00487b10(void *this,int param_1)

{
  void *this_00;
  int *piVar1;
  
  FUN_00488a20(this,param_1);
  this_00 = (void *)FUN_0040bbc0(&DAT_005ba568,param_1);
  FUN_00412da0(this_00,this);
  piVar1 = FUN_00412310(0x28,FUN_004885d0,this);
  *(int **)((int)this + 8) = piVar1;
  piVar1 = FUN_004123b0(0xc,FUN_00488680,this);
  *(int **)((int)this + 0xc) = piVar1;
  FUN_00423520((void *)((int)this + 0x24),0);
  return 0;
}

