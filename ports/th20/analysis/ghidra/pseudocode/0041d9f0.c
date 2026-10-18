/* Entry: 0x0041d9f0; symbol: FUN_0041d9f0; body bytes: 322 */

void __fastcall FUN_0041d9f0(int param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  uint uVar2;
  default_delete<class_std::_Facet_base> *local_10;
  int local_8;
  
  local_10 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0x6000730));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_0041be70(param_1 + 0x6000730);
  for (; local_10 != pdVar1; local_10 = local_10 + 4) {
    if (*(int *)local_10 != 0) {
      for (local_8 = 0; local_8 < *(int *)(*(int *)local_10 + 0x44); local_8 = local_8 + 1) {
        uVar2 = *(uint *)(*(int *)(*(int *)local_10 + 0x58) + 0x14 + local_8 * 0x18) & 3;
        if (uVar2 == 1) {
          FUN_0044c150(local_8 * 0x18 + *(int *)(*(int *)local_10 + 0x58),
                       (uint)*(ushort *)
                              (*(int *)(*(int *)(*(int *)local_10 + 0x58) + 0x10 + local_8 * 0x18) +
                              10),
                       (uint)*(ushort *)
                              (*(int *)(*(int *)(*(int *)local_10 + 0x58) + 0x10 + local_8 * 0x18) +
                              0xc));
        }
        else if (uVar2 == 2) {
          FUN_0044c0b0(local_8 * 0x18 + *(int *)(*(int *)local_10 + 0x58),
                       (uint)*(ushort *)
                              (*(int *)(*(int *)(*(int *)local_10 + 0x58) + 0x10 + local_8 * 0x18) +
                              10),
                       (uint)*(ushort *)
                              (*(int *)(*(int *)(*(int *)local_10 + 0x58) + 0x10 + local_8 * 0x18) +
                              0xc),
                       (uint)*(ushort *)
                              (*(int *)(*(int *)(*(int *)local_10 + 0x58) + 0x10 + local_8 * 0x18) +
                              0xe));
        }
      }
    }
  }
  return;
}

