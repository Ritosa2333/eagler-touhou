/* Entry: 0x0055356c; symbol: operator()<class_<lambda_69a2805e680e0e292e8ba93315fe43a8>,class_<lambda_f03950bc5685219e0bcd2087efbe011e>&,class_<lambda_03fcd07e894ec930e3f35da366ca99d6>_>; body bytes: 76 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    public: int __thiscall __crt_seh_guarded_call<int>::operator()<class
   <lambda_69a2805e680e0e292e8ba93315fe43a8>,class <lambda_f03950bc5685219e0bcd2087efbe011e> &,class
   <lambda_03fcd07e894ec930e3f35da366ca99d6> >(class <lambda_69a2805e680e0e292e8ba93315fe43a8>
   &&,class <lambda_f03950bc5685219e0bcd2087efbe011e> &,class
   <lambda_03fcd07e894ec930e3f35da366ca99d6> &&)
   
   Library: Visual Studio 2019 Release */

int __thiscall
__crt_seh_guarded_call<int>::
operator()<class_<lambda_69a2805e680e0e292e8ba93315fe43a8>,class_<lambda_f03950bc5685219e0bcd2087efbe011e>&,class_<lambda_03fcd07e894ec930e3f35da366ca99d6>_>
          (__crt_seh_guarded_call<int> *this,<lambda_69a2805e680e0e292e8ba93315fe43a8> *param_1,
          <lambda_f03950bc5685219e0bcd2087efbe011e> *param_2,
          <lambda_03fcd07e894ec930e3f35da366ca99d6> *param_3)

{
  int iVar1;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(*(int *)param_1);
  iVar1 = <lambda_f03950bc5685219e0bcd2087efbe011e>::operator()(param_2);
  FUN_005535bb();
  *unaff_FS_OFFSET = local_14;
  return iVar1;
}

