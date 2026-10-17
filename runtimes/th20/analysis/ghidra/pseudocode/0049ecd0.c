/* Entry: 0x0049ecd0; symbol: FUN_0049ecd0; body bytes: 696 */

undefined4 * __thiscall FUN_0049ecd0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  default_delete<class_std::_Facet_base> *local_48;
  undefined4 local_44;
  default_delete<class_std::_Facet_base> *local_40;
  uint *local_3c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_38;
  uint *local_34;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_30;
  int local_2c;
  int local_28;
  default_delete<class_std::_Facet_base> *local_24;
  default_delete<class_std::_Facet_base> *local_20;
  default_delete<class_std::_Facet_base> *local_1c;
  default_delete<class_std::_Facet_base> *local_18;
  undefined4 *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569573;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *(undefined4 *)this = 0;
  local_14 = (undefined4 *)this;
  FUN_00422d90((undefined4 *)((int)this + 4));
  FUN_00422d90(local_14 + 5);
  FUN_00422d90(local_14 + 9);
  FUN_0049eb90(local_14 + 0xd);
  FUN_0049eb90(local_14 + 0x12);
  FUN_0040bc20(local_14 + 0x18,0xc,2,FUN_00422e10);
  local_14[0x1e] = 0;
  local_14[0x1f] = 0;
  local_14[0x20] = 0;
  local_14[0x21] = 0xffffff;
  local_14[0x22] = 0xffffff;
  local_14[0x23] = 0;
  local_14[0x24] = 0;
  local_14[0x25] = 0;
  local_14[0x26] = 0;
  FUN_0049ebe0(local_14 + 0x27);
  FUN_0040b780(local_14 + 0x37);
  local_8 = 0;
  local_14[0x3b] = 0;
  local_28 = 0;
  local_30 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)(local_14 + 0xd);
  local_18 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(local_30);
  local_40 = (default_delete<class_std::_Facet_base> *)FUN_004a0da0((int)local_30);
  for (; local_18 != local_40; local_18 = local_18 + 4) {
    local_1c = local_18;
    local_44 = FUN_00470670();
    puVar1 = FUN_00450c70(&local_50,(uint *)&DAT_0056fe1c,local_28 + 0x4c,-1,(undefined4 *)0x0);
    *(undefined4 *)local_1c = *puVar1;
    iVar2 = FUN_0044ced0((undefined4 *)local_1c);
    *(undefined1 *)(iVar2 + 0x578) = 0x10;
    iVar2 = FUN_0044ced0((undefined4 *)local_1c);
    *(undefined1 *)(iVar2 + 0x579) = 0x10;
    iVar2 = FUN_0044ced0((undefined4 *)local_1c);
    local_34 = (uint *)(iVar2 + 0x49c);
    *local_34 = *local_34 & 0xfffffbff;
    local_28 = local_28 + 1;
  }
  local_2c = 0;
  local_38 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)(local_14 + 0x12);
  local_20 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(local_38);
  local_48 = (default_delete<class_std::_Facet_base> *)FUN_004a0da0((int)local_38);
  for (; local_20 != local_48; local_20 = local_20 + 4) {
    local_24 = local_20;
    local_4c = FUN_00470670();
    puVar1 = FUN_00450c70(&local_54,(uint *)&DAT_0056fe1c,local_2c + 0x51,-1,(undefined4 *)0x0);
    *(undefined4 *)local_24 = *puVar1;
    iVar2 = FUN_0044ced0((undefined4 *)local_24);
    *(undefined1 *)(iVar2 + 0x578) = 0x10;
    iVar2 = FUN_0044ced0((undefined4 *)local_24);
    *(undefined1 *)(iVar2 + 0x579) = 0x10;
    iVar2 = FUN_0044ced0((undefined4 *)local_24);
    local_3c = (uint *)(iVar2 + 0x49c);
    *local_3c = *local_3c & 0xfffffbff;
    local_2c = local_2c + 1;
  }
  *local_14 = param_2;
  local_14[0x17] = param_1;
  local_14[0x1f] = local_14[0x1f] | 1;
  *unaff_FS_OFFSET = local_10;
  return local_14;
}

