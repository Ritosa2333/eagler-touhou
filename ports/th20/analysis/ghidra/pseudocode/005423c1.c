/* Entry: 0x005423c1; symbol: _Random_device; body bytes: 34 */

/* Library Function - Single Match
    unsigned int __cdecl std::_Random_device(void)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

uint __cdecl std::_Random_device(void)

{
  code *pcVar1;
  int iVar2;
  uint uVar3;
  uint local_8;
  
  iVar2 = rand_s(&local_8);
  if (iVar2 == 0) {
    return local_8;
  }
  _Xout_of_range("invalid random_device value");
  pcVar1 = (code *)swi(3);
  uVar3 = (*pcVar1)();
  return uVar3;
}

