/* Entry: 0x004a2c50; symbol: FUN_004a2c50; body bytes: 224 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall
FUN_004a2c50(void *this,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_1,undefined4 param_2)

{
  uint uVar1;
  char cVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  uint *puVar5;
  int *unaff_FS_OFFSET;
  byte local_12;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005696ed;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00467fa0(this,(uint)local_12,param_2);
  local_8 = 0;
  puVar3 = (undefined4 *)FUN_0040f160(param_1);
  puVar4 = (undefined4 *)
           FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  cVar2 = FUN_0048bc80(puVar4,puVar3);
  if (cVar2 == '\0') {
    uVar1 = *(uint *)(param_1 + 0x14);
    puVar5 = Myptr((undefined4 *)(param_1 + 4));
    FUN_00468180(this,puVar5,uVar1);
  }
  else {
    FUN_0040ec90();
    FUN_0046ed90(this,(int)param_1);
  }
  *unaff_FS_OFFSET = local_10;
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

