/* Entry: 0x0055ca88; symbol: __isindst; body bytes: 72 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    __isindst
   
   Library: Visual Studio 2019 Release */

int __cdecl __isindst(tm *_Time)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(6);
  bVar1 = FUN_0055c162(&_Time->tm_sec);
  FUN_0055cad3();
  *unaff_FS_OFFSET = local_14;
  return CONCAT31(extraout_var,bVar1);
}

