/* Entry: 0x0047d600; symbol: FUN_0047d600; body bytes: 863 */

void __fastcall FUN_0047d600(int param_1)

{
  bool bVar1;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  int iVar4;
  int *piVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005683cd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004776a0(DAT_005c0028,*(undefined4 *)(param_1 + 0x286d9c));
  *(undefined4 *)(param_1 + 0x44) = 0;
  *(undefined4 *)(param_1 + 0x28) = 0;
  *(undefined4 *)(param_1 + 0x24) = 0;
  *(undefined4 *)(param_1 + 0x20) = 0;
  *(undefined4 *)(param_1 + 0x1c) = 0;
  *(undefined4 *)(param_1 + 0x18) = 0;
  *(undefined4 *)(param_1 + 0x14) = 0;
  *(undefined4 *)(param_1 + 0x40) = 0;
  *(undefined4 *)(param_1 + 0x3c) = 0;
  *(undefined4 *)(param_1 + 0x38) = 0;
  *(undefined4 *)(param_1 + 0x34) = 0;
  *(undefined4 *)(param_1 + 0x30) = 0;
  *(undefined4 *)(param_1 + 0x2c) = 0;
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)(param_1 + 0x286d64),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  do {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) {
      local_8 = 0xffffffff;
      FUN_00411b00(local_1c);
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    if ((DAT_005ba828 == 0) || ((*(uint *)(DAT_005ba828 + 0xe8) >> 0xb & 1) == 0)) {
      piVar5 = (int *)FUN_0040f160(p_Var3);
      if ((*(uint *)(*piVar5 + 0x14) >> 8 & 1) != 0) {
        piVar5 = (int *)FUN_0040f160(p_Var3);
        if (*(int *)(*piVar5 + 0x50) == 2) {
          piVar5 = (int *)FUN_0040f160(p_Var3);
          bVar1 = FUN_004235c0((void *)(*piVar5 + 0x4f8),8);
          if (!bVar1) goto LAB_0047d7f6;
        }
        else {
LAB_0047d7f6:
          piVar5 = (int *)FUN_0040f160(p_Var3);
          if (*(int *)(*piVar5 + 0x50) != 1) goto LAB_0047d820;
        }
        puVar7 = (undefined4 *)FUN_0040f160(p_Var3);
        FUN_00484a20((void *)*puVar7,1);
        goto LAB_0047d838;
      }
LAB_0047d820:
      puVar7 = (undefined4 *)FUN_0040f160(p_Var3);
      iVar4 = FUN_0047d960((void *)*puVar7);
      if (iVar4 == 0) goto LAB_0047d838;
    }
    else {
LAB_0047d838:
      piVar5 = (int *)FUN_0040f160(p_Var3);
      if ((*(uint *)(*piVar5 + 0x14) >> 9 & 1) == 0) {
        piVar5 = (int *)FUN_0040f160(p_Var3);
        if (*(int *)(param_1 + 0x14 + *(int *)(*piVar5 + 0x44) * 4) == 0) {
          puVar7 = (undefined4 *)FUN_0040f160(p_Var3);
          uVar6 = *puVar7;
          piVar5 = (int *)FUN_0040f160(p_Var3);
          *(undefined4 *)(param_1 + 0x14 + *(int *)(*piVar5 + 0x44) * 4) = uVar6;
        }
        else {
          piVar5 = (int *)FUN_0040f160(p_Var3);
          if (*(int *)(param_1 + 0x2c + *(int *)(*piVar5 + 0x44) * 4) != 0) {
            uVar6 = FUN_0040c300((undefined4 *)p_Var3);
            piVar5 = (int *)FUN_0040f160(p_Var3);
            *(undefined4 *)(*(int *)(param_1 + 0x2c + *(int *)(*piVar5 + 0x44) * 4) + 0x60) = uVar6;
          }
        }
        puVar7 = (undefined4 *)FUN_0040f160(p_Var3);
        uVar6 = *puVar7;
        piVar5 = (int *)FUN_0040f160(p_Var3);
        *(undefined4 *)(param_1 + 0x2c + *(int *)(*piVar5 + 0x44) * 4) = uVar6;
        piVar5 = (int *)FUN_0040f160(p_Var3);
        *(undefined4 *)(*piVar5 + 0x60) = 0;
      }
      *(int *)(param_1 + 0x44) = *(int *)(param_1 + 0x44) + 1;
      piVar5 = (int *)FUN_0040f160(p_Var3);
      FUN_00423540((undefined4 *)(*piVar5 + 0x4f8));
    }
    FUN_00411c30(local_1c);
  } while( true );
}

