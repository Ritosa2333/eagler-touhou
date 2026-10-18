/* Entry: 0x00555940; symbol: FUN_00555940; body bytes: 57 */

undefined4 __cdecl FUN_00555940(undefined4 param_1)

{
  undefined4 uVar1;
  byte bVar2;
  code *pcVar3;
  
  bVar2 = (byte)DAT_005b25c0 & 0x1f;
  pcVar3 = (code *)((DAT_005e55d4 ^ DAT_005b25c0) >> bVar2 |
                   (DAT_005e55d4 ^ DAT_005b25c0) << 0x20 - bVar2);
  if (pcVar3 == (code *)0x0) {
    return 0;
  }
  (*(code *)PTR_guard_check_icall_0056c37c)(param_1);
  uVar1 = (*pcVar3)();
  return uVar1;
}

