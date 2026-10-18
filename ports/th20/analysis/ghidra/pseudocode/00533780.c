/* Entry: 0x00533780; symbol: FUN_00533780; body bytes: 406 */

void __thiscall FUN_00533780(void *this,int param_1)

{
  bool bVar1;
  char cVar2;
  undefined3 extraout_var;
  int iVar3;
  int iVar4;
  void *pvVar5;
  undefined4 uVar6;
  int local_c;
  
  bVar1 = FUN_00478160(DAT_005c06a4);
  if ((CONCAT31(extraout_var,bVar1) == 0) && (*(int *)((int)this + 0x54) == 0)) {
    iVar3 = FUN_00464080(0);
    cVar2 = FUN_00464400(iVar3);
    if (cVar2 == '\0') {
      local_c = 10;
    }
    else {
      local_c = 0xf;
    }
    iVar3 = FUN_00464080(0);
    cVar2 = FUN_00534390(iVar3);
    if (cVar2 != '\0') {
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_0051b8a0(iVar3);
      if (iVar3 % local_c == local_c + -1) {
        param_1 = param_1 + 1;
        FUN_004865b0(*(void **)((int)this + 0x34),1);
      }
    }
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_0051b8a0(iVar3);
    iVar4 = FUN_00464080(0);
    iVar4 = FUN_0051b920(iVar4);
    if (iVar3 < iVar4) {
      pvVar5 = (void *)FUN_00464080(0);
      FUN_00533920(pvVar5,param_1);
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_0051b8a0(iVar3);
      iVar4 = FUN_00464080(0);
      iVar4 = FUN_0051b920(iVar4);
      if (iVar4 <= iVar3) {
        pvVar5 = (void *)FUN_00464080(0);
        iVar3 = FUN_00464080(0);
        uVar6 = FUN_0051b920(iVar3);
        FUN_004be940(pvVar5,uVar6);
        FUN_00534d00();
      }
    }
    else if (*(int *)((int)this + 0x54) == 0) {
      pvVar5 = (void *)FUN_00464080(0);
      iVar3 = FUN_00464080(0);
      uVar6 = FUN_0051b920(iVar3);
      FUN_004be940(pvVar5,uVar6);
      FUN_00534d00();
    }
  }
  return;
}

