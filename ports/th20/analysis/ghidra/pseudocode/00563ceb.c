/* Entry: 0x00563ceb; symbol: FUN_00563ceb; body bytes: 174 */

undefined4 __cdecl FUN_00563ceb(byte *param_1,uint param_2,undefined4 *param_3,int param_4)

{
  byte bVar1;
  undefined4 uVar2;
  byte bVar3;
  int iVar4;
  int iVar5;
  
  if (param_1 != (byte *)0x0) {
    if (param_2 != 0) {
      if ((param_2 & 0xffffff80) == 0) {
        *param_1 = (byte)param_2;
        return 1;
      }
      if ((param_2 & 0xfffff800) == 0) {
        bVar3 = 0xc0;
        iVar4 = 1;
        iVar5 = iVar4;
      }
      else if ((param_2 & 0xffff0000) == 0) {
        if ((0xd7ff < param_2) && (param_2 < 0xe000)) {
LAB_00563d88:
          uVar2 = FUN_00563dad(param_3,param_4);
          return uVar2;
        }
        iVar4 = 2;
        bVar3 = 0xe0;
        iVar5 = iVar4;
      }
      else {
        if (((param_2 & 0xffe00000) != 0) || (0x10ffff < param_2)) goto LAB_00563d88;
        iVar4 = 3;
        bVar3 = 0xf0;
        iVar5 = iVar4;
      }
      do {
        bVar1 = (byte)param_2;
        param_2 = param_2 >> 6;
        param_1[iVar4] = bVar1 & 0x3f | 0x80;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      *param_1 = bVar3 | (byte)param_2;
      uVar2 = FUN_00563d99(iVar5 + 1,param_3);
      return uVar2;
    }
    *param_1 = (byte)param_2;
  }
  *param_3 = 0;
  param_3[1] = 0;
  return 1;
}

