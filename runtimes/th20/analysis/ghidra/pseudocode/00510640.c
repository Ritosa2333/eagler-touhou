/* Entry: 0x00510640; symbol: FUN_00510640; body bytes: 200 */

undefined4 __thiscall FUN_00510640(void *this,int param_1)

{
  int *piVar1;
  void *this_00;
  
  *(undefined4 *)((int)this + 0x10) = *(undefined4 *)(DAT_005c0698 + 0x1a244);
  piVar1 = FUN_00412310(0x19,FUN_00510810,this);
  *(int **)((int)this + 8) = piVar1;
  piVar1 = FUN_004123b0(0x33,FUN_00510820,this);
  *(int **)((int)this + 0xc) = piVar1;
  FUN_004708e0(*(void **)((int)this + 0x10),(void *)((int)this + 0x1c),0x121);
  *(byte *)((int)this + 0x4bf) = *(byte *)((int)this + 0x4bf) & 0xfc | 2;
  *(undefined1 *)((int)this + 0x4c0) = 3;
  FUN_00510890(this,param_1);
  this_00 = (void *)FUN_0040bbc0(&DAT_005ba568,param_1);
  FUN_0041de70(this_00,this);
  return 0;
}

