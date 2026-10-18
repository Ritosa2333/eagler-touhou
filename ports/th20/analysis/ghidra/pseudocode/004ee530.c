/* Entry: 0x004ee530; symbol: FUN_004ee530; body bytes: 199 */

void __cdecl FUN_004ee530(char *param_1,char *param_2,void *param_3)

{
  undefined4 *puVar1;
  undefined1 local_10 [4];
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((*param_1 < '1') || ('9' < *param_1)) {
    if (*param_1 == '{') {
      param_1 = param_1 + 1;
      if (param_1 != param_2) {
        puVar1 = FUN_004142a0(local_10,param_3);
        param_1 = (char *)FUN_004ede90(param_1,param_2,puVar1);
      }
      if ((param_1 == param_2) || (*param_1 != '}')) {
        func_0x004fcd20("Invalid format string.");
      }
    }
  }
  else {
    local_c = 0;
    FUN_004ee230(param_1,param_2,&local_c);
    FUN_004fcbc0(param_3,local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

