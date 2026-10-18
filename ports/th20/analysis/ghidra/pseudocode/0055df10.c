/* Entry: 0x0055df10; symbol: FUN_0055df10; body bytes: 70 */

void __cdecl FUN_0055df10(ushort *param_1,ushort *param_2)

{
  if (DAT_005e55dc != 0) {
    FUN_0055df60(param_1,param_2,(undefined4 *)0x0);
    return;
  }
  if ((param_1 != (ushort *)0x0) && (param_2 != (ushort *)0x0)) {
    FUN_0055dec0(param_1,param_2);
    return;
  }
  FUN_00555720((int)param_1,(int)param_2);
  return;
}

