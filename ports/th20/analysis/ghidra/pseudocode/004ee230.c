/* Entry: 0x004ee230; symbol: FUN_004ee230; body bytes: 71 */

void __cdecl FUN_004ee230(char *param_1,char *param_2,uint *param_3)

{
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_c = 0;
  FUN_004ee280(param_1,param_2,&local_c);
  *param_3 = local_c;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

