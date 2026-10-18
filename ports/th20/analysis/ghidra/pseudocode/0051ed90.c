/* Entry: 0x0051ed90; symbol: FUN_0051ed90; body bytes: 1168 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0051ed90(int param_1)

{
  float fVar1;
  float fVar2;
  undefined4 uVar3;
  uint uVar4;
  void *extraout_ECX;
  void *extraout_ECX_00;
  int *unaff_FS_OFFSET;
  float10 fVar5;
  float fVar6;
  float fVar7;
  int local_4c;
  int local_48;
  float *local_44;
  float local_40;
  undefined1 local_38 [12];
  float local_2c;
  float local_28;
  undefined4 local_20;
  float local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056aebd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar3 = FUN_0040c310(&DAT_005c0240,9);
  FUN_0040b730(&local_20,uVar3);
  local_8 = 0;
  if (*(int *)(param_1 + 0x5924) != 0) {
    local_44 = *(float **)(*(int *)(param_1 + 0x5924) + 0x14);
    FUN_0049d5b0(*(void **)(param_1 + 0x5924),0,0.0,(float)DAT_005b87f8,(float)DAT_005b87fc);
    FUN_00422e10(&local_2c);
    FUN_00422dd0(local_38,(float)DAT_005c538c + (float)DAT_005b87f8 / DAT_0056c8d0,
                 (float)DAT_005c5390 + (float)DAT_005b87fc / DAT_0056c8d0,0);
    FUN_00447de0(local_1c);
    fVar2 = DAT_0056f280;
    fVar1 = DAT_0056cdb0;
    for (local_48 = 0; local_48 < **(int **)(param_1 + 0x5924); local_48 = local_48 + 1) {
      for (local_4c = 0; local_4c < *(int *)(*(int *)(param_1 + 0x5924) + 4);
          local_4c = local_4c + 1) {
        fVar6 = *local_44;
        fVar7 = local_44[1];
        fVar5 = (float10)FUN_004fe850(*(float *)(param_1 + 0x592c) - (fVar6 - fVar7));
        if ((float)fVar5 <= fVar1) {
          local_40 = (fVar1 - (float)fVar5) / fVar1;
        }
        else {
          local_40 = 0.0;
        }
        FUN_00452f20(local_1c,(DAT_0056e0f0 * DAT_0056c8d0 *
                              ((*(float *)(param_1 + 0x592c) - (fVar6 - fVar7)) / DAT_00570560)) /
                              DAT_0056e048);
        fVar5 = FUN_004292e0(local_1c);
        fVar5 = FUN_00439820(extraout_ECX,(float)fVar5);
        fVar6 = (float)((uint)(float)fVar5 ^ _DAT_0056d7d0);
        fVar5 = FUN_004292e0(local_1c);
        fVar5 = FUN_004397c0(extraout_ECX_00,(float)fVar5);
        fVar7 = (float)((uint)(float)fVar5 ^ _DAT_0056d7d0);
        if ((((local_48 != 0) && (local_48 != **(int **)(param_1 + 0x5924) + -1)) && (local_4c != 0)
            ) && (local_4c != *(int *)(*(int *)(param_1 + 0x5924) + 4) + -1)) {
          local_2c = fVar6 * fVar2 * local_40;
          local_28 = fVar7 * fVar2 * local_40;
          FUN_004296e0(local_44,&local_2c);
        }
        fVar7 = (float)((uint)fVar6 ^ _DAT_0056d7d0) + fVar7;
        uVar4 = FUN_0051e100(*(int *)(param_1 + 0x5938),local_40,fVar7);
        *(char *)((int)local_44 + 0x12) = (char)uVar4;
        uVar4 = FUN_0051e100(*(int *)(param_1 + 0x5934),local_40,fVar7);
        *(char *)((int)local_44 + 0x11) = (char)uVar4;
        uVar4 = FUN_0051e100(*(int *)(param_1 + 0x5930),local_40,fVar7);
        *(char *)(local_44 + 4) = (char)uVar4;
        *(undefined1 *)((int)local_44 + 0x13) = *(undefined1 *)(param_1 + 0x593c);
        local_44 = local_44 + 7;
      }
    }
    *(float *)(param_1 + 0x592c) = *(float *)(param_1 + 0x592c) + DAT_0056d7bc;
    if (_DAT_00575698 < *(float *)(param_1 + 0x592c)) {
      uVar4 = FUN_00423ee0(0x5ba4c4);
      *(float *)(param_1 + 0x592c) =
           *(float *)(param_1 + 0x592c) -
           ((float)((double)(uVar4 % 1000) + _DAT_0056cdd0) + _DAT_0057569c);
    }
    FUN_0049ddc0(*(int **)(param_1 + 0x5924));
  }
  local_8 = 0xffffffff;
  FUN_0040b900(&local_20);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

