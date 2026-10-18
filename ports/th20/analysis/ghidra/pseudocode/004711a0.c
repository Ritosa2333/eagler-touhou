/* Entry: 0x004711a0; symbol: FUN_004711a0; body bytes: 105 */

int * __cdecl FUN_004711a0(int *param_1,int *param_2,int *param_3)

{
  bool bVar1;
  undefined4 local_c;
  undefined4 local_8;
  
  bVar1 = FUN_00471130(param_1,param_2);
  if (bVar1) {
    local_8 = param_2;
  }
  else {
    local_8 = param_1;
  }
  bVar1 = FUN_00471130(param_3,param_1);
  if (bVar1) {
    local_c = param_3;
  }
  else {
    local_c = local_8;
  }
  return local_c;
}

