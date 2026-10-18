/* Entry: 0x0052d270; symbol: FUN_0052d270; body bytes: 343 */

void __thiscall FUN_0052d270(void *this,undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  uint *puVar5;
  int iVar6;
  void *pvVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  
  piVar3 = (int *)FUN_0052eca0((int)this);
  piVar4 = (int *)Mysize((int)this);
  if ((uint)(*piVar3 + *piVar4) % 4 == 0) {
    puVar5 = (uint *)FUN_0052eba0((int)this);
    piVar3 = (int *)Mysize((int)this);
    if (*puVar5 <= *piVar3 + 4U >> 2) {
      FUN_0052e860(this,1);
    }
  }
  piVar3 = (int *)FUN_0052eba0((int)this);
  iVar1 = *piVar3;
  puVar5 = (uint *)FUN_0052eca0((int)this);
  *puVar5 = *puVar5 & iVar1 * 4 - 1U;
  piVar3 = (int *)FUN_0052eca0((int)this);
  piVar4 = (int *)Mysize((int)this);
  iVar1 = *piVar3;
  iVar2 = *piVar4;
  iVar6 = FUN_0052e800(this,iVar1 + iVar2);
  piVar3 = (int *)FUN_0052eb60((int)this);
  if (*(int *)(*piVar3 + iVar6 * 4) == 0) {
    pvVar7 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                   *)this);
    uVar8 = FUN_00497d70(pvVar7,4);
    piVar3 = (int *)FUN_0052eb60((int)this);
    *(undefined4 *)(*piVar3 + iVar6 * 4) = uVar8;
  }
  uVar8 = move<>(param_1);
  pvVar7 = (void *)FUN_004fbff0((int)this);
  uVar9 = FUN_004b6580(pvVar7,iVar1 + iVar2);
  uVar10 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  FUN_0051d890(uVar10,uVar9,uVar8);
  piVar3 = (int *)Mysize((int)this);
  *piVar3 = *piVar3 + 1;
  return;
}

