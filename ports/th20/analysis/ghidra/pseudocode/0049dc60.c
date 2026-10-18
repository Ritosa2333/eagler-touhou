/* Entry: 0x0049dc60; symbol: FUN_0049dc60; body bytes: 143 */

int * __thiscall FUN_0049dc60(void *this,int param_1,int param_2,uint *param_3,int param_4)

{
  int *piVar1;
  ulonglong uVar2;
  int local_c;
  int *local_8;
  
  piVar1 = (int *)FUN_00449490((void *)((int)this + 0x103c),0);
  local_c = 0;
  local_8 = piVar1;
  while( true ) {
    if (0x3ff < local_c) {
      return piVar1;
    }
    if (*local_8 < 0) break;
    local_8 = local_8 + 0x12;
    local_c = local_c + 1;
  }
  *local_8 = param_2;
  local_8[3] = param_1;
  local_8[1] = (int)param_3;
  local_8[2] = param_4;
  uVar2 = FUN_00543e20((uint *)(local_8 + 4),param_3,0x38);
  return (int *)uVar2;
}

