/* Entry: 0x00511f00; symbol: FUN_00511f00; body bytes: 384 */

undefined4 __thiscall FUN_00511f00(void *this,int param_1)

{
  void *pvVar1;
  float *pfVar2;
  undefined4 *puVar3;
  int iVar4;
  int iVar5;
  
  pvVar1 = (void *)FUN_00478e80(0);
  pfVar2 = (float *)FUN_0047a2c0(param_1);
  FUN_0047cf60(pvVar1,pfVar2,DAT_0056f10c,0,99999,0);
  pvVar1 = (void *)FUN_00478ee0(0);
  puVar3 = (undefined4 *)FUN_0047a2c0(param_1);
  FUN_004caad0(pvVar1,puVar3,DAT_0056f10c,0,1);
                    /* WARNING: Load size is inaccurate */
  switch(*(undefined4 *)(*this + 0x2c)) {
  case 0:
    pvVar1 = (void *)FUN_00464080(0);
    FUN_004fea60(pvVar1,1);
    break;
  case 1:
    pvVar1 = (void *)FUN_00464080(0);
    FUN_004fe9b0(pvVar1,1);
    break;
  case 2:
    pvVar1 = (void *)FUN_00464080(0);
    FUN_004feab0(pvVar1,1);
    break;
  case 3:
    pvVar1 = (void *)FUN_00464080(0);
    FUN_004fea00(pvVar1,1);
  }
  iVar4 = FUN_00464080(0);
  iVar4 = FUN_00499520(iVar4);
  iVar5 = FUN_00464080(0);
  iVar5 = FUN_00513e60(iVar5);
  if (iVar5 / 2 <= iVar4) {
    pvVar1 = (void *)FUN_00464080(0);
    iVar4 = FUN_00464080(0);
    iVar4 = FUN_00513e60(iVar4);
    FUN_004be620(pvVar1,iVar4 / 2);
  }
  pvVar1 = (void *)FUN_00464080(0);
  FUN_00513c70(pvVar1,0x96);
  return 0;
}

