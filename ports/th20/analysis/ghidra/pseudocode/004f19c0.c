/* Entry: 0x004f19c0; symbol: FUN_004f19c0; body bytes: 410 */

void __cdecl FUN_004f19c0(undefined4 *param_1,undefined1 *param_2,int param_3)

{
  bool bVar1;
  char *pcVar2;
  undefined4 *puVar3;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *this;
  default_delete<class_std::_Facet_base> *this_00;
  int in_stack_0000001c;
  undefined4 uStack00000020;
  undefined1 *puVar4;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined1 local_30 [4];
  undefined4 local_2c;
  undefined1 *local_28;
  undefined4 local_24;
  undefined4 *local_20;
  int *local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040c080(&local_c,4);
  FUN_004fed50(&stack0x00000010,&local_c);
  local_10 = 0;
  local_14 = 0;
  for (local_18 = 0; local_18 < in_stack_0000001c; local_18 = local_18 + 1) {
    pcVar2 = (char *)FUN_0040c300(&local_c);
    local_14 = *pcVar2 + local_14;
    local_20 = FUN_004f58c0(&local_c,&local_2c,1);
    local_1c = (int *)FUN_004fefc0(&stack0x00000010,local_30);
    bVar1 = FUN_00449400(local_20,local_1c);
    if (bVar1) {
      local_10 = local_10 + 1;
    }
    else {
      FUN_00461860(&local_c);
    }
  }
  puVar3 = (undefined4 *)move<>(&stack0x00000020);
  puVar3 = FUN_004ef4d0(&local_34,param_2,(undefined1 *)(param_3 - local_14),*puVar3);
  uStack00000020 = *puVar3;
  param_2 = (undefined1 *)(param_3 - local_14);
  for (; 0 < in_stack_0000001c; in_stack_0000001c = in_stack_0000001c + -1) {
    if (local_10 < 1) {
      FUN_004f5870(&local_c);
    }
    else {
      local_10 = local_10 + -1;
    }
    puVar4 = &stack0x00000018;
    this = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *)FUN_004f57f0(&stack0x00000020,&local_38);
    this_00 = std::
              _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              ::_Get_first(this);
    FUN_004f5690(this_00,puVar4);
    puVar3 = (undefined4 *)move<>(&stack0x00000020);
    local_24 = *puVar3;
    pcVar2 = (char *)FUN_0040c300(&local_c);
    local_28 = param_2 + *pcVar2;
    puVar3 = FUN_004ef4d0(&local_3c,param_2,local_28,local_24);
    uStack00000020 = *puVar3;
    pcVar2 = (char *)FUN_0040c300(&local_c);
    param_2 = param_2 + *pcVar2;
  }
  *param_1 = uStack00000020;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

