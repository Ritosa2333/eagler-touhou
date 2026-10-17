/* Entry: 0x00555a94; symbol: operator()<class_<lambda_4fdada1b837b2abbf20876fac97688ad>,class_<lambda_b57350f2640456a0859d250846f69caf>&,class_<lambda_eed5e4f92b5b7d55fa22c48c484aaa54>_>; body bytes: 65 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    public: void __thiscall __crt_seh_guarded_call<void>::operator()<class
   <lambda_4fdada1b837b2abbf20876fac97688ad>,class <lambda_b57350f2640456a0859d250846f69caf> &,class
   <lambda_eed5e4f92b5b7d55fa22c48c484aaa54> >(class <lambda_4fdada1b837b2abbf20876fac97688ad>
   &&,class <lambda_b57350f2640456a0859d250846f69caf> &,class
   <lambda_eed5e4f92b5b7d55fa22c48c484aaa54> &&)
   
   Library: Visual Studio 2019 Release */

void __thiscall
__crt_seh_guarded_call<void>::
operator()<class_<lambda_4fdada1b837b2abbf20876fac97688ad>,class_<lambda_b57350f2640456a0859d250846f69caf>&,class_<lambda_eed5e4f92b5b7d55fa22c48c484aaa54>_>
          (__crt_seh_guarded_call<void> *this,<lambda_4fdada1b837b2abbf20876fac97688ad> *param_1,
          <lambda_b57350f2640456a0859d250846f69caf> *param_2,
          <lambda_eed5e4f92b5b7d55fa22c48c484aaa54> *param_3)

{
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(*(int *)param_1);
  <lambda_b57350f2640456a0859d250846f69caf>::operator()(param_2);
  FUN_00555ad5();
  *unaff_FS_OFFSET = local_14;
  return;
}

