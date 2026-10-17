/* Entry: 0x00557802; symbol: destroy_ptd; body bytes: 205 */

/* Library Function - Single Match
    void __cdecl destroy_ptd(struct __acrt_ptd * const)
   
   Library: Visual Studio 2019 Release */

void __cdecl destroy_ptd(__acrt_ptd *param_1)

{
  undefined4 local_14;
  __acrt_ptd **local_10;
  undefined4 local_c;
  __crt_seh_guarded_call<void> local_5;
  
  if (*(undefined **)param_1 != &DAT_00599348) {
    FUN_00557ba0(*(undefined **)param_1);
  }
  FUN_00557ba0(*(LPVOID *)(param_1 + 0x3c));
  FUN_00557ba0(*(LPVOID *)(param_1 + 0x30));
  FUN_00557ba0(*(LPVOID *)(param_1 + 0x34));
  FUN_00557ba0(*(LPVOID *)(param_1 + 0x38));
  FUN_00557ba0(*(LPVOID *)(param_1 + 0x28));
  FUN_00557ba0(*(LPVOID *)(param_1 + 0x2c));
  FUN_00557ba0(*(LPVOID *)(param_1 + 0x40));
  FUN_00557ba0(*(LPVOID *)(param_1 + 0x44));
  FUN_00557ba0(*(LPVOID *)(param_1 + 0x360));
  local_10 = &param_1;
  local_c = 5;
  local_14 = 5;
  __crt_seh_guarded_call<void>::
  operator()<class_<lambda_38edbb1296d33220d7e4dd0ed76b244a>,class_<lambda_5ce1d447e08cb34b2473517608e21441>&,class_<lambda_fb385d3da700c9147fc39e65dd577a8c>_>
            (&local_5,(<lambda_38edbb1296d33220d7e4dd0ed76b244a> *)&local_14,
             (<lambda_5ce1d447e08cb34b2473517608e21441> *)&local_10,
             (<lambda_fb385d3da700c9147fc39e65dd577a8c> *)&local_c);
  local_10 = &param_1;
  local_14 = 4;
  local_c = 4;
  __crt_seh_guarded_call<void>::
  operator()<class_<lambda_6affb1475c98b40b75cdec977db92e3c>,class_<lambda_b8d4b9c228a6ecc3f80208dbb4b4a104>&,class_<lambda_608742c3c92a14382c1684fc64f96c88>_>
            (&local_5,(<lambda_6affb1475c98b40b75cdec977db92e3c> *)&local_c,
             (<lambda_b8d4b9c228a6ecc3f80208dbb4b4a104> *)&local_10,
             (<lambda_608742c3c92a14382c1684fc64f96c88> *)&local_14);
  return;
}

