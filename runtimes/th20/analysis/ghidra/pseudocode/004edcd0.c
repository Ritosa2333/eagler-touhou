/* Entry: 0x004edcd0; symbol: FUN_004edcd0; body bytes: 209 */

void __cdecl FUN_004edcd0(char *param_1,char *param_2,undefined4 *param_3)

{
  char cVar1;
  char *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  cVar1 = *param_1;
  if ((cVar1 == '}') || (cVar1 == ':')) {
    FUN_004fc4d0(param_3);
  }
  else if ((cVar1 < '0') || ('9' < cVar1)) {
    func_0x004fcd20("Invalid format string.");
  }
  else {
    local_c = (char *)0x0;
    if (cVar1 == '0') {
      param_1 = param_1 + 1;
    }
    else {
      param_1 = FUN_004ee280(param_1,param_2,(uint *)&local_c);
    }
    if ((param_1 == param_2) || ((*param_1 != '}' && (*param_1 != ':')))) {
      func_0x004fcd20("Invalid format string.");
    }
    FUN_004fc890(param_3,local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

