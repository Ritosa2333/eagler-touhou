/* Entry: 0x00551430; symbol: FUN_00551430; body bytes: 108 */

undefined4 __cdecl FUN_00551430(ulonglong *param_1)

{
  ulonglong uVar1;
  uint uVar2;
  undefined4 local_c;
  undefined4 uStack_8;
  
  uVar1 = *param_1;
  uVar2 = (uint)(uVar1 >> 0x34) & 0x7ff;
  local_c = (int)uVar1;
  if (uVar2 == 0x7ff) {
    if (local_c == 0 && (uVar1 & 0xfffff00000000) == 0) {
      return 1;
    }
    return 2;
  }
  if (uVar2 != 0) {
    return 0xffffffff;
  }
  if (local_c == 0 && (uVar1 & 0x7fffffff00000000) == 0) {
    return 0;
  }
  return 0xfffffffe;
}

