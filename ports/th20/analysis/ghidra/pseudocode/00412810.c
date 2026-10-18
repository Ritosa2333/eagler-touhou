/* Entry: 0x00412810; symbol: FUN_00412810; body bytes: 606 */

void __fastcall FUN_00412810(void *param_1)

{
  bool bVar1;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  int *piVar4;
  undefined4 *puVar5;
  int iVar6;
  code *pcVar7;
  undefined4 uVar8;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567c0d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00412550(&DAT_005c0240,0);
LAB_00412848:
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)((int)param_1 + 4),local_1c);
  local_8 = 0;
  piVar2 = (int *)FUN_00412540();
  do {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) {
      local_8 = 0xffffffff;
      FUN_00411b00(local_1c);
LAB_00412a4b:
      FUN_00412750(&DAT_005c0240,0);
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar4 = (int *)FUN_0040f160(p_Var3);
    if (*(int *)(*piVar4 + 8) != 0) {
      while (piVar4 = (int *)FUN_0040f160(p_Var3), (*(uint *)(*piVar4 + 4) >> 1 & 1) != 0) {
        if (*(int *)((int)param_1 + 0x34) != 0) {
switchD_00412940_caseD_7:
          piVar4 = (int *)FUN_0040f160(p_Var3);
          iVar6 = FUN_00411700(*piVar4);
          if (iVar6 != 0) {
            piVar4 = (int *)FUN_0040f160(p_Var3);
            pcVar7 = (code *)FUN_00411700(*piVar4);
            piVar4 = (int *)FUN_0040f160(p_Var3);
            uVar8 = FUN_00412710(*piVar4);
            (*pcVar7)(uVar8);
          }
          break;
        }
        FUN_00412750(&DAT_005c0240,0);
        piVar4 = (int *)FUN_0040f160(p_Var3);
        pcVar7 = *(code **)(*piVar4 + 8);
        piVar4 = (int *)FUN_0040f160(p_Var3);
        uVar8 = (*pcVar7)(*(undefined4 *)(*piVar4 + 0x28));
        FUN_00412550(&DAT_005c0240,0);
        switch(uVar8) {
        case 0:
          puVar5 = (undefined4 *)FUN_0040f160(p_Var3);
          FUN_00412400(param_1,(void *)*puVar5);
        default:
          goto switchD_00412940_caseD_1;
        case 2:
          break;
        case 3:
          local_8 = 0xffffffff;
          FUN_00411b00(local_1c);
          goto LAB_00412a4b;
        case 4:
          local_8 = 0xffffffff;
          FUN_00411b00(local_1c);
          goto LAB_00412a4b;
        case 5:
          local_8 = 0xffffffff;
          FUN_00411b00(local_1c);
          goto LAB_00412a4b;
        case 6:
          local_8 = 0xffffffff;
          FUN_00411b00(local_1c);
          goto LAB_00412848;
        case 7:
          goto switchD_00412940_caseD_7;
        case 8:
          local_8 = 0xffffffff;
          FUN_00411b00(local_1c);
          goto LAB_00412a4b;
        }
      }
switchD_00412940_caseD_1:
    }
    FUN_00411c30(local_1c);
  } while( true );
}

