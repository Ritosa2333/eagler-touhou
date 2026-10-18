/* Entry: 0x00557c08; symbol: FUN_00557c08; body bytes: 100 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

BOOL FUN_00557c08(int *param_1,undefined4 *param_2)

{
  BOOL BVar1;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(*param_1);
  DAT_005e565c = *(undefined4 *)*param_2;
  BVar1 = EnumSystemLocalesW(FUN_00557be6,1);
  DAT_005e565c = 0;
  FUN_00557c6f();
  *unaff_FS_OFFSET = local_14;
  return BVar1;
}

