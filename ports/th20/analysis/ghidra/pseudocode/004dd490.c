/* Entry: 0x004dd490; symbol: FUN_004dd490; body bytes: 356 */

undefined4 FUN_004dd490(void)

{
  int iVar1;
  char **unaff_EBP;
  
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown MotherInf\n",unaff_EBP);
  do {
    iVar1 = FUN_004277f0(&DAT_005ba830);
  } while (iVar1 != 0);
  FUN_0040bcf0((undefined4 *)&DAT_005c5ae0);
  FUN_004d9e30((char **)&DAT_005c4d40);
  if (DAT_005c5b04 != (LPVOID)0x0) {
    if (DAT_005c5b04 != (LPVOID)0x0) {
      FUN_0041f670(DAT_005c5b04);
    }
    DAT_005c5b04 = (LPVOID)0x0;
  }
  FUN_004dd730();
  FUN_004217c0(DAT_005c4a00);
  FUN_004217c0(DAT_005b8898);
  std::_Adl_verify_range<char*,char_const*>((char **)"VertexBuffer delete\n",unaff_EBP);
  FUN_004dab40(DAT_005c0028);
  std::_Adl_verify_range<char*,char_const*>((char **)"Sound Stop\n",unaff_EBP);
  FUN_00428c90(&DAT_005ba830,(char **)0x4,0,0x571b14);
  FUN_0053a170((undefined4 *)&DAT_005b66c8);
  FUN_00447410(DAT_005c4eec);
  DAT_005c4eec = (void *)0x0;
  FUN_00447410(DAT_005c4ef0);
  DAT_005c4ef0 = (void *)0x0;
  FUN_00447410(DAT_005c4ef4);
  DAT_005c4ef4 = (void *)0x0;
  FUN_00447410(DAT_005c4ef8);
  DAT_005c4ef8 = (void *)0x0;
  FUN_00447410(DAT_005c4efc);
  DAT_005c4efc = (void *)0x0;
  return 0;
}

