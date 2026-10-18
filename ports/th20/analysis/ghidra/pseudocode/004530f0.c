/* Entry: 0x004530f0; symbol: FUN_004530f0; body bytes: 295 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004530f0(void *this,float param_1)

{
  float *pfVar1;
  float10 fVar2;
  
  if ((*(uint *)((int)this + 0xc) & 1) == 0) {
    FUN_00423f50((undefined4 *)this);
    FUN_00423fe0(this,0);
    *(uint *)((int)this + 0xc) = *(uint *)((int)this + 0xc) | 1;
  }
  if ((*(uint *)((int)this + 0xc) >> 1 & 3) != 0) {
    *(uint *)((int)this + 0xc) = *(uint *)((int)this + 0xc) & 0xfffffff9;
  }
  pfVar1 = (float *)(&PTR_DAT_005aefe0)[*(uint *)((int)this + 0xc) >> 1 & 3];
  *(undefined4 *)this = *(undefined4 *)((int)this + 4);
  if (pfVar1 == (float *)0x0) {
    *(float *)((int)this + 8) = *(float *)((int)this + 8) + param_1;
  }
  else {
    fVar2 = FUN_004292e0(pfVar1);
    if (((float)fVar2 <= _DAT_0056e72c) ||
       (fVar2 = FUN_004292e0(pfVar1), DAT_0056e730 <= (float)fVar2)) {
      fVar2 = FUN_00452f50(pfVar1,param_1);
      *(float *)((int)this + 8) = *(float *)((int)this + 8) + (float)fVar2;
    }
    else {
      *(float *)((int)this + 8) = *(float *)((int)this + 8) + param_1;
    }
  }
  *(int *)((int)this + 4) = (int)*(float *)((int)this + 8);
  return;
}

