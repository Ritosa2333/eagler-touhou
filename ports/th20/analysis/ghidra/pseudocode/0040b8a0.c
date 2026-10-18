/* Entry: 0x0040b8a0; symbol: FUN_0040b8a0; body bytes: 63 */

undefined4 * __fastcall FUN_0040b8a0(undefined4 *param_1)

{
  _String_val<char,class_std::_DebugHeapAllocator<char>_> *p_Var1;
  undefined4 local_c;
  
  p_Var1 = (_String_val<char,class_std::_DebugHeapAllocator<char>_> *)operator_new(0x14);
  if (p_Var1 == (_String_val<char,class_std::_DebugHeapAllocator<char>_> *)0x0) {
    local_c = (_String_val<char,class_std::_DebugHeapAllocator<char>_> *)0x0;
  }
  else {
    local_c = FUN_0040b7b0(p_Var1);
  }
  *param_1 = local_c;
  return param_1;
}

