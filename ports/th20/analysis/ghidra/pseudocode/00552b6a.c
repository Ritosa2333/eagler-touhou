/* Entry: 0x00552b6a; symbol: FUN_00552b6a; body bytes: 196 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

void __fastcall FUN_00552b6a(undefined4 *param_1)

{
  code *pcVar1;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  if (DAT_005e5498 != '\0') goto LAB_00552c1e;
  LOCK();
  DAT_005e5490 = 1;
  UNLOCK();
  if (*(int *)*param_1 == 0) {
    if (DAT_005e5494 != DAT_005b25c0) {
      pcVar1 = (code *)FUN_0055299d(DAT_005e5494);
      (*(code *)PTR_guard_check_icall_0056c37c)(0,0,0);
      (*pcVar1)();
    }
LAB_00552bd5:
    FUN_00553825();
  }
  else if (*(int *)*param_1 == 1) goto LAB_00552bd5;
  if (*(int *)*param_1 == 0) {
    FUN_005559fe((undefined4 *)&DAT_0056c440,(undefined4 *)&DAT_0056c450);
  }
  FUN_005559fe((undefined4 *)&DAT_0056c454,(undefined4 *)&DAT_0056c458);
  if (*(int *)param_1[1] == 0) {
    DAT_005e5498 = '\x01';
    *(undefined1 *)param_1[2] = 1;
  }
LAB_00552c1e:
  *unaff_FS_OFFSET = local_14;
  return;
}

