/* Entry: 0x004ee310; symbol: FUN_004ee310; body bytes: 235 */

void __cdecl FUN_004ee310(char *param_1,char *param_2,void *param_3)

{
  char *pcVar1;
  undefined4 *puVar2;
  undefined1 local_14 [7];
  char local_d;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  pcVar1 = param_1 + 1;
  local_d = '\0';
  if (pcVar1 != param_2) {
    local_d = *pcVar1;
  }
  if ((local_d < '0') || ('9' < local_d)) {
    if (local_d == '{') {
      param_1 = param_1 + 2;
      if (param_1 != param_2) {
        puVar2 = FUN_004142a0(local_14,param_3);
        param_1 = (char *)FUN_004eddb0(param_1,param_2,puVar2);
      }
      if ((param_1 == param_2) || (*param_1 != '}')) {
        func_0x004fcd20("Invalid format string.");
      }
    }
    else {
      func_0x004fcd20("Missing precision specifier.");
    }
  }
  else {
    local_c = 0;
    FUN_004ee230(pcVar1,param_2,&local_c);
    FUN_004fc920(param_3,local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

