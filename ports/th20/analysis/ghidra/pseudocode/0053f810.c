/* Entry: 0x0053f810; symbol: FUN_0053f810; body bytes: 140 */

int __thiscall FUN_0053f810(void *this,undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int *piVar5;
  
  piVar5 = (int *)((int)this + 8);
  uVar2 = move<>(param_1);
  uVar3 = move<>(*piVar5);
  uVar4 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  FUN_0053fa90(uVar4,uVar3,uVar2);
  FUN_00414b60();
  iVar1 = *piVar5;
  *piVar5 = *piVar5 + 8;
  return iVar1;
}

