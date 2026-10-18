/* Entry: 0x004ee130; symbol: FUN_004ee130; body bytes: 249 */

void __cdecl FUN_004ee130(undefined4 param_1,undefined4 param_2,char **param_3)

{
  int iVar1;
  byte *pbVar2;
  void *this;
  byte *pbVar3;
  byte *pbVar4;
  byte *local_10;
  byte *local_8;
  
  local_8 = (byte *)FUN_0040c300(&param_1);
  iVar1 = FUN_0040ff90((int)&param_1);
  pbVar2 = local_8 + iVar1;
  this = (void *)FUN_004ec500();
  do {
    if (local_8 == pbVar2) {
      return;
    }
    local_10 = local_8;
    if (*local_8 != 0x7b) {
      local_10 = (byte *)FUN_004fbe30(this,(char *)local_8,(char *)pbVar2,'{');
      while (pbVar3 = (byte *)FUN_004fbe30(this,(char *)local_8,(char *)local_10,'}'),
            pbVar3 != local_10) {
        pbVar4 = pbVar3 + 1;
        if ((pbVar4 == local_10) || (*pbVar4 != 0x7d)) {
          func_0x004fcd20("Unmatched \'}\' in format string.");
        }
        FUN_004fcb40(param_3,local_8,pbVar4);
        local_8 = pbVar3 + 2;
      }
      FUN_004fcb40(param_3,local_8,local_10);
      if (local_10 == pbVar2) {
        return;
      }
    }
    local_8 = (byte *)FUN_004ee400((int)local_10,pbVar2,param_3);
  } while( true );
}

