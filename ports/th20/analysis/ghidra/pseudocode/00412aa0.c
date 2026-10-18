/* Entry: 0x00412aa0; symbol: FUN_00412aa0; body bytes: 585 */

void __fastcall FUN_00412aa0(void *param_1)

{
  code *pcVar1;
  bool bVar2;
  int *piVar3;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var4;
  int *piVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  void *this;
  int *unaff_FS_OFFSET;
  int local_24 [2];
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567c55;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00412550(&DAT_005c0240,0);
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)((int)param_1 + 0x1c),local_1c);
  local_8 = 0;
  piVar3 = (int *)FUN_00412540();
  do {
    bVar2 = FUN_00411bb0(local_1c,piVar3);
    if (!bVar2) {
      local_8 = 0xffffffff;
      FUN_00411b00(local_1c);
LAB_00412c4c:
      FUN_0040c080(local_24,8);
      FUN_00412280((void *)((int)param_1 + 0x1c),local_24);
      local_8 = 1;
      piVar3 = (int *)FUN_00412540();
      while (bVar2 = FUN_00411bb0(local_24,piVar3), bVar2) {
        this = (void *)FUN_0040c300(local_24);
        FUN_00412da0(this,0);
        FUN_00411c30(local_24);
      }
      local_8 = 0xffffffff;
      FUN_00411b00(local_24);
      FUN_00412750(&DAT_005c0240,0);
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    p_Var4 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar5 = (int *)FUN_0040f160(p_Var4);
    if (*(int *)(*piVar5 + 8) != 0) {
      while (piVar5 = (int *)FUN_0040f160(p_Var4), (*(uint *)(*piVar5 + 4) >> 1 & 1) != 0) {
        FUN_00412750(&DAT_005c0240,0);
        piVar5 = (int *)FUN_0040f160(p_Var4);
        pcVar1 = *(code **)(*piVar5 + 8);
        piVar5 = (int *)FUN_0040f160(p_Var4);
        uVar6 = FUN_00412710(*piVar5);
        uVar6 = (*pcVar1)(uVar6);
        FUN_00412550(&DAT_005c0240,0);
        switch(uVar6) {
        case 0:
          puVar7 = (undefined4 *)FUN_0040f160(p_Var4);
          FUN_00412400(param_1,(void *)*puVar7);
        default:
          goto switchD_00412bbf_caseD_1;
        case 2:
          break;
        case 3:
          local_8 = 0xffffffff;
          FUN_00411b00(local_1c);
          goto LAB_00412c4c;
        case 4:
          local_8 = 0xffffffff;
          FUN_00411b00(local_1c);
          goto LAB_00412c4c;
        case 5:
          local_8 = 0xffffffff;
          FUN_00411b00(local_1c);
          goto LAB_00412c4c;
        }
      }
switchD_00412bbf_caseD_1:
    }
    FUN_00411c30(local_1c);
  } while( true );
}

