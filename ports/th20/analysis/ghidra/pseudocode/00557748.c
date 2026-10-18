/* Entry: 0x00557748; symbol: construct_ptd; body bytes: 153 */

/* Library Function - Single Match
    void __cdecl construct_ptd(struct __acrt_ptd * const,struct __crt_locale_data * * const)
   
   Library: Visual Studio 2019 Release */

void __cdecl construct_ptd(__acrt_ptd *param_1,__crt_locale_data **param_2)

{
  undefined4 local_18;
  __acrt_ptd **local_14;
  __acrt_ptd **local_10;
  __crt_locale_data ***local_c;
  __crt_seh_guarded_call<void> local_5;
  
  *(undefined4 *)(param_1 + 0x18) = 1;
  *(undefined **)param_1 = &DAT_00599348;
  *(undefined4 *)(param_1 + 0x350) = 1;
  *(undefined **)(param_1 + 0x48) = &DAT_005b2a00;
  *(undefined2 *)(param_1 + 0x6c) = 0x43;
  *(undefined2 *)(param_1 + 0x172) = 0x43;
  *(undefined4 *)(param_1 + 0x34c) = 0;
  local_14 = &param_1;
  local_c = (__crt_locale_data ***)0x5;
  local_18 = 5;
  __crt_seh_guarded_call<void>::
  operator()<class_<lambda_15ade71b0218206bbe3333a0c9b79046>,class_<lambda_da44e0f8b0f19ba52fefafb335991732>&,class_<lambda_207f2d024fc103971653565357d6cd41>_>
            (&local_5,(<lambda_15ade71b0218206bbe3333a0c9b79046> *)&local_18,
             (<lambda_da44e0f8b0f19ba52fefafb335991732> *)&local_14,
             (<lambda_207f2d024fc103971653565357d6cd41> *)&local_c);
  local_10 = &param_1;
  local_c = &param_2;
  local_18 = 4;
  local_14 = (__acrt_ptd **)0x4;
  __crt_seh_guarded_call<void>::
  operator()<class_<lambda_a7e850c220f1c8d1e6efeecdedd162c6>,class_<lambda_46720907175c18b6c9d2717bc0d2d362>&,class_<lambda_9048902d66e8d99359bc9897bbb930a8>_>
            (&local_5,(<lambda_a7e850c220f1c8d1e6efeecdedd162c6> *)&local_14,
             (<lambda_46720907175c18b6c9d2717bc0d2d362> *)&local_10,
             (<lambda_9048902d66e8d99359bc9897bbb930a8> *)&local_18);
  return;
}

