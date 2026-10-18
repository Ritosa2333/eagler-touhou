/* Entry: 0x0055969b; symbol: FUN_0055969b; body bytes: 67 */

uint __cdecl FUN_0055969b(undefined2 *param_1,byte *param_2,uint param_3,uint *param_4,int param_5)

{
  uint uVar1;
  uint local_8;
  
  uVar1 = FUN_0055940b(&local_8,param_2,param_3,param_4,param_5);
  if (uVar1 < 5) {
    if (0xffff < local_8) {
      local_8 = 0xfffd;
    }
    if (param_1 != (undefined2 *)0x0) {
      *param_1 = (short)local_8;
    }
  }
  return uVar1;
}

