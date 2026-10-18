/* Entry: 0x00560b88; symbol: ___acrt_update_thread_locale_data; body bytes: 98 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    ___acrt_update_thread_locale_data
   
   Library: Visual Studio 2019 Release */

int ___acrt_update_thread_locale_data(void)

{
  int iVar1;
  int iVar2;
  undefined **ppuVar3;
  undefined4 *unaff_FS_OFFSET;
  undefined4 uStack_14;
  
  iVar2 = FUN_005579c2();
  if (((*(uint *)(iVar2 + 0x350) & DAT_005b29f8) != 0) &&
     (iVar1 = *(int *)(iVar2 + 0x4c), iVar1 != 0)) {
    *unaff_FS_OFFSET = uStack_14;
    return iVar1;
  }
  ___acrt_lock(4);
  ppuVar3 = __updatetlocinfoEx_nolock((int *)(iVar2 + 0x4c),DAT_005e5664);
  FUN_00560be8();
  if (ppuVar3 != (undefined **)0x0) {
    iVar2 = FUN_00560bf1();
    return iVar2;
  }
                    /* WARNING: Subroutine does not return */
  _abort();
}

