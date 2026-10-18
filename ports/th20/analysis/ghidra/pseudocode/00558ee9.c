/* Entry: 0x00558ee9; symbol: FUN_00558ee9; body bytes: 147 */

/* WARNING: Removing unreachable block (ram,0x00558f73) */

uint __cdecl FUN_00558ee9(uint *param_1,undefined4 param_2,uint param_3,short param_4,int param_5)

{
  uint uVar1;
  ulonglong uVar2;
  undefined8 uVar3;
  
  uVar2 = __aullshr((byte)param_4,param_1[1] & param_3 & 0xfffff);
  if (param_5 == 0) {
    uVar1 = CONCAT31((int3)(uVar2 >> 8),'\x01' - (((uint)uVar2 & 0xffff) < 8));
  }
  else {
    uVar1 = FUN_00559a20();
    if (uVar1 == 0) {
      uVar3 = FUN_0055862f(param_1,param_2,param_3,param_4);
      uVar1 = (uint)uVar3;
    }
    else {
      if (uVar1 == 0x200) {
        if ((short)uVar2 == 0) {
          return 0;
        }
        if ((int)param_1[1] < 0) {
          return 0;
        }
      }
      else {
        if (uVar1 != 0x100) {
          return uVar1 & 0xffffff00;
        }
        if ((short)uVar2 == 0) {
          return 0;
        }
        if (0 < (int)param_1[1]) {
          return 0;
        }
        if (-1 < (int)param_1[1]) {
          return 0;
        }
      }
      uVar1 = 1;
    }
  }
  return uVar1;
}

