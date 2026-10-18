/* Entry: 0x005159e0; symbol: FUN_005159e0; body bytes: 180 */

void * __thiscall FUN_005159e0(void *this,int param_1,uint param_2,uint param_3,undefined4 param_4)

{
  uint uVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  byte local_11;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568cfd;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00467fa0(this,(uint)local_11,param_4);
  local_8 = 0;
  Check_offset((void *)(param_1 + 4),param_2);
  uVar1 = std::_String_val<struct_std::_Simple_types<char>_>::_Clamp_suffix_size
                    ((_String_val<struct_std::_Simple_types<char>_> *)(param_1 + 4),param_2,param_3)
  ;
  puVar2 = Myptr((undefined4 *)(param_1 + 4));
  FUN_00468030(this,(uint *)((int)puVar2 + param_2),uVar1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

