/* Entry: 0x004dbce0; symbol: FUN_004dbce0; body bytes: 132 */

void FUN_004dbce0(void)

{
  float fVar1;
  float fVar2;
  
  fVar1 = (float)(DAT_005b8810 - DAT_005b87f8) / DAT_0056c8d0;
  fVar2 = (float)(DAT_005b8814 - DAT_005b87fc) / DAT_0056c8d0;
  std::_Adl_verify_range<char*,char_const*>((char **)"RenderOffet %f, %f\n",SUB84((double)fVar1,0));
  FUN_004ddb20(&DAT_005c4d40,(int)fVar1,(int)fVar2);
  return;
}

