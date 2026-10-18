/* Entry: 0x0055df60; symbol: FUN_0055df60; body bytes: 425 */

int __cdecl FUN_0055df60(ushort *param_1,ushort *param_2,undefined4 *param_3)

{
  ushort uVar1;
  ushort uVar2;
  __acrt_ptd *p_Var3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int local_18;
  undefined *local_14;
  undefined *local_10;
  char local_c;
  uint local_8;
  
  if (param_1 == (ushort *)0x0) {
    p_Var3 = FUN_005516c1();
    *(undefined4 *)p_Var3 = 0x16;
    FUN_005480bc();
    return 0x7fffffff;
  }
  if (param_2 != (ushort *)0x0) {
    local_c = '\0';
    if (param_3 == (undefined4 *)0x0) {
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
      local_14 = (undefined *)*param_3;
      local_10 = (undefined *)param_3[1];
    }
    if (*(int *)(local_14 + 0xa8) == 0) {
      iVar4 = FUN_0055dec0(param_1,param_2);
    }
    else {
      do {
        uVar1 = *param_1;
        uVar5 = (uint)uVar1;
        param_1 = param_1 + 1;
        uVar2 = *param_2;
        uVar6 = (uint)uVar2;
        local_8 = uVar6;
        if (uVar5 != uVar6) {
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
          if (uVar5 != local_8) {
            if (uVar2 < 0x100) {
              if (((&DAT_00599142)[(local_8 & 0xff) * 2] & 1) != 0) {
                local_8 = (uint)*(byte *)((local_8 & 0xff) + *(int *)(local_14 + 0x94));
              }
              local_8 = local_8 & 0xff;
            }
            else {
              local_8 = FUN_005653d9(uVar6,&local_14);
              local_8 = local_8 & 0xffff;
            }
          }
        }
        iVar4 = uVar5 - local_8;
      } while ((iVar4 == 0) && (param_2 = param_2 + 1, uVar5 != 0));
    }
    if (local_c != '\0') {
      *(uint *)(local_18 + 0x350) = *(uint *)(local_18 + 0x350) & 0xfffffffd;
    }
    return iVar4;
  }
  p_Var3 = FUN_005516c1();
  *(undefined4 *)p_Var3 = 0x16;
  FUN_005480bc();
  return 0x7fffffff;
}

