/* Entry: 0x004fd8b0; symbol: FUN_004fd8b0; body bytes: 155 */

uint __cdecl FUN_004fd8b0(uint param_1,uint param_2,int param_3,ulonglong *param_4)

{
  uint3 uVar2;
  uint uVar1;
  int iVar3;
  longlong lVar4;
  ulonglong uVar5;
  
  iVar3 = param_3 + -0x433;
  uVar2 = (uint3)(param_1 >> 8);
  if (iVar3 < 1) {
    if (iVar3 < -0x34) {
      uVar1 = (uint)uVar2 << 8;
    }
    else {
      lVar4 = __allshl(-(char)iVar3,0);
      uVar1 = param_1 & (int)lVar4 - 1U;
      if (uVar1 == 0 &&
          ((param_2 | 0x100000) & (int)((ulonglong)lVar4 >> 0x20) - (uint)((int)lVar4 == 0)) == 0) {
        uVar5 = __aullshr(-(char)iVar3,param_2 | 0x100000);
        *param_4 = uVar5;
        *(undefined4 *)(param_4 + 1) = 0;
        uVar1 = CONCAT31((int3)(uVar5 >> 8),1);
      }
      else {
        uVar1 = uVar1 & 0xffffff00;
      }
    }
  }
  else {
    uVar1 = (uint)uVar2 << 8;
  }
  return uVar1;
}

