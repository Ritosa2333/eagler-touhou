/* Entry: 0x004c3c00; symbol: FUN_004c3c00; body bytes: 141 */

undefined4 __thiscall FUN_004c3c00(void *this,int param_1)

{
  void *this_00;
  int *piVar1;
  
  this_00 = (void *)FUN_0040bbc0(&DAT_005ba568,param_1);
  FUN_00412dc0(this_00,this);
  FUN_004c4fd0(this,param_1);
  piVar1 = FUN_00412310(0x27,FUN_004c46f0,this);
  *(int **)((int)this + 8) = piVar1;
  piVar1 = FUN_004123b0(0x23,FUN_004c4770,this);
  *(int **)((int)this + 0xc) = piVar1;
  piVar1 = FUN_004123b0(0x13,FUN_004c4740,this);
  *(int **)((int)this + 0x14) = piVar1;
  FUN_004bc220((int)this);
  return 0;
}

