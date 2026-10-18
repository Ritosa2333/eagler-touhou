/* Entry: 0x00560880; symbol: FUN_00560880; body bytes: 63 */

ushort * __cdecl FUN_00560880(ushort *param_1,ushort *param_2)

{
  ushort uVar1;
  ushort *puVar2;
  
  uVar1 = *param_1;
  while( true ) {
    if (uVar1 == 0) {
      return (ushort *)0x0;
    }
    puVar2 = FUN_0054628e(param_2,uVar1);
    if (puVar2 != (ushort *)0x0) break;
    uVar1 = param_1[1];
    param_1 = param_1 + 1;
  }
  return param_1;
}

