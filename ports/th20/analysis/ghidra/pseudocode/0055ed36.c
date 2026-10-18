/* Entry: 0x0055ed36; symbol: update_thread_multibyte_data_internal; body bytes: 129 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    struct __crt_multibyte_data * __cdecl update_thread_multibyte_data_internal(struct __acrt_ptd *
   const,struct __crt_multibyte_data * * const)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

__crt_multibyte_data * __cdecl
update_thread_multibyte_data_internal(__acrt_ptd *param_1,__crt_multibyte_data **param_2)

{
  int iVar1;
  __crt_multibyte_data *p_Var2;
  
  if (((*(uint *)(param_1 + 0x350) & DAT_005b29f8) == 0) || (*(int *)(param_1 + 0x4c) == 0)) {
    ___acrt_lock(5);
    p_Var2 = *(__crt_multibyte_data **)(param_1 + 0x48);
    if (p_Var2 != *param_2) {
      if (p_Var2 != (__crt_multibyte_data *)0x0) {
        LOCK();
        iVar1 = *(int *)p_Var2;
        *(int *)p_Var2 = iVar1 + -1;
        UNLOCK();
        if ((iVar1 + -1 == 0) && (p_Var2 != (__crt_multibyte_data *)&DAT_005b2a00)) {
          FUN_00557ba0(p_Var2);
        }
      }
      p_Var2 = *param_2;
      *(__crt_multibyte_data **)(param_1 + 0x48) = p_Var2;
      LOCK();
      *(int *)p_Var2 = *(int *)p_Var2 + 1;
      UNLOCK();
    }
    FUN_0055edb5();
  }
  else {
    p_Var2 = *(__crt_multibyte_data **)(param_1 + 0x48);
  }
  if (p_Var2 != (__crt_multibyte_data *)0x0) {
    p_Var2 = (__crt_multibyte_data *)FUN_0055edbe();
    return p_Var2;
  }
                    /* WARNING: Subroutine does not return */
  _abort();
}

