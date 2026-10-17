/* Entry: 0x0043d830; symbol: FUN_0043d830; body bytes: 375 */

undefined4 __thiscall FUN_0043d830(void *this,float param_1,float param_2,float param_3)

{
  float *pfVar1;
  int *piVar2;
  
  if (*(uint *)((int)this + 0x7d40e80) < (int)this + 0x7d40e80U) {
    pfVar1 = *(float **)((int)this + 0x7d40e80);
    FUN_004455c0((int)this);
    *pfVar1 = param_1 + *(float *)((int)this + 0xd0);
    pfVar1[1] = param_2 + *(float *)((int)this + 0xd4);
    pfVar1[2] = 0.0;
    pfVar1[3] = DAT_0056c8cc;
    pfVar1[4] = param_3;
    FUN_004ddf80(0x5c4d40);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar2 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar2 + 0x164))(piVar2,0x44);
    piVar2 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar2 + 0x14c))(piVar2,1,1,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = *(int *)((int)this + 0x7d40e80) + 0x14;
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  return 0;
}

