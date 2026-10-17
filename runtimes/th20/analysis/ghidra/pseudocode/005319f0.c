/* Entry: 0x005319f0; symbol: FUN_005319f0; body bytes: 87 */

void FUN_005319f0(void)

{
  char cVar1;
  int iVar2;
  void *pvVar3;
  
  iVar2 = FUN_00464080(0);
  cVar1 = FUN_00464400(iVar2);
  if (cVar1 == '\0') {
    pvVar3 = (void *)FUN_00464080(0);
    FUN_004beaf0(pvVar3,0xf);
  }
  else {
    pvVar3 = (void *)FUN_00464080(0);
    FUN_004beaf0(pvVar3,10);
  }
  return;
}

