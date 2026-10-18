/* Entry: 0x0055862f; symbol: FUN_0055862f; body bytes: 178 */

undefined8 __cdecl FUN_0055862f(uint *param_1,undefined4 param_2,uint param_3,short param_4)

{
  uint uVar1;
  undefined1 uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  ulonglong uVar7;
  
  uVar6 = (uint)param_4;
  uVar3 = param_1[1];
  uVar1 = *param_1;
  uVar7 = __aullshr((byte)param_4,uVar3 & param_3 & 0xfffff);
  uVar5 = (uint)(uVar7 >> 0x20);
  uVar4 = (uint)uVar7 & 0xffff;
  if ((ushort)uVar7 < 9) {
    if ((ushort)uVar7 < 8) {
      uVar3 = (uint)uVar7 & 0xff00;
      goto LAB_005586dc;
    }
    uVar2 = 0;
    uVar4 = 1 << (uVar6 & 0x1f);
    uVar5 = 0;
    if (0x1f < uVar6) {
      uVar5 = uVar4;
    }
    uVar4 = uVar4 ^ uVar5;
    if (0x3f < uVar6) {
      uVar5 = uVar4;
    }
    uVar5 = uVar4 - 1 & uVar1 | (uVar5 - 1) + (uint)(uVar4 != 0) & uVar3 & 0xfffff;
    uVar4 = uVar6;
    if (uVar5 == 0) {
      if (param_4 == 0x30) {
        uVar7 = (ulonglong)uVar3 & 0xffffffff7ff00000;
        if ((uVar3 & 0x7ff00000) != 0) {
          uVar2 = 1;
          uVar7 = (ulonglong)uVar3 & 0xffffffff7ff00000;
        }
      }
      else {
        uVar7 = __aullshr((byte)param_4,uVar3 >> 4 & param_3 & 0xffff);
        uVar2 = (undefined1)uVar7;
      }
      uVar5 = (uint)(uVar7 >> 0x20);
      uVar3 = CONCAT31((int3)(uVar7 >> 8),uVar2) & 0xffffff01;
      goto LAB_005586dc;
    }
  }
  uVar3 = CONCAT31((int3)(uVar4 >> 8),1);
LAB_005586dc:
  return CONCAT44(uVar5,uVar3);
}

