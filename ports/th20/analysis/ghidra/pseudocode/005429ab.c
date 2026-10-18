/* Entry: 0x005429ab; symbol: FUN_005429ab; body bytes: 46 */

undefined4 __cdecl FUN_005429ab(undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  if (DAT_005e4f50 == -1) {
    iVar1 = FUN_0055380f();
  }
  else {
    iVar1 = __register_onexit_function();
  }
  uVar2 = 0;
  if (iVar1 == 0) {
    uVar2 = param_1;
  }
  return uVar2;
}

