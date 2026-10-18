/* Entry: 0x00565d40; symbol: FUN_00565d40; body bytes: 73 */

void __cdecl FUN_00565d40(ushort *param_1,ushort *param_2,int param_3)

{
  if (DAT_005e55dc != 0) {
    FUN_00565d90(param_1,param_2,param_3,(undefined4 *)0x0);
    return;
  }
  if ((param_1 != (ushort *)0x0) && (param_2 != (ushort *)0x0)) {
    FUN_00565cd0(param_1,param_2,param_3);
    return;
  }
  FUN_00555720((int)param_1,(int)param_2);
  return;
}

