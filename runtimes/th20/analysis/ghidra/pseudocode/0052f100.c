/* Entry: 0x0052f100; symbol: FUN_0052f100; body bytes: 130 */

void __cdecl FUN_0052f100(byte *param_1)

{
  byte bVar1;
  byte *local_c;
  char local_6;
  byte local_5;
  
  local_c = (byte *)FUN_0049f1c0(&stack0x00000008,0);
  local_5 = 0x77;
  local_6 = '\a';
  do {
    bVar1 = *local_c;
    *param_1 = *local_c ^ local_5;
    local_c = local_c + 1;
    param_1 = param_1 + 1;
    local_5 = local_5 + local_6;
    local_6 = local_6 + '\x10';
  } while (bVar1 != 0);
  FUN_00449240((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)&stack0x00000008);
  return;
}

