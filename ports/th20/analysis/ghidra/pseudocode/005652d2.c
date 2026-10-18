/* Entry: 0x005652d2; symbol: FUN_005652d2; body bytes: 263 */

void __cdecl FUN_005652d2(uint *param_1,undefined4 *param_2,uint param_3,int *param_4,int param_5)

{
  uint uVar1;
  ushort *puVar2;
  int iVar3;
  uint *puVar4;
  ushort *puVar5;
  undefined4 local_14;
  uint local_10 [2];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  puVar5 = (ushort *)*param_2;
  puVar2 = puVar5;
  if (param_1 != (uint *)0x0) {
    do {
      local_14 = puVar2;
      puVar4 = local_10;
      if (3 < param_3) {
        puVar4 = param_1;
      }
      uVar1 = FUN_005595c4((byte *)puVar4,(uint)*puVar5,param_4,param_5);
      if (uVar1 == 0xffffffff) {
        *param_2 = local_14;
        goto LAB_005653ca;
      }
      if (puVar4 != param_1) {
        puVar2 = local_14;
        if (param_3 < uVar1) goto LAB_0056537e;
        FUN_00543e20(param_1,puVar4,uVar1);
      }
      if (uVar1 != 0) {
        if (*(char *)((int)param_1 + (uVar1 - 1)) == '\0') goto LAB_00565379;
        local_14 = puVar5 + 1;
      }
      puVar5 = puVar5 + 1;
      param_3 = param_3 - uVar1;
      param_1 = (uint *)((int)param_1 + uVar1);
      puVar2 = local_14;
    } while( true );
  }
  while ((iVar3 = FUN_005595c4((byte *)local_10,(uint)*puVar5,param_4,param_5), iVar3 != -1 &&
         ((iVar3 == 0 || (*(char *)((int)local_10 + iVar3 + -1) != '\0'))))) {
    puVar5 = puVar5 + 1;
  }
LAB_005653ca:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
LAB_00565379:
  puVar2 = (ushort *)0x0;
LAB_0056537e:
  *param_2 = puVar2;
  goto LAB_005653ca;
}

