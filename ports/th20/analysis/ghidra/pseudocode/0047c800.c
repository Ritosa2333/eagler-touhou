/* Entry: 0x0047c800; symbol: FUN_0047c800; body bytes: 224 */

bool __fastcall FUN_0047c800(int param_1)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  void *this;
  float10 fVar4;
  undefined1 local_34 [12];
  void *local_28;
  void *local_24;
  float local_20;
  void *local_1c;
  void *local_18;
  int local_14;
  void *local_10;
  void *local_c;
  int local_8;
  
  local_c = (void *)(param_1 + 0xa0);
  local_8 = param_1;
  local_18 = (void *)FUN_0047c680(local_c,9);
  local_10 = (void *)(local_8 + 0xa0);
  iVar2 = FUN_0047c680(local_10,9);
  local_14 = *(int *)(iVar2 + 0x30);
  bVar1 = FUN_004235c0(local_18,local_14);
  if (!bVar1) {
    local_1c = (void *)(local_8 + 0xa0);
    iVar2 = FUN_0047c680(local_1c,9);
    local_24 = (void *)(iVar2 + 0x24);
    fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
    local_20 = (float)fVar4;
    pfVar3 = (float *)FUN_004292f0(local_24,local_34,local_20);
    FUN_004296e0((void *)(local_8 + 100),pfVar3);
    local_28 = (void *)(local_8 + 0xa0);
    iVar2 = 0;
    this = (void *)FUN_0047c680(local_28,9);
    FUN_00423520(this,iVar2);
  }
  else {
    *(uint *)(local_8 + 0x90) = *(uint *)(local_8 + 0x90) & 0xfff7ffff;
  }
  return bVar1;
}

