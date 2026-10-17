/* Entry: 0x004f86f0; symbol: FUN_004f86f0; body bytes: 336 */

void FUN_004f86f0(void)

{
  int iVar1;
  undefined4 local_20;
  void *local_1c;
  undefined4 local_18;
  float *local_14;
  void *local_10;
  void *local_c;
  void *local_8;
  
  iVar1 = FUN_00478060(0);
  FUN_00478190(iVar1);
  iVar1 = FUN_00411700(*(int *)((int)local_8 + 0x14858));
  FUN_00487bb0(iVar1);
  iVar1 = FUN_00464230(0);
  iVar1 = FUN_004ff6d0(iVar1);
  if (iVar1 == 1) {
    local_c = (void *)FUN_00464230(0);
    FUN_004ffec0(local_c,2);
    FUN_00426d70(&DAT_005ba830,0x2d,0);
    FUN_00479060(local_8,4);
  }
  else {
    if ((*(uint *)((int)local_8 + 0x14) >> 3 & 1) == 0) {
      FUN_00426d70(&DAT_005ba830,2,0);
    }
    local_10 = (void *)FUN_0041cad0(*(int *)((int)local_8 + 0x14858));
    local_18 = FUN_0045d100(local_10,0);
    local_14 = (float *)FUN_00460850((int)local_8);
    FUN_004790e0(&local_20,(uint *)"effect",0x16,local_14,0,-1,(undefined4 *)0x0);
    iVar1 = FUN_00411700(*(int *)((int)local_8 + 0x14858));
    FUN_00487bb0(iVar1);
    FUN_00423520((void *)((int)local_8 + 0x644),0);
    FUN_00412da0(local_8,4);
    FUN_00423520((void *)((int)local_8 + 0x2050),6);
    local_1c = *(void **)((int)local_8 + 0x1c);
    FUN_004382b0(local_1c,(void *)((int)local_8 + 0x28),0,0);
    FUN_004ffd00(local_8,8);
  }
  return;
}

