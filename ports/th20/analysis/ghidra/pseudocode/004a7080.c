/* Entry: 0x004a7080; symbol: FUN_004a7080; body bytes: 238 */

undefined4 __thiscall FUN_004a7080(void *this,int param_1,undefined4 param_2)

{
  void *this_00;
  undefined4 *puVar1;
  int *piVar2;
  
  this_00 = (void *)FUN_0040bbc0(&DAT_005ba568,param_1);
  FUN_0049c4e0(this_00,this);
  FUN_004ab8f0(this,param_1);
  puVar1 = FUN_004a2870();
  *(undefined4 **)((int)this + 0x104) = puVar1;
  FUN_004ab8b0(*(void **)((int)this + 0x104),param_1);
  (**(code **)**(undefined4 **)((int)this + 0x104))
            (param_2,*(code **)**(undefined4 **)((int)this + 0x104),
             *(undefined4 *)((int)this + 0x104));
  piVar2 = FUN_00412310(0x24,FUN_004aa1b0,this);
  *(int **)((int)this + 8) = piVar2;
  piVar2 = FUN_004123b0(0x17,FUN_004aa930,this);
  *(int **)((int)this + 0xc) = piVar2;
  FUN_00423520((void *)((int)this + 0x9c),0);
  *(undefined4 *)((int)this + 0x98) = 99999;
  *(undefined4 *)((int)this + 200) = 0;
  return 0;
}

