/* Entry: 0x0055ca2b; symbol: ___tzset; body bytes: 84 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    ___tzset
   
   Library: Visual Studio 2019 Release */

void __cdecl ___tzset(void)

{
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  if (DAT_005e58b4 == 0) {
    ___acrt_lock(6);
    if (DAT_005e58b4 == 0) {
      FUN_0055c974();
      LOCK();
      DAT_005e58b4 = DAT_005e58b4 + 1;
      UNLOCK();
    }
    FUN_0055ca7f();
  }
  *unaff_FS_OFFSET = local_14;
  return;
}

