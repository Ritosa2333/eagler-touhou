/* Entry: 0x00542281; symbol: _Addstd; body bytes: 76 */

/* Library Function - Single Match
    public: static void __cdecl std::ios_base::_Addstd(class std::ios_base *)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl std::ios_base::_Addstd(ios_base *param_1)

{
  ios_base *piVar1;
  uint uVar2;
  _Lockit local_8 [4];
  
  _Lockit::_Lockit(local_8,2);
  uVar2 = 1;
  *(undefined4 *)(param_1 + 8) = 1;
  do {
    piVar1 = *(ios_base **)(uVar2 * 4 + 0x5e4e38);
    if ((piVar1 == (ios_base *)0x0) || (piVar1 == param_1)) break;
    uVar2 = uVar2 + 1;
    *(uint *)(param_1 + 8) = uVar2;
  } while (uVar2 < 8);
  (&DAT_005e4e60)[uVar2] = (&DAT_005e4e60)[uVar2] + '\x01';
  *(ios_base **)(uVar2 * 4 + 0x5e4e38) = param_1;
  FUN_005419ac((int *)local_8);
  return;
}

