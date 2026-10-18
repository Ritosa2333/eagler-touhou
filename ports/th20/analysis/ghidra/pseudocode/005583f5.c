/* Entry: 0x005583f5; symbol: FUN_005583f5; body bytes: 49 */

bool __fastcall FUN_005583f5(void *param_1)

{
  FARPROC pFVar1;
  bool bVar2;
  
  if (DAT_005e600c == -1) {
    pFVar1 = (FARPROC)0x0;
  }
  else {
    bVar2 = DAT_005e600c == 0;
    if (!bVar2) goto LAB_00558422;
    pFVar1 = FUN_00557e53(param_1,3,"FlsGetValue2",(int *)&DAT_00599d48,(int *)"FlsGetValue2");
  }
  bVar2 = pFVar1 == (FARPROC)0x0;
LAB_00558422:
  return !bVar2;
}

