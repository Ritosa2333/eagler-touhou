/* Entry: 0x0046ffd0; symbol: FUN_0046ffd0; body bytes: 81 */

int __cdecl FUN_0046ffd0(uint param_1,uint param_2)

{
  ulonglong uVar1;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_8;
  
  local_8 = 0;
  while( true ) {
    uVar1 = FUN_0046fc00(param_1,param_2);
    local_14 = (uint)uVar1;
    if (param_1 + local_14 * -5 != 0) break;
    param_1 = local_14;
    local_10 = (uint)(uVar1 >> 0x20);
    param_2 = local_10;
    local_8 = local_8 + 1;
  }
  return local_8;
}

