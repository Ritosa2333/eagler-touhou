/* Entry: 0x00515550; symbol: FUN_00515550; body bytes: 316 */

void __cdecl FUN_00515550(undefined4 *param_1,char *param_2,undefined4 param_3,byte *param_4)

{
  byte bVar1;
  char cVar2;
  ulonglong *puVar3;
  ulonglong *puVar4;
  byte *pbVar5;
  byte *local_14;
  byte *local_10;
  byte *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>(&param_2,(char **)&param_3);
  local_c = (byte *)FUN_004690e0(&param_2);
  local_14 = (byte *)FUN_004690e0(&param_3);
  local_10 = (byte *)FUN_004683b0((char *)local_c,(char *)local_14,(char *)param_4);
  local_c = local_10;
  if (local_10 != local_14) {
    cVar2 = FUN_00411170();
    if (cVar2 == '\0') {
      puVar3 = (ulonglong *)FUN_00425620(&local_c);
      bVar1 = *param_4;
      puVar4 = (ulonglong *)FUN_00425620(&local_14);
      local_10 = (byte *)FUN_00514730(puVar3,puVar4,bVar1);
      FUN_004ee740(&param_2,&local_10);
      *param_1 = param_2;
      goto LAB_0051567e;
    }
    while (local_c = local_c + 1, local_c != local_14) {
      if (*local_c != *param_4) {
        pbVar5 = (byte *)move<>(local_c);
        *local_10 = *pbVar5;
        local_10 = local_10 + 1;
      }
    }
  }
  FUN_004ee740(&param_2,&local_10);
  *param_1 = param_2;
LAB_0051567e:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

