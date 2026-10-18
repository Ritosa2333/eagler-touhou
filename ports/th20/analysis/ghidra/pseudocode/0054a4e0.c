/* Entry: 0x0054a4e0; symbol: FUN_0054a4e0; body bytes: 90 */

bool __cdecl FUN_0054a4e0(uint param_1)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  if ((param_1 & 0x300) == param_1) {
    uVar1 = FUN_00559aa0();
    uVar2 = FUN_00559a30(0x300);
    uVar3 = FUN_00559a30(param_1 & 0x300);
    FUN_00559d30(~uVar2 & uVar1 | uVar3);
    uVar1 = FUN_00559a20();
    return uVar1 != param_1;
  }
  return true;
}

