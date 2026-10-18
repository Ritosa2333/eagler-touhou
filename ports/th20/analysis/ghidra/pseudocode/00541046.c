/* Entry: 0x00541046; symbol: ___std_fs_code_page@0; body bytes: 29 */

/* Library Function - Single Match
    ___std_fs_code_page@0
   
   Library: Visual Studio 2019 Release */

uint ___std_fs_code_page_0(void)

{
  UINT UVar1;
  BOOL BVar2;
  
  UVar1 = ____lc_codepage_func();
  if (UVar1 == 0xfde9) {
    return 0xfde9;
  }
  BVar2 = AreFileApisANSI();
  return (uint)(BVar2 == 0);
}

