/* Entry: 0x004ec380; symbol: FUN_004ec380; body bytes: 93 */

void __cdecl
FUN_004ec380(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,undefined4 param_6)

{
  char *local_28 [8];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_004f3fb0(local_28,param_2,param_3,param_4,param_5,param_6,&stack0x0000001c);
  FUN_004ee130(param_3,param_4,local_28);
  *param_1 = param_2;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

