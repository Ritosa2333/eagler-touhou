/* Entry: 0x0049d790; symbol: FUN_0049d790; body bytes: 183 */

undefined4 __thiscall FUN_0049d790(void *this,int param_1)

{
  void *this_00;
  int *piVar1;
  undefined4 *puVar2;
  int local_c;
  
  FUN_0049e010(this,param_1);
  this_00 = (void *)FUN_0040bbc0(&DAT_005ba568,param_1);
  FUN_0041df30(this_00,this);
  FUN_0049da70((int)this);
  piVar1 = FUN_00412310(0x29,FUN_0049de90,this);
  *(int **)((int)this + 8) = piVar1;
  piVar1 = FUN_004123b0(0x2a,FUN_0049dea0,this);
  *(int **)((int)this + 0xc) = piVar1;
  FUN_004216a0((int)this);
  for (local_c = 0; local_c < 0x400; local_c = local_c + 1) {
    puVar2 = (undefined4 *)FUN_00449490((void *)((int)this + 0x103c),local_c);
    *puVar2 = 0xffffffff;
  }
  return 0;
}

