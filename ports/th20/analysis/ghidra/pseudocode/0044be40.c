/* Entry: 0x0044be40; symbol: FUN_0044be40; body bytes: 581 */

void __thiscall
FUN_0044be40(void *this,int param_1,int param_2,undefined4 param_3,int *param_4,undefined4 param_5,
            int param_6)

{
  char cVar1;
  byte bVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint *local_2c;
  int local_28;
  int local_24;
  uint *local_1c;
  uint local_18;
  uint *local_14;
  int *local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  piVar3 = (int *)FUN_00414580((void *)((int)this + 0x6000730),param_1);
  if (*(int *)(*(int *)(*piVar3 + 0x58) + param_2 * 0x18) != 0) {
    FUN_004455c0((int)this);
    piVar3 = (int *)FUN_00414580((void *)((int)this + 0x6000730),param_1);
    piVar3 = *(int **)(*(int *)(*piVar3 + 0x58) + param_2 * 0x18);
    iVar4 = (**(code **)(*piVar3 + 0x48))(piVar3,0,&local_10);
    if (iVar4 == 0) {
      iVar4 = D3DXLoadSurfaceFromSurface(local_10,0,param_4,param_3,0,param_5,2,0);
      if (iVar4 == 0) {
        if (param_6 == 1) {
          iVar4 = (**(code **)(*local_10 + 0x34))(local_10,&local_18,param_4,0);
          if (iVar4 != 0) {
            (**(code **)(*local_10 + 8))(local_10);
            goto LAB_0044c073;
          }
          local_2c = local_14;
          for (local_24 = 0; local_24 < param_4[2] - *param_4; local_24 = local_24 + 1) {
            local_1c = local_2c;
            FUN_00425cc0(&local_c);
            for (local_28 = 0; local_28 < param_4[3] - param_4[1]; local_28 = local_28 + 1) {
              local_c._1_1_ = (byte)(*local_1c >> 8);
              bVar2 = local_c._1_1_;
              uVar6 = (uint)local_c._1_1_;
              local_c = *local_1c;
              uVar5 = FUN_00449d20(0x5ba4c4);
              *(byte *)((int)local_1c + 1) =
                   bVar2 - (char)((ulonglong)(uVar6 * (uVar5 & 0xff)) / 0x300);
              cVar1 = (char)local_c;
              uVar6 = local_c & 0xff;
              uVar5 = FUN_00449d20(0x5ba4c4);
              *(char *)local_1c = cVar1 - (char)((ulonglong)((uVar5 & 0xff) * uVar6) / 0x300);
              bVar2 = local_c._2_1_;
              uVar6 = (uint)local_c._2_1_;
              uVar5 = FUN_00449d20(0x5ba4c4);
              *(byte *)((int)local_1c + 2) =
                   bVar2 - (char)((ulonglong)((uVar5 & 0xff) * uVar6) / 0x500);
              *(char *)((int)local_1c + 3) = -1;
              local_1c = local_1c + 1;
            }
            local_2c = (uint *)((int)local_2c + (local_18 & 0xfffffffc));
          }
          (**(code **)(*local_10 + 0x38))(local_10);
        }
        (**(code **)(*local_10 + 8))(local_10);
      }
      else {
        (**(code **)(*local_10 + 8))(local_10);
      }
    }
  }
LAB_0044c073:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

