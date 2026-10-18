/* Entry: 0x00422cb0; symbol: FUN_00422cb0; body bytes: 133 */

uint __cdecl FUN_00422cb0(uint param_1)

{
  longlong lVar1;
  ulonglong uVar2;
  undefined4 local_14;
  undefined4 local_c;
  undefined4 local_8;
  
  local_8 = (uint)((ulonglong)param_1 * 0xbc8f >> 0x20);
  uVar2 = __aullshr(0x1f,local_8);
  local_c = (uint)((ulonglong)param_1 * 0xbc8f);
  lVar1 = uVar2 + (local_c & 0x7fffffff);
  local_8 = (uint)((ulonglong)lVar1 >> 0x20);
  local_c = (uint)lVar1;
  if ((local_8 == 0) && (local_c < 0x7fffffff)) {
    local_14 = local_c;
  }
  else {
    local_14 = local_c + 0x80000001;
  }
  return local_14;
}

