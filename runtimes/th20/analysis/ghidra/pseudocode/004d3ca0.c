/* Entry: 0x004d3ca0; symbol: FUN_004d3ca0; body bytes: 186 */

undefined4 __thiscall FUN_004d3ca0(void *this,int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  void *this_00;
  
  iVar1 = FUN_0044eb30(DAT_005c0028,7,(char **)"bullet.anm");
  *(int *)((int)this + 0x48) = iVar1;
  if (iVar1 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056fc8c);
    uVar2 = 0xffffffff;
  }
  else {
    piVar3 = FUN_00412310(0x25,FUN_004d5b10,this);
    *(int **)((int)this + 8) = piVar3;
    piVar3 = FUN_004123b0(0x27,FUN_004d5bb0,this);
    *(int **)((int)this + 0xc) = piVar3;
    this_00 = (void *)FUN_0040bbc0(&DAT_005ba568,param_1);
    FUN_0041de50(this_00,this);
    FUN_004d7e10(this,param_1);
    FUN_0044a640((void *)((int)this + 0x10),0);
    uVar2 = 0;
  }
  return uVar2;
}

