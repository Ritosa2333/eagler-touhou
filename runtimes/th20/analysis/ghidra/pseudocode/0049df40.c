/* Entry: 0x0049df40; symbol: FUN_0049df40; body bytes: 107 */

void __cdecl
FUN_0049df40(float param_1,float param_2,undefined4 param_3,float *param_4,float *param_5)

{
  int iVar1;
  
  iVar1 = FUN_0049dfd0(0x5b6758);
  *param_4 = param_1 / (float)iVar1;
  iVar1 = FUN_0049dfb0(0x5b6758);
  *param_5 = param_2 / (float)iVar1;
  if (*param_4 <= 0.0 && *param_4 != 0.0) {
    *param_4 = 0.0;
  }
  if (*param_5 <= 0.0 && *param_5 != 0.0) {
    *param_5 = 0.0;
  }
  return;
}

