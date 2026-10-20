
// block 0041e641
0041e641  xor        eax, eax
0041e643  cmp        dword ptr [0x4b0cb8], 0x18
0041e64a  setge      al
0041e64d  dec        eax
0041e64e  and        eax, 0xfffffffb
0041e651  add        eax, 0x7c
