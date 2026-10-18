/* Entry: 0x00484320; symbol: FUN_00484320; body bytes: 100 */

void __thiscall FUN_00484320(void *this,uint param_1)

{
  void *pvVar1;
  int iVar2;
  uint *puVar3;
  
  puVar3 = &param_1;
  pvVar1 = (void *)FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)this);
  iVar2 = FUN_0047a840(pvVar1,puVar3);
  *(int *)((int)this + 4) = iVar2;
  *(int *)((int)this + 8) = iVar2;
  *(uint *)((int)this + 0xc) = param_1 * 0x2c + iVar2;
  return;
}

