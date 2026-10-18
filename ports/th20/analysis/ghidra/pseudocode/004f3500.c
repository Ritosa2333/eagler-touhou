/* Entry: 0x004f3500; symbol: FUN_004f3500; body bytes: 252 */

void FUN_004f3500(void)

{
  undefined4 *puVar1;
  tagCY tVar2;
  undefined4 uVar3;
  GdiplusStartupInput *pGVar4;
  undefined4 local_158;
  undefined1 local_154 [4];
  undefined4 local_150;
  undefined4 local_14c;
  undefined4 local_148;
  undefined4 local_144;
  undefined4 local_13c;
  undefined4 local_138;
  undefined1 local_131;
  GdiplusStartupInput local_130 [284];
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  FUN_0040c080(local_130,0x114);
  FUN_004f3e70(local_130);
  local_131 = FUN_004f38b0();
  tVar2 = COleCurrency::operator_union_tagCY((COleCurrency *)&stack0x00000004);
  local_150 = *(undefined4 *)tVar2.s.Lo;
  local_14c = ((undefined4 *)tVar2.s.Lo)[1];
  pGVar4 = local_130;
  uVar3 = 0x4f3597;
  local_148 = local_150;
  local_144 = local_14c;
  puVar1 = FUN_0040af10(local_154,pGVar4);
  local_13c = *puVar1;
  local_138 = local_13c;
  FUN_004f4460((undefined4 *)&stack0xfffffe98);
  FUN_004f3a90(&local_158,local_13c,local_150,local_14c,uVar3,pGVar4);
  FUN_004fb9a0((int)local_130);
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

