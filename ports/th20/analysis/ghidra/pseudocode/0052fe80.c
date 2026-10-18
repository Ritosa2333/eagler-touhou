/* Entry: 0x0052fe80; symbol: FUN_0052fe80; body bytes: 87 */

void FUN_0052fe80(void)

{
  char cVar1;
  int iVar2;
  void *pvVar3;
  
  iVar2 = FUN_00464080(0);
  cVar1 = FUN_00464400(iVar2);
  if (cVar1 == '\0') {
    pvVar3 = (void *)FUN_00464080(0);
    FUN_004bec10(pvVar3,0xc);
  }
  else {
    pvVar3 = (void *)FUN_00464080(0);
    FUN_004bec10(pvVar3,7);
  }
  return;
}

