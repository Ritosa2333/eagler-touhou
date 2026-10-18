/* Entry: 0x005642db; symbol: common_getenv_s<>; body bytes: 83 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Multiple Matches With Same Base Name
    int __cdecl common_getenv_s<char>(unsigned int * const,char * const,unsigned int,char const *
   const)
    int __cdecl common_getenv_s<wchar_t>(unsigned int * const,wchar_t * const,unsigned int,wchar_t
   const * const)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

undefined4 __cdecl common_getenv_s<>(uint *param_1,short *param_2,uint param_3,wchar_t *param_4)

{
  undefined4 uVar1;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(0xb);
  uVar1 = FUN_0056433a(param_1,param_2,param_3,param_4);
  FUN_00564331();
  *unaff_FS_OFFSET = local_14;
  return uVar1;
}

