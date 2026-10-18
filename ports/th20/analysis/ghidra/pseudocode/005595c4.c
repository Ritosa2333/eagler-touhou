/* Entry: 0x005595c4; symbol: FUN_005595c4; body bytes: 171 */

undefined4 __cdecl FUN_005595c4(byte *param_1,uint param_2,int *param_3,int param_4)

{
  ushort uVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined8 local_c;
  
  piVar3 = &DAT_005e5678;
  if (param_3 != (int *)0x0) {
    piVar3 = param_3;
  }
  uVar1 = (short)param_2 + 0x2400;
  if (*piVar3 == 0) {
    if (0x3ff < uVar1) {
      if ((ushort)((short)param_2 + 0x2800U) < 0x400) {
        *piVar3 = ((param_2 & 0x27ff) + 0x40) * 0x400;
        return 0;
      }
      uVar2 = FUN_00563ceb(param_1,param_2 & 0xffff,piVar3,param_4);
      return uVar2;
    }
  }
  else if (uVar1 < 0x400) {
    local_c = 0;
    uVar2 = FUN_00563ceb(param_1,(param_2 & 0x23ff) + *piVar3,(undefined4 *)&local_c,param_4);
    uVar2 = FUN_00563d99(uVar2,piVar3);
    return uVar2;
  }
  uVar2 = FUN_00563dad(piVar3,param_4);
  return uVar2;
}

