/* Entry: 0x00497900; symbol: FUN_00497900; body bytes: 344 */

void __thiscall FUN_00497900(void *this,int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  uint uVar4;
  int *piVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  char **ppcVar9;
  
  uVar2 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  piVar3 = (int *)((int)this + 4);
  iVar6 = *(int *)(param_1 + 4);
  iVar1 = *(int *)(param_1 + 8);
  uVar4 = (iVar1 - iVar6) / 0x2c;
  piVar5 = (int *)((int)this + 8);
  FUN_0040e5e0();
  if ((uint)((*(int *)((int)this + 0xc) - *piVar3) / 0x2c) < uVar4) {
    FUN_004844a0(this,uVar4);
    iVar6 = FUN_0047b120(iVar6,iVar1,*piVar3,uVar2);
    *piVar5 = iVar6;
  }
  else {
    uVar7 = (*piVar5 - *piVar3) / 0x2c;
    if (uVar7 < uVar4) {
      iVar8 = uVar7 * 0x2c + iVar6;
      FUN_00489c10(iVar6,iVar8,(undefined4 *)*piVar3);
      iVar6 = FUN_0047b120(iVar8,iVar1,*piVar5,uVar2);
      *piVar5 = iVar6;
    }
    else {
      ppcVar9 = (char **)(uVar4 * 0x2c + *piVar3);
      FUN_00489c10(iVar6,iVar1,(undefined4 *)*piVar3);
      std::_Adl_verify_range<char*,char_const*>(ppcVar9,(char **)*piVar5);
      *piVar5 = (int)ppcVar9;
    }
  }
  return;
}

