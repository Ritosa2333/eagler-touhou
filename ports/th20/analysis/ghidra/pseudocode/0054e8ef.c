/* Entry: 0x0054e8ef; symbol: FUN_0054e8ef; body bytes: 177 */

/* WARNING: Removing unreachable block (ram,0x0054e940) */

void __cdecl FUN_0054e8ef(int *param_1,floating_point_value *param_2)

{
  byte bVar1;
  uint uVar2;
  byte *pbVar3;
  int iVar4;
  byte *pbVar5;
  uint uVar6;
  bool bVar7;
  ulonglong uVar8;
  uint local_14;
  uint uStack_10;
  
  iVar4 = 0x17;
  if (param_2[4] != (floating_point_value)0x0) {
    iVar4 = 0x34;
  }
  uStack_10 = 0;
  local_14 = 0;
  pbVar5 = (byte *)(param_1 + 2);
  iVar4 = iVar4 + *param_1;
  pbVar3 = (byte *)(param_1[1] + 8 + (int)param_1);
  for (; (pbVar5 != pbVar3 &&
         (uVar8 = FUN_0054efd4((int)param_2), CONCAT44(uStack_10,local_14) <= uVar8));
      uStack_10 = (uStack_10 << 4 | uVar2) + (uint)CARRY4(uVar6,(uint)bVar1)) {
    bVar1 = *pbVar5;
    pbVar5 = pbVar5 + 1;
    uVar2 = local_14 >> 0x1c;
    uVar6 = local_14 * 0x10;
    local_14 = uVar6 + bVar1;
    iVar4 = iVar4 + -4;
  }
  bVar7 = true;
  while ((pbVar5 != pbVar3 && (bVar7))) {
    bVar1 = *pbVar5;
    pbVar5 = pbVar5 + 1;
    bVar7 = bVar1 == 0;
  }
  FUN_0054c685(local_14,uStack_10,iVar4,SUB41(param_1[0xc2],0),bVar7,param_2);
  return;
}

