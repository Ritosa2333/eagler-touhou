/* Entry: 0x0048a990; symbol: FUN_0048a990; body bytes: 107 */

uint FUN_0048a990(void)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined4 *local_8;
  
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000004,(char **)&stack0x00000008);
  local_8 = (undefined4 *)FUN_00467070((undefined4 *)&stack0x00000004);
  puVar2 = (undefined4 *)FUN_00467070((undefined4 *)&stack0x00000008);
  puVar3 = puVar2;
  while( true ) {
    if (local_8 == puVar2) {
      return (uint)puVar3 & 0xffffff00;
    }
    iVar4 = FUN_0040c300(local_8);
    bVar1 = FUN_0048be50(iVar4);
    if (bVar1) break;
    local_8 = local_8 + 1;
    puVar3 = (undefined4 *)0x0;
  }
  return 1;
}

