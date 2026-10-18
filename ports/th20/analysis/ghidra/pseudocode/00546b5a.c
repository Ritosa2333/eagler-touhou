/* Entry: 0x00546b5a; symbol: FUN_00546b5a; body bytes: 930 */

void __cdecl
FUN_00546b5a(EHExceptionRecord *param_1,EHRegistrationNode *param_2,_CONTEXT *param_3,void *param_4,
            _s_FuncInfo *param_5,char param_6,int param_7,EHRegistrationNode *param_8)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  EHRegistrationNode *pEVar4;
  byte *pbVar5;
  _s_FuncInfo *p_Var6;
  int iVar7;
  int *piVar8;
  int *piVar9;
  int local_68 [4];
  undefined4 *local_58;
  undefined4 local_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  undefined4 uStack_48;
  int *local_44;
  _s_FuncInfo *local_40 [2];
  _s_FuncInfo *local_38;
  _s_FuncInfo *local_34;
  undefined4 local_30;
  int *local_2c;
  int local_28;
  int *local_24;
  int local_20;
  uint local_1c;
  undefined4 *local_18;
  int local_14;
  _s_FuncInfo *local_10;
  int local_c;
  _CONTEXT *local_8;
  
  local_14 = 0;
  local_1c = local_1c & 0xffffff00;
  local_c = __FrameHandler3::GetCurrentState(param_2,param_4,param_5);
  if ((local_c < -1) || (*(int *)(param_5 + 4) <= local_c)) goto LAB_00546ef7;
  if (((*(int *)param_1 != -0x1f928c9d) || (*(int *)(param_1 + 0x10) != 3)) ||
     ((((*(int *)(param_1 + 0x14) != 0x19930520 && (*(int *)(param_1 + 0x14) != 0x19930521)) &&
       (*(int *)(param_1 + 0x14) != 0x19930522)) || (iVar7 = 0, *(int *)(param_1 + 0x1c) != 0)))) {
    local_8 = param_3;
    goto LAB_00546ca8;
  }
  iVar2 = ___vcrt_getptd();
  if (*(int *)(iVar2 + 0x10) == 0) {
    return;
  }
  iVar2 = ___vcrt_getptd();
  param_1 = *(EHExceptionRecord **)(iVar2 + 0x10);
  iVar2 = ___vcrt_getptd();
  local_1c = CONCAT31(local_1c._1_3_,1);
  local_8 = *(_CONTEXT **)(iVar2 + 0x14);
  if ((param_1 == (EHExceptionRecord *)0x0) ||
     ((((*(int *)param_1 == -0x1f928c9d && (*(int *)(param_1 + 0x10) == 3)) &&
       ((*(int *)(param_1 + 0x14) == 0x19930520 ||
        ((*(int *)(param_1 + 0x14) == 0x19930521 || (*(int *)(param_1 + 0x14) == 0x19930522)))))) &&
      (*(int *)(param_1 + 0x1c) == 0)))) goto LAB_00546ef7;
  iVar2 = ___vcrt_getptd();
  if (*(int *)(iVar2 + 0x1c) == 0) {
LAB_00546ca8:
    local_34 = param_5;
    local_30 = 0;
    if (((*(int *)param_1 == -0x1f928c9d) && (*(int *)(param_1 + 0x10) == 3)) &&
       ((*(int *)(param_1 + 0x14) == 0x19930520 ||
        ((*(int *)(param_1 + 0x14) == 0x19930521 || (*(int *)(param_1 + 0x14) == 0x19930522)))))) {
      if (*(int *)(param_5 + 0xc) != 0) {
        FUN_00544394(&local_44,&local_34,local_c,param_4,(int)param_5,param_7);
        local_2c = local_44;
        local_10 = local_40[0];
        if (local_40[0] < local_38) {
          local_20 = (int)local_40[0] * 0x14;
          p_Var6 = local_40[0];
          do {
            iVar2 = local_c;
            piVar8 = (int *)(*(int *)(*local_2c + 0x10) + local_20);
            piVar9 = local_68;
            local_10 = p_Var6;
            for (iVar7 = 5; iVar7 != 0; iVar7 = iVar7 + -1) {
              *piVar9 = *piVar8;
              piVar8 = piVar8 + 1;
              piVar9 = piVar9 + 1;
            }
            if (((local_68[0] <= iVar2) && (iVar2 <= local_68[1])) &&
               (local_14 = 0, local_68[3] != 0)) {
              pbVar5 = *(byte **)(param_1 + 0x1c);
              local_28 = **(int **)(pbVar5 + 0xc);
              local_24 = *(int **)(pbVar5 + 0xc) + 1;
              local_18 = local_58;
              do {
                local_54 = *local_18;
                uStack_50 = local_18[1];
                uStack_4c = local_18[2];
                uStack_48 = local_18[3];
                piVar8 = local_24;
                p_Var6 = local_10;
                for (iVar7 = local_28; local_10 = p_Var6, 0 < iVar7; iVar7 = iVar7 + -1) {
                  iVar2 = FID_conflict____TypeMatch((byte *)&local_54,(byte *)*piVar8,pbVar5);
                  if (iVar2 != 0) {
                    FUN_00546ad9(param_1,param_2,local_8,param_4,param_5,(_s_HandlerType *)&local_54
                                 ,(_s_CatchableType *)*piVar8,local_68,param_7,param_8);
                    p_Var6 = local_10;
                    goto LAB_00546de0;
                  }
                  pbVar5 = *(byte **)(param_1 + 0x1c);
                  piVar8 = piVar8 + 1;
                  p_Var6 = local_10;
                }
                local_14 = local_14 + 1;
                local_18 = local_18 + 4;
              } while (local_14 != local_68[3]);
            }
LAB_00546de0:
            p_Var6 = p_Var6 + 1;
            local_20 = local_20 + 0x14;
            local_10 = p_Var6;
          } while (p_Var6 < local_38);
        }
      }
      if (param_6 != '\0') {
        FUN_005463e0((int *)param_1);
      }
      if (0x19930520 < (*(uint *)param_5 & 0x1fffffff)) {
        if (*(int *)(param_5 + 0x1c) != 0) {
          if ((*(uint *)(param_5 + 0x20) >> 2 & 1) != 0) goto LAB_00546e36;
          uVar3 = FUN_00547585((int)param_1,*(int **)(param_5 + 0x1c));
          if ((char)uVar3 != '\0') goto LAB_00546e82;
          goto LAB_00546eb7;
        }
        if (((*(uint *)(param_5 + 0x20) >> 2 & 1) != 0) && (param_7 == 0)) {
LAB_00546e36:
          iVar7 = ___vcrt_getptd();
          *(EHExceptionRecord **)(iVar7 + 0x10) = param_1;
          iVar7 = ___vcrt_getptd();
          *(_CONTEXT **)(iVar7 + 0x14) = local_8;
          goto LAB_00546e92;
        }
      }
    }
    else if (*(int *)(param_5 + 0xc) != 0) {
      if (param_6 != '\0') goto LAB_00546ef7;
      FindHandlerForForeignException<class___FrameHandler3>
                (param_1,param_2,local_8,param_4,param_5,local_c,param_7,param_8);
    }
LAB_00546e82:
    iVar7 = ___vcrt_getptd();
    if (*(int *)(iVar7 + 0x1c) == 0) {
      return;
    }
  }
  else {
    iVar2 = ___vcrt_getptd();
    local_10 = *(_s_FuncInfo **)(iVar2 + 0x1c);
    iVar2 = ___vcrt_getptd();
    *(undefined4 *)(iVar2 + 0x1c) = 0;
    uVar3 = FUN_00547585((int)param_1,(int *)local_10);
    p_Var6 = local_10;
    if ((char)uVar3 != '\0') goto LAB_00546ca8;
    param_5 = p_Var6;
    if (0 < *(int *)local_10) {
      do {
        bVar1 = type_info::operator==
                          (*(type_info **)(iVar7 + 4 + *(int *)(p_Var6 + 4)),
                           (type_info *)&PTR_PTR_005b4030);
        if (bVar1) goto LAB_00546e97;
        iVar7 = iVar7 + 0x10;
        local_14 = local_14 + 1;
      } while (local_14 < *(int *)p_Var6);
    }
LAB_00546e92:
    terminate();
LAB_00546e97:
    FUN_005463e0((int *)param_1);
    FUN_005471e7(local_40);
    FUN_00544a7c((int *)local_40,&DAT_005ac1fc);
LAB_00546eb7:
    iVar7 = ___vcrt_getptd();
    *(EHExceptionRecord **)(iVar7 + 0x10) = param_1;
    iVar7 = ___vcrt_getptd();
    *(_CONTEXT **)(iVar7 + 0x14) = local_8;
    pEVar4 = param_2;
    if (param_8 != (EHRegistrationNode *)0x0) {
      pEVar4 = param_8;
    }
    _UnwindNestedFrames(pEVar4,param_1);
    __FrameHandler3::FrameUnwindToEmptyState(param_2,param_4,param_5);
    uVar3 = FUN_00547642((int)param_5);
    FUN_005473f3(uVar3);
  }
LAB_00546ef7:
                    /* WARNING: Subroutine does not return */
  _abort();
}

