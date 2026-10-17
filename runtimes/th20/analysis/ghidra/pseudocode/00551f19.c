/* Entry: 0x00551f19; symbol: operator()<class_<lambda_f5952838e36297a1e2e9d6c748429792>,class_<lambda_2116bde18c9e5f34230805ea4a4660ed>&,class_<lambda_3c7107de1cc98bf330a3031f42c322a3>_>; body bytes: 76 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    public: char * __thiscall __crt_seh_guarded_call<char *>::operator()<class
   <lambda_f5952838e36297a1e2e9d6c748429792>,class <lambda_2116bde18c9e5f34230805ea4a4660ed> &,class
   <lambda_3c7107de1cc98bf330a3031f42c322a3> >(class <lambda_f5952838e36297a1e2e9d6c748429792>
   &&,class <lambda_2116bde18c9e5f34230805ea4a4660ed> &,class
   <lambda_3c7107de1cc98bf330a3031f42c322a3> &&)
   
   Library: Visual Studio 2019 Release */

char * __thiscall
__crt_seh_guarded_call<char*>::
operator()<class_<lambda_f5952838e36297a1e2e9d6c748429792>,class_<lambda_2116bde18c9e5f34230805ea4a4660ed>&,class_<lambda_3c7107de1cc98bf330a3031f42c322a3>_>
          (__crt_seh_guarded_call<char*> *this,<lambda_f5952838e36297a1e2e9d6c748429792> *param_1,
          <lambda_2116bde18c9e5f34230805ea4a4660ed> *param_2,
          <lambda_3c7107de1cc98bf330a3031f42c322a3> *param_3)

{
  char *pcVar1;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(*(int *)param_1);
  pcVar1 = <lambda_2116bde18c9e5f34230805ea4a4660ed>::operator()(param_2);
  FUN_00551f68();
  *unaff_FS_OFFSET = local_14;
  return pcVar1;
}

