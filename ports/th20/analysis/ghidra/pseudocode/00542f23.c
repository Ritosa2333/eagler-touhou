/* Entry: 0x00542f23; symbol: FUN_00542f23; body bytes: 108 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

void FUN_00542f23(undefined4 param_1,undefined4 param_2,int param_3,undefined *param_4)

{
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  while( true ) {
    if (param_3 == 0) break;
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*(code *)param_4)();
    param_3 = param_3 + -1;
  }
  FUN_00542f9b();
  *unaff_FS_OFFSET = local_14;
  return;
}

