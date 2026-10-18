/* Entry: 0x00558304; symbol: FUN_00558304; body bytes: 61 */

void __fastcall FUN_00558304(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6064 != (FARPROC)0xffffffff) {
    pFVar1 = DAT_005e6064;
    if ((DAT_005e6064 == (FARPROC)0x0) &&
       (pFVar1 = FUN_00557e53(param_1,0x19,"RoUninitialize",(int *)&DAT_00599e4c,
                              (int *)"RoUninitialize"), pFVar1 == (FARPROC)0x0)) {
      return;
    }
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*pFVar1)();
  }
  return;
}

