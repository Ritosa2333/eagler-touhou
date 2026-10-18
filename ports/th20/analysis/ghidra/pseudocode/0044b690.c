/* Entry: 0x0044b690; symbol: FUN_0044b690; body bytes: 132 */

void * __cdecl FUN_0044b690(void *param_1)

{
  uint *puVar1;
  int iVar2;
  short *psVar3;
  short *psVar4;
  short local_6;
  
  puVar1 = (uint *)FUN_0040c300((undefined4 *)&stack0x00000008);
  iVar2 = FUN_0040ff90((int)&stack0x00000008);
  iVar2 = (int)puVar1 + iVar2 * 2;
  psVar3 = (short *)FUN_0044b4e0(puVar1,iVar2);
  local_6 = 0x3a;
  psVar4 = (short *)FUN_004478b0((char *)psVar3,iVar2,&local_6);
  psVar4 = FUN_0044b450(psVar3,psVar4);
  FUN_0040de00(param_1,psVar3,(int)psVar4 - (int)psVar3 >> 1);
  return param_1;
}

