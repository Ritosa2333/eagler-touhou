/* Entry: 0x0047a8d0; symbol: FUN_0047a8d0; body bytes: 470 */

void __thiscall FUN_0047a8d0(void *this,undefined4 *param_1,uint param_2)

{
  char cVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined4 *puVar4;
  int iVar5;
  uint uVar6;
  undefined4 *puVar7;
  int *piVar8;
  char **ppcVar9;
  undefined4 *local_18;
  
  uVar2 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  piVar3 = (int *)((int)this + 4);
  piVar8 = (int *)((int)this + 8);
  FUN_0040e5e0();
  if ((uint)((*(int *)((int)this + 0xc) - *piVar3) / 0x2c) < param_2) {
    FUN_004844a0(this,param_2);
    iVar5 = *piVar3;
    puVar4 = (undefined4 *)move<>(&param_1);
    iVar5 = FUN_0047b050(*puVar4,param_2,iVar5,uVar2);
    *piVar8 = iVar5;
  }
  else {
    uVar6 = (*piVar8 - *piVar3) / 0x2c;
    if (uVar6 < param_2) {
      cVar1 = FUN_00411170();
      if (cVar1 == '\0') {
        FUN_0047ab60(param_1,uVar6);
        param_1 = param_1 + uVar6 * 0xb;
      }
      else {
        for (local_18 = (undefined4 *)*piVar3; local_18 != (undefined4 *)*piVar8;
            local_18 = local_18 + 0xb) {
          puVar4 = param_1;
          puVar7 = local_18;
          for (iVar5 = 0xb; iVar5 != 0; iVar5 = iVar5 + -1) {
            *puVar7 = *puVar4;
            puVar4 = puVar4 + 1;
            puVar7 = puVar7 + 1;
          }
          param_1 = param_1 + 0xb;
        }
      }
      iVar5 = *piVar8;
      puVar4 = (undefined4 *)move<>(&param_1);
      iVar5 = FUN_0047b050(*puVar4,param_2 - uVar6,iVar5,uVar2);
      *piVar8 = iVar5;
    }
    else {
      ppcVar9 = (char **)(param_2 * 0x2c + *piVar3);
      puVar4 = (undefined4 *)*piVar3;
      puVar7 = (undefined4 *)move<>(&param_1);
      FUN_0047abb0((undefined4 *)*puVar7,param_2,puVar4);
      std::_Adl_verify_range<char*,char_const*>(ppcVar9,(char **)*piVar8);
      *piVar8 = (int)ppcVar9;
    }
  }
  return;
}

