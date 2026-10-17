/* Entry: 0x0055ccbf; symbol: FUN_0055ccbf; body bytes: 76 */

undefined4 __cdecl FUN_0055ccbf(char param_1)

{
  undefined4 *local_24;
  char *local_20;
  undefined4 *local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  __crt_seh_guarded_call<void> local_5;
  
  local_10 = 0;
  local_24 = &local_10;
  local_c = 0;
  local_20 = &param_1;
  local_1c = &local_c;
  local_14 = 8;
  local_18 = 8;
  __crt_seh_guarded_call<void>::
  operator()<class_<lambda_2cc53f568c5a2bb6f192f930a45d44ea>,class_<lambda_ab61a845afdef5b7c387490eaf3616ee>&,class_<lambda_c2ffc0b7726aa6be21d5f0026187e748>_>
            (&local_5,(<lambda_2cc53f568c5a2bb6f192f930a45d44ea> *)&local_18,
             (<lambda_ab61a845afdef5b7c387490eaf3616ee> *)&local_24,
             (<lambda_c2ffc0b7726aa6be21d5f0026187e748> *)&local_14);
  if (param_1 != '\0') {
    local_c = local_10;
  }
  return local_c;
}

