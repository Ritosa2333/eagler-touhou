/* Entry: 0x00565cd0; symbol: FUN_00565cd0; body bytes: 103 */

int __cdecl FUN_00565cd0(ushort *param_1,ushort *param_2,int param_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  if (param_3 != 0) {
    do {
      uVar1 = (uint)*param_1;
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
    } while (((uVar1 - uVar3 == 0) && (uVar1 != 0)) &&
            (param_3 = param_3 + -1, param_1 = param_1 + 1, param_3 != 0));
    return uVar1 - uVar3;
  }
  return 0;
}

