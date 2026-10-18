/* Entry: 0x00457300; symbol: FUN_00457300; body bytes: 122 */

undefined1 __cdecl
FUN_00457300(float param_1,float param_2,float param_3,float param_4,float param_5,float param_6)

{
  float10 fVar1;
  
  fVar1 = FUN_00445680(param_1 - param_3);
  if (((float)fVar1 < param_5 / DAT_0056c8d0) &&
     (fVar1 = FUN_00445680(param_2 - param_4), (float)fVar1 < param_6 / DAT_0056c8d0)) {
    return 1;
  }
  return 0;
}

