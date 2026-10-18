/* Entry: 0x004c38d0; symbol: FUN_004c38d0; body bytes: 815 */

void __thiscall FUN_004c38d0(void *this,int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  bool bVar5;
  byte bVar6;
  int *piVar7;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var8;
  int *piVar9;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int *unaff_FS_OFFSET;
  float10 fVar10;
  float fVar11;
  int local_34;
  int local_2c [2];
  uint local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  undefined4 local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_00569a8d;
  local_1c = *unaff_FS_OFFSET;
  local_24 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  *unaff_FS_OFFSET = (int)&local_1c;
  if (param_1 == 1) {
    local_34 = 0xc;
  }
  else {
    local_34 = 6;
  }
  FUN_0044f3d0(DAT_005c0028,local_34,*(int *)((int)this + 0x49c894));
  FUN_0040c080(local_2c,8);
  FUN_00412280((void *)((int)this + 0x49c818),local_2c);
  local_14 = 0;
  piVar7 = (int *)FUN_00412540();
  while (bVar5 = FUN_00411bb0(local_2c,piVar7), bVar5) {
    p_Var8 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_2c);
    piVar9 = (int *)FUN_0040f160(p_Var8);
    if (*(int *)(*piVar9 + 0xc24) != 0) {
      piVar9 = (int *)FUN_0040f160(p_Var8);
      bVar6 = FUN_00445ab0(*piVar9 + 0x18);
      if (((CONCAT31(extraout_var,bVar6) != 0) &&
          (piVar9 = (int *)FUN_0040f160(p_Var8), *(int *)(*piVar9 + 0xc34) < 1)) && (param_1 != 0))
      {
        piVar9 = (int *)FUN_0040f160(p_Var8);
        iVar1 = *piVar9;
        uVar2 = *(undefined4 *)(iVar1 + 0xbe4);
        uVar3 = *(undefined4 *)(iVar1 + 0xbe8);
        uVar4 = *(undefined4 *)(iVar1 + 0xbec);
        piVar9 = (int *)FUN_0040f160(p_Var8);
        iVar1 = *piVar9;
        *(undefined4 *)(iVar1 + 0x5d4) = uVar2;
        *(undefined4 *)(iVar1 + 0x5d8) = uVar3;
        *(undefined4 *)(iVar1 + 0x5dc) = uVar4;
        piVar9 = (int *)FUN_0040f160(p_Var8);
        iVar1 = *piVar9;
        uVar2 = *(undefined4 *)(iVar1 + 0xbe4);
        uVar3 = *(undefined4 *)(iVar1 + 0xbe8);
        uVar4 = *(undefined4 *)(iVar1 + 0xbec);
        piVar9 = (int *)FUN_0040f160(p_Var8);
        iVar1 = *piVar9;
        *(undefined4 *)(iVar1 + 3000) = uVar2;
        *(undefined4 *)(iVar1 + 0xbbc) = uVar3;
        *(undefined4 *)(iVar1 + 0xbc0) = uVar4;
        piVar9 = (int *)FUN_0040f160(p_Var8);
        fVar10 = FUN_004c4b70(*piVar9 + 0x18);
        if (DAT_00570db4 <= (float)fVar10) {
          piVar9 = (int *)FUN_0040f160(p_Var8);
          FUN_0044c570((void *)(*piVar9 + 0x18));
          piVar9 = (int *)FUN_0040f160(p_Var8);
          *(undefined4 *)(*piVar9 + 0xc2c) = 0;
        }
        else {
          piVar9 = (int *)FUN_0040f160(p_Var8);
          bVar6 = FUN_00445ab0(*piVar9 + 0x5fc);
          if (CONCAT31(extraout_var_00,bVar6) != 0) {
            piVar9 = (int *)FUN_0040f160(p_Var8);
            fVar10 = FUN_004c4b70(*piVar9 + 0x5fc);
            fVar11 = (float)fVar10 + DAT_0056d7bc;
            piVar9 = (int *)FUN_0040f160(p_Var8);
            FUN_00470b40((void *)(*piVar9 + 0x5fc),DAT_0056d7bc);
            if (fVar11 < DAT_0056cd94) {
              piVar9 = (int *)FUN_0040f160(p_Var8);
              FUN_00470b60((void *)(*piVar9 + 0x5fc),
                           (char)(int)((fVar11 / DAT_0056cd94) * DAT_0056cd9c));
            }
            else {
              piVar9 = (int *)FUN_0040f160(p_Var8);
              FUN_00470b60((void *)(*piVar9 + 0x5fc),0xff);
            }
            piVar9 = (int *)FUN_0040f160(p_Var8);
            FUN_0044c570((void *)(*piVar9 + 0x5fc));
          }
          piVar9 = (int *)FUN_0040f160(p_Var8);
          *(undefined4 *)(*piVar9 + 0xc2c) = 1;
        }
      }
    }
    FUN_00411c30(local_2c);
  }
  local_14 = 0xffffffff;
  FUN_00411b00(local_2c);
  *unaff_FS_OFFSET = local_1c;
  FUN_005429ee(local_24 ^ (uint)&stack0xfffffff0);
  return;
}

