/* Entry: 0x00555b26; symbol: operator()<class_<lambda_e5124f882df8998aaf41531e079ba474>,class_<lambda_3e16ef9562a7dcce91392c22ab16ea36>&,class_<lambda_e25ca0880e6ef98be67edffd8c599615>_>; body bytes: 98 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */
/* Library Function - Single Match
    public: void __thiscall __crt_seh_guarded_call<void>::operator()<class
   <lambda_e5124f882df8998aaf41531e079ba474>,class <lambda_3e16ef9562a7dcce91392c22ab16ea36> &,class
   <lambda_e25ca0880e6ef98be67edffd8c599615> >(class <lambda_e5124f882df8998aaf41531e079ba474>
   &&,class <lambda_3e16ef9562a7dcce91392c22ab16ea36> &,class
   <lambda_e25ca0880e6ef98be67edffd8c599615> &&)
   
   Library: Visual Studio 2019 Release */

void __thiscall
__crt_seh_guarded_call<void>::
operator()<class_<lambda_e5124f882df8998aaf41531e079ba474>,class_<lambda_3e16ef9562a7dcce91392c22ab16ea36>&,class_<lambda_e25ca0880e6ef98be67edffd8c599615>_>
          (__crt_seh_guarded_call<void> *this,<lambda_e5124f882df8998aaf41531e079ba474> *param_1,
          <lambda_3e16ef9562a7dcce91392c22ab16ea36> *param_2,
          <lambda_e25ca0880e6ef98be67edffd8c599615> *param_3)

{
  undefined **ppuVar1;
  int *piVar2;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  ___acrt_lock(*(int *)param_1);
  for (piVar2 = &DAT_005e5664; piVar2 != &DAT_005e5668; piVar2 = piVar2 + 1) {
    if ((undefined **)*piVar2 != &PTR_DAT_005b2788) {
      ppuVar1 = __updatetlocinfoEx_nolock(piVar2,&PTR_DAT_005b2788);
      *piVar2 = (int)ppuVar1;
    }
  }
  FUN_00555b88();
  *unaff_FS_OFFSET = local_14;
  return;
}

