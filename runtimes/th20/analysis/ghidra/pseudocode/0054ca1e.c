/* Entry: 0x0054ca1e; symbol: FUN_0054ca1e; body bytes: 413 */

void __cdecl
FUN_0054ca1e(uint *param_1,uint param_2,bool param_3,byte param_4,floating_point_value *param_5)

{
  int iVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  longlong lVar8;
  ulonglong uVar9;
  longlong lVar10;
  byte local_10;
  
  iVar1 = 0x17;
  if (param_5[4] != (floating_point_value)0x0) {
    iVar1 = 0x34;
  }
  if (0x40 < param_2) {
    uVar7 = param_2 >> 5;
    uVar5 = param_2 & 0x1f;
    iVar6 = uVar7 - 2;
    iVar4 = iVar6 * 0x20;
    uVar3 = param_1[uVar7 - 1];
    if (uVar5 == 0) {
      lVar10 = CONCAT44(param_1[uVar7],param_1[uVar7 - 1]);
      local_10 = param_4 ^ 1;
      for (; iVar6 != 0; iVar6 = iVar6 + -1) {
        param_1 = param_1 + 1;
        bVar2 = 0;
        if (*param_1 == 0) {
          bVar2 = local_10;
        }
        local_10 = bVar2;
      }
    }
    else {
      local_10 = 1;
      bVar2 = (byte)uVar5;
      iVar4 = uVar5 + iVar4;
      lVar8 = __allshl(-bVar2 + 0x20,0);
      uVar9 = __aullshr(bVar2,0);
      lVar10 = __allshl(-bVar2 + 0x40,0);
      lVar10 = lVar8 + uVar9 + lVar10;
      if ((param_4 != 0) || ((uVar3 & (1 << bVar2) - 1U) != 0)) {
        local_10 = 0;
      }
      for (; iVar6 != 0; iVar6 = iVar6 + -1) {
        param_1 = param_1 + 1;
        bVar2 = 0;
        if (*param_1 == 0) {
          bVar2 = local_10;
        }
        local_10 = bVar2;
      }
    }
    FUN_0054c685((uint)lVar10,(uint)((ulonglong)lVar10 >> 0x20),iVar1 + iVar4,param_3,local_10,
                 param_5);
    return;
  }
  if (*param_1 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = param_1[1];
    if (1 < *param_1) {
      uVar5 = param_1[2];
      goto LAB_0054ca5c;
    }
  }
  uVar5 = 0;
LAB_0054ca5c:
  FUN_0054c685(uVar3,uVar5,iVar1,param_3,param_4 ^ 1,param_5);
  return;
}

