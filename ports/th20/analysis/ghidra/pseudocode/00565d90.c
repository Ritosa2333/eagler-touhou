/* Entry: 0x00565d90; symbol: FUN_00565d90; body bytes: 437 */

int __cdecl FUN_00565d90(ushort *param_1,ushort *param_2,int param_3,undefined4 *param_4)

{
  ushort uVar1;
  __acrt_ptd *p_Var2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int local_18;
  undefined *local_14;
  undefined *local_10;
  char local_c;
  uint local_8;
  
  if (param_1 == (ushort *)0x0) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
    return 0x7fffffff;
  }
  if (param_2 != (ushort *)0x0) {
    if (param_3 != 0) {
      local_c = '\0';
      if (param_4 == (undefined4 *)0x0) {
        if (DAT_005e55dc == 0) {
          local_14 = PTR_PTR_005b2840;
          local_10 = PTR_DAT_005b2844;
        }
        else {
          local_18 = FUN_005579c2();
          local_14 = *(undefined **)(local_18 + 0x4c);
          local_10 = *(undefined **)(local_18 + 0x48);
          ___acrt_update_locale_info(local_18,(int *)&local_14);
          FUN_0055856f(local_18,(int *)&local_10);
          if ((*(uint *)(local_18 + 0x350) & 2) == 0) {
            *(uint *)(local_18 + 0x350) = *(uint *)(local_18 + 0x350) | 2;
            local_c = '\x01';
          }
        }
      }
      else {
        local_14 = (undefined *)*param_4;
        local_10 = (undefined *)param_4[1];
      }
      if (*(int *)(local_14 + 0xa8) == 0) {
        iVar3 = FUN_00565cd0(param_1,param_2,param_3);
      }
      else {
        do {
          uVar1 = *param_1;
          uVar4 = (uint)uVar1;
          param_1 = param_1 + 1;
          if (uVar1 < 0x100) {
            if (((&DAT_00599142)[(uVar4 & 0xff) * 2] & 1) != 0) {
              uVar4 = (uint)*(byte *)((uVar4 & 0xff) + *(int *)(local_14 + 0x94));
            }
            uVar4 = uVar4 & 0xff;
          }
          else {
            uVar4 = FUN_005653d9(uVar4,&local_14);
            uVar4 = uVar4 & 0xffff;
          }
          uVar1 = *param_2;
          uVar5 = (uint)uVar1;
          param_2 = param_2 + 1;
          local_8 = uVar4;
          if (uVar1 < 0x100) {
            if (((&DAT_00599142)[(uVar5 & 0xff) * 2] & 1) != 0) {
              uVar5 = (uint)*(byte *)((uVar5 & 0xff) + *(int *)(local_14 + 0x94));
            }
            uVar5 = uVar5 & 0xff;
          }
          else {
            uVar5 = FUN_005653d9(uVar5,&local_14);
            uVar5 = uVar5 & 0xffff;
          }
          iVar3 = uVar4 - uVar5;
        } while (((iVar3 == 0) && (local_8 != 0)) && (param_3 = param_3 + -1, param_3 != 0));
      }
      if (local_c != '\0') {
        *(uint *)(local_18 + 0x350) = *(uint *)(local_18 + 0x350) & 0xfffffffd;
      }
      return iVar3;
    }
    return 0;
  }
  p_Var2 = FUN_005516c1();
  *(undefined4 *)p_Var2 = 0x16;
  FUN_005480bc();
  return 0x7fffffff;
}

