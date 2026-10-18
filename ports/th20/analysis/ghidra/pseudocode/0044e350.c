/* Entry: 0x0044e350; symbol: FUN_0044e350; body bytes: 1432 */

void FUN_0044e350(undefined4 *param_1,uint param_2,int param_3,int param_4,int *param_5)

{
  char **ppcVar1;
  int iVar2;
  int *local_90;
  int local_8c;
  undefined1 local_80 [24];
  int local_68;
  int local_64;
  undefined4 local_60;
  uint local_5c;
  uint local_58;
  float local_54;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  float local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (param_5 == (int *)0x0) {
    ppcVar1 = (char **)FUN_0044baf0((int)(param_1 + 1));
    std::_Adl_verify_range<char*,char_const*>((char **)&DAT_0056e4fc,ppcVar1);
  }
  else if (*param_5 == 8) {
    *(int **)(param_1[0x16] + 0x10 + param_2 * 0x18) = param_5;
    if ((char)param_5[8] == '\0') {
      if (*(char *)((int)param_5 + param_5[4]) == '@') {
        if (((char *)((int)param_5 + param_5[4]))[1] == 'R') {
          *(undefined2 *)((int)param_5 + 10) = (undefined2)DAT_005b87f8;
          *(undefined2 *)(param_5 + 3) = (undefined2)DAT_005b87fc;
          FUN_0044c150(param_2 * 0x18 + param_1[0x16],DAT_005b87f8,DAT_005b87fc);
        }
        else {
          iVar2 = FUN_0044c0b0(param_2 * 0x18 + param_1[0x16],(uint)*(ushort *)((int)param_5 + 10),
                               (uint)*(ushort *)(param_5 + 3),(uint)*(ushort *)((int)param_5 + 0xe))
          ;
          param_1[0x19] = param_1[0x19] + iVar2;
        }
      }
      else {
        iVar2 = FUN_0044e8f0((undefined4 *)(param_2 * 0x18 + param_1[0x16]),
                             (uint)*(ushort *)((int)param_5 + 0xe),0,
                             (uint)*(ushort *)((int)param_5 + 10),(uint)*(ushort *)(param_5 + 3),
                             (int)(short)param_5[5],(int)*(short *)((int)param_5 + 0x16),
                             (uint)*(ushort *)(param_5 + 10),(uint)*(ushort *)((int)param_5 + 0x2a))
        ;
        if (iVar2 < 0) {
          ppcVar1 = (char **)FUN_0044baf0((int)(param_1 + 1));
          std::_Adl_verify_range<char*,char_const*>((char **)&DAT_0056e560,ppcVar1);
          goto LAB_0044e8dc;
        }
        param_1[0x19] = param_1[0x19] + iVar2;
      }
    }
    else {
      iVar2 = FUN_0044d9d0((undefined4 *)(param_2 * 0x18 + param_1[0x16]),(int)param_5 + param_5[7],
                           (uint)*(ushort *)((int)param_5 + 0xe),
                           (uint)*(ushort *)((int)param_5 + 10),(uint)*(ushort *)(param_5 + 3));
      if (iVar2 < 0) {
        ppcVar1 = (char **)FUN_0044baf0((int)(param_1 + 1));
        std::_Adl_verify_range<char*,char_const*>((char **)&DAT_0056e560,ppcVar1);
        goto LAB_0044e8dc;
      }
      param_1[0x19] = param_1[0x19] + iVar2;
    }
    (**(code **)(**(int **)(param_1[0x16] + param_2 * 0x18) + 0x44))
              (*(undefined4 *)(param_1[0x16] + param_2 * 0x18),0,local_80);
    FUN_004488f0(&local_60);
    local_90 = param_5 + 0x10;
    for (local_8c = 0; local_8c < (int)(uint)*(ushort *)(param_5 + 1); local_8c = local_8c + 1) {
      iVar2 = *local_90;
      local_60 = *param_1;
      local_58 = FUN_0044d000(param_1,param_2);
      local_5c = param_2;
      local_10 = (float)((double)local_68 + *(double *)(&DAT_0056cdd0 + (local_68 >> 0x1f) * -8)) /
                 (float)*(ushort *)((int)param_5 + 10);
      local_c = (float)((double)local_64 + *(double *)(&DAT_0056cdd0 + (local_64 >> 0x1f) * -8)) /
                (float)*(ushort *)(param_5 + 3);
      local_54 = *(float *)((int)param_5 + iVar2 + 4) * local_10;
      local_50 = *(float *)((int)param_5 + iVar2 + 8) * local_c;
      local_4c = (*(float *)((int)param_5 + iVar2 + 4) + *(float *)((int)param_5 + iVar2 + 0xc)) *
                 local_10;
      local_48 = (*(float *)((int)param_5 + iVar2 + 8) + *(float *)((int)param_5 + iVar2 + 0x10)) *
                 local_c;
      local_40 = (float)((double)local_68 + *(double *)(&DAT_0056cdd0 + (local_68 >> 0x1f) * -8));
      local_44 = (float)((double)local_64 + *(double *)(&DAT_0056cdd0 + (local_64 >> 0x1f) * -8));
      local_3c = *(undefined4 *)((int)param_5 + iVar2 + 0x14);
      local_38 = *(undefined4 *)((int)param_5 + iVar2 + 0x18);
      local_34 = *(undefined4 *)((int)param_5 + iVar2 + 0x1c);
      local_30 = *(undefined4 *)((int)param_5 + iVar2 + 0x20);
      local_2c = *(undefined4 *)((int)param_5 + iVar2 + 0x24);
      FUN_004506f0(param_1,param_3,&local_60);
      param_3 = param_3 + 1;
      local_90 = local_90 + 1;
    }
    for (local_8c = 0; local_8c < (int)(uint)*(ushort *)((int)param_5 + 6); local_8c = local_8c + 1)
    {
      *(int *)(param_1[0x15] + param_4 * 4) = (int)param_5 + local_90[1];
      param_4 = param_4 + 1;
      local_90 = local_90 + 2;
    }
  }
  else {
    FUN_0044baf0((int)(param_1 + 1));
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_0056e53c);
  }
LAB_0044e8dc:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

