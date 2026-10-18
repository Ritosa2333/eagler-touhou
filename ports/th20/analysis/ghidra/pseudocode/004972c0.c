/* Entry: 0x004972c0; symbol: FUN_004972c0; body bytes: 147 */

void __fastcall FUN_004972c0(int param_1)

{
  *(undefined4 *)(param_1 + 0x10) = 0;
  *(undefined4 *)(param_1 + 0x18) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x14) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x34) = 0xffffffff;
  *(int *)(param_1 + 0x38) = param_1;
  *(uint *)(param_1 + 0x54) = *(uint *)(param_1 + 0x54) & 0xfffffffe;
  *(undefined4 *)(param_1 + 0x3c) = 0;
  FUN_00497f40((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0x44));
  *(int *)(param_1 + 0xc) = param_1 + 0x10;
  FUN_00497360((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0x1c));
  FUN_0044a670((void *)(param_1 + 0x5c),param_1 + 0x10);
  return;
}

