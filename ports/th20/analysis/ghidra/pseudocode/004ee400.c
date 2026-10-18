/* Entry: 0x004ee400; symbol: FUN_004ee400; body bytes: 300 */

void __cdecl FUN_004ee400(int param_1,byte *param_2,char **param_3)

{
  byte *pbVar1;
  char *pcVar2;
  byte local_11;
  char **local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  pbVar1 = (byte *)(param_1 + 1);
  if (pbVar1 == param_2) {
    func_0x004fcd20("Invalid format string.");
  }
  if (*pbVar1 == 0x7d) {
    pcVar2 = FUN_004ff890(param_3);
    FUN_004fc970((uint)pcVar2,(int)pbVar1);
  }
  else if (*pbVar1 == 0x7b) {
    FUN_004fcb40(param_3,pbVar1,(undefined1 *)(param_1 + 2));
  }
  else {
    local_10 = param_3;
    local_c = 0xffffffff;
    pbVar1 = (byte *)FUN_004edcd0((char *)pbVar1,(char *)param_2,&local_10);
    local_11 = 0;
    if (pbVar1 != param_2) {
      local_11 = *pbVar1;
    }
    if (local_11 == 0x7d) {
      FUN_004fc970(local_c,(int)pbVar1);
    }
    else if (local_11 == 0x3a) {
      pbVar1 = (byte *)FUN_004fc5f0(param_3,local_c,pbVar1 + 1,param_2);
      if ((pbVar1 == param_2) || (*pbVar1 != 0x7d)) {
        func_0x004fcd20("Unknown format specifier.");
      }
    }
    else {
      func_0x004fcd20("Missing \'}\' in format string.");
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

