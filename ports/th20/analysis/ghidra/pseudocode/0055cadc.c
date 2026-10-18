/* Entry: 0x0055cadc; symbol: FUN_0055cadc; body bytes: 162 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

int FUN_0055cadc(void)

{
  int iVar1;
  int iVar2;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_20;
  undefined4 local_14;
  
  local_20 = 0;
  ___acrt_lock(8);
  for (iVar2 = 3; iVar2 != DAT_005e5318; iVar2 = iVar2 + 1) {
    iVar1 = *(int *)(DAT_005e531c + iVar2 * 4);
    if (iVar1 != 0) {
      if ((*(uint *)(iVar1 + 0xc) >> 0xd & 1) != 0) {
        iVar1 = FUN_00564518(*(FILE **)(DAT_005e531c + iVar2 * 4));
        if (iVar1 != -1) {
          local_20 = local_20 + 1;
        }
      }
      DeleteCriticalSection((LPCRITICAL_SECTION)(*(int *)(DAT_005e531c + iVar2 * 4) + 0x20));
      FUN_00557ba0(*(LPVOID *)(DAT_005e531c + iVar2 * 4));
      *(undefined4 *)(DAT_005e531c + iVar2 * 4) = 0;
    }
  }
  FUN_0055cb7e();
  *unaff_FS_OFFSET = local_14;
  return local_20;
}

