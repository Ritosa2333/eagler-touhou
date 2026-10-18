/* Entry: 0x0055dec0; symbol: FUN_0055dec0; body bytes: 75 */

void __cdecl FUN_0055dec0(ushort *param_1,ushort *param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  do {
    uVar1 = (uint)*param_1;
    param_1 = param_1 + 1;
    uVar2 = (uint)*param_2;
    param_2 = param_2 + 1;
    uVar3 = uVar2;
    if (uVar1 != uVar2) {
      uVar3 = uVar1 + 0x20;
      if (0x19 < uVar1 - 0x41) {
        uVar3 = uVar1;
      }
      uVar1 = uVar3;
      uVar3 = uVar2 + 0x20;
      if (0x19 < uVar2 - 0x41) {
        uVar3 = uVar2;
      }
    }
  } while ((uVar1 == uVar3) && (uVar1 != 0));
  return;
}

