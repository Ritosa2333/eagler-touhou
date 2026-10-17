/* Entry: 0x00555750; symbol: FUN_00555750; body bytes: 120 */

void __cdecl FUN_00555750(byte *param_1,byte *param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  if (DAT_005e55dc != 0) {
    FUN_005557d0(param_1,param_2,(undefined4 *)0x0);
    return;
  }
  if ((param_1 != (byte *)0x0) && (param_2 != (byte *)0x0)) {
    do {
      uVar2 = (uint)*param_1;
      param_1 = param_1 + 1;
      uVar3 = (uint)*param_2;
      param_2 = param_2 + 1;
      if (uVar2 != uVar3) {
        uVar1 = uVar2 + 0x20;
        if (0x19 < uVar2 - 0x41) {
          uVar1 = uVar2;
        }
        uVar2 = uVar1;
        if (uVar3 - 0x41 < 0x1a) {
          uVar3 = uVar3 + 0x20;
        }
      }
    } while ((uVar2 == uVar3) && (uVar2 != 0));
    return;
  }
  FUN_00555720((int)param_1,(int)param_2);
  return;
}

