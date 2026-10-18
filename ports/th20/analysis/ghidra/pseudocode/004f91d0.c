/* Entry: 0x004f91d0; symbol: FUN_004f91d0; body bytes: 661 */

void __thiscall
FUN_004f91d0(void *this,float *param_1,float param_2,float param_3,float param_4,int param_5)

{
  float fVar1;
  bool bVar2;
  char cVar3;
  int iVar4;
  float *pfVar5;
  undefined3 extraout_var;
  float fVar6;
  float local_24;
  float local_20;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  local_14 = *param_1;
  local_10 = param_1[1];
  local_c = param_1[2];
  bVar2 = FUN_004ff5e0((int)this);
  if (bVar2) {
    local_20 = *(float *)((int)this + 0x2098);
  }
  else {
    local_20 = *(float *)((int)this + 0x2094);
  }
  iVar4 = FUN_0041caf0(*(int *)((int)this + 0x14858));
  iVar4 = FUN_004ff490(iVar4);
  fVar6 = ((float)iVar4 * local_20) / DAT_0056e04c;
  bVar2 = FUN_004ff5e0((int)this);
  if (bVar2) {
    local_24 = *(float *)((int)this + 0x2098);
  }
  else {
    local_24 = *(float *)((int)this + 0x2094);
  }
  local_24 = local_24 + DAT_0056f280;
  iVar4 = FUN_00460850((int)this);
  fVar1 = *(float *)(iVar4 + 4);
  pfVar5 = (float *)FUN_00460850((int)this);
  cVar3 = FUN_00457610(local_14,local_10,param_4,param_3,param_2,*pfVar5,fVar1,local_24);
  if (cVar3 != '\0') {
    iVar4 = FUN_00460850((int)this);
    fVar1 = *(float *)(iVar4 + 4);
    pfVar5 = (float *)FUN_00460850((int)this);
    cVar3 = FUN_00457610(local_14,local_10,param_4,param_3,param_2,*pfVar5,fVar1,fVar6);
    if ((((cVar3 != '\0') &&
         (((DAT_005c06a4 == 0 ||
           (bVar2 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var,bVar2) == 0)) &&
          (param_5 == 0)))) &&
        (((iVar4 = FUN_00411700((int)this), iVar4 != 2 &&
          (iVar4 = FUN_00411700((int)this), iVar4 != 4)) &&
         (iVar4 = FUN_00411700((int)this), iVar4 != 3)))) &&
       (bVar2 = FUN_00461070((void *)((int)this + 0x2050),0), !bVar2)) {
      FUN_004f86f0();
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

