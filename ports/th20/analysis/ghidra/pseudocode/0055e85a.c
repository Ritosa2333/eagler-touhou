/* Entry: 0x0055e85a; symbol: operator()<class_<lambda_ceb1ee4838e85a9d631eb091e2fbe199>,class_<lambda_ae742caa10f662c28703da3d2ea5e57e>&,class_<lambda_cd08b5d6af4937fe54fc07d0c9bf6b37>_>; body bytes: 65 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    public: void __thiscall __crt_seh_guarded_call<void>::operator()<class
   <lambda_ceb1ee4838e85a9d631eb091e2fbe199>,class <lambda_ae742caa10f662c28703da3d2ea5e57e> &,class
   <lambda_cd08b5d6af4937fe54fc07d0c9bf6b37> >(class <lambda_ceb1ee4838e85a9d631eb091e2fbe199>
   &&,class <lambda_ae742caa10f662c28703da3d2ea5e57e> &,class
   <lambda_cd08b5d6af4937fe54fc07d0c9bf6b37> &&)
   
   Library: Visual Studio 2019 Release */

void __thiscall
__crt_seh_guarded_call<void>::
operator()<class_<lambda_ceb1ee4838e85a9d631eb091e2fbe199>,class_<lambda_ae742caa10f662c28703da3d2ea5e57e>&,class_<lambda_cd08b5d6af4937fe54fc07d0c9bf6b37>_>
          (__crt_seh_guarded_call<void> *this,<lambda_ceb1ee4838e85a9d631eb091e2fbe199> *param_1,
          <lambda_ae742caa10f662c28703da3d2ea5e57e> *param_2,
          <lambda_cd08b5d6af4937fe54fc07d0c9bf6b37> *param_3)

{
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(*(int *)param_1);
  <lambda_ae742caa10f662c28703da3d2ea5e57e>::operator()(param_2);
  FUN_0055e89b();
  *unaff_FS_OFFSET = local_14;
  return;
}

