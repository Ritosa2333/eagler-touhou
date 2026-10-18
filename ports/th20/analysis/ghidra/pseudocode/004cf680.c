/* Entry: 0x004cf680; symbol: FUN_004cf680; body bytes: 694 */

void __fastcall FUN_004cf680(void *param_1)

{
  bool bVar1;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  int *piVar4;
  undefined4 *puVar5;
  int iVar6;
  uint *puVar7;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569bcd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004776a0(DAT_005c0028,*(undefined4 *)((int)param_1 + 0x50));
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)((int)param_1 + 0x10),local_1c);
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
    piVar4 = (int *)FUN_0040f160(p_Var3);
    if ((*(uint *)(*piVar4 + 0x1c) >> 1 & 3) == 0) {
LAB_004cf7c9:
      piVar4 = (int *)FUN_0040f160(p_Var3);
      if (*(int *)(*piVar4 + 0x20) == 1) {
        puVar5 = (undefined4 *)FUN_0040f160(p_Var3);
        (**(code **)(*(int *)*puVar5 + 0x24))();
        piVar4 = (int *)FUN_0040f160(p_Var3);
        FUN_004cfb80(param_1,*piVar4);
        piVar4 = (int *)FUN_0040f160(p_Var3);
        FUN_004c7f20(piVar4);
      }
      else {
        piVar4 = (int *)FUN_0040f160(p_Var3);
        if ((*(uint *)(*piVar4 + 0x1c) >> 3 & 1) == 0) {
          puVar5 = (undefined4 *)FUN_0040f160(p_Var3);
          iVar6 = (**(code **)(*(int *)*puVar5 + 0x1c))();
          if (iVar6 == 0) {
            piVar4 = (int *)FUN_0040f160(p_Var3);
            FUN_00423540((undefined4 *)(*piVar4 + 0x28));
            piVar4 = (int *)FUN_0040f160(p_Var3);
            *(uint *)(*piVar4 + 0x1c) = *(uint *)(*piVar4 + 0x1c) | 1;
          }
          else {
            puVar5 = (undefined4 *)FUN_0040f160(p_Var3);
            (**(code **)(*(int *)*puVar5 + 0x24))();
            piVar4 = (int *)FUN_0040f160(p_Var3);
            FUN_004cfb80(param_1,*piVar4);
            piVar4 = (int *)FUN_0040f160(p_Var3);
            FUN_004c7f20(piVar4);
          }
        }
        else {
          puVar5 = (undefined4 *)FUN_0040f160(p_Var3);
          (**(code **)(*(int *)*puVar5 + 0x4c))(1);
        }
      }
    }
    else {
      piVar4 = (int *)FUN_0040f160(p_Var3);
      puVar7 = (uint *)(*piVar4 + 0x1c);
      *puVar7 = *puVar7 & 0xfffffff9 | ((*puVar7 >> 1 & 3) + 1 & 3) << 1;
      piVar4 = (int *)FUN_0040f160(p_Var3);
      if ((*(uint *)(*piVar4 + 0x1c) >> 1 & 3) < 2) goto LAB_004cf7c9;
      puVar5 = (undefined4 *)FUN_0040f160(p_Var3);
      (**(code **)(*(int *)*puVar5 + 0x24))();
      piVar4 = (int *)FUN_0040f160(p_Var3);
      FUN_004cfb80(param_1,*piVar4);
      piVar4 = (int *)FUN_0040f160(p_Var3);
      FUN_004c7f20(piVar4);
    }
    FUN_00411c30(local_1c);
  } while( true );
}

