/* Entry: 0x00531400; symbol: FUN_00531400; body bytes: 87 */

void FUN_00531400(void)

{
  char cVar1;
  int iVar2;
  void *pvVar3;
  
  iVar2 = FUN_00464080(0);
  cVar1 = FUN_00464400(iVar2);
  if (cVar1 == '\0') {
    pvVar3 = (void *)FUN_00464080(0);
    FUN_004be440(pvVar3,0x1e);
  }
  else {
    pvVar3 = (void *)FUN_00464080(0);
    FUN_004be440(pvVar3,0x14);
  }
  return;
}

