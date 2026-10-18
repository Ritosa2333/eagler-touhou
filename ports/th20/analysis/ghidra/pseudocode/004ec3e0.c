/* Entry: 0x004ec3e0; symbol: FUN_004ec3e0; body bytes: 61 */

char * __cdecl FUN_004ec3e0(char *param_1,void *param_2,uint param_3)

{
  bool bVar1;
  
  FUN_0040c080(param_1,0x10);
  FUN_004feca0(param_2,(undefined4 *)param_1,param_3);
  bVar1 = FUN_004f5790(param_1);
  if (!bVar1) {
    func_0x004fcd20("Argument not found.");
  }
  return param_1;
}

