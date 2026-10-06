
// block 00472bdd
00472bdd  mov        edi, edi
00472bdf  push       ebp
00472be0  mov        ebp, esp
00472be2  push       edi
00472be3  mov        edi, 0x3e8

// block 00472be8
00472be8  push       edi
00472be9  call       dword ptr [0x498084]

// block 00472bef
00472bef  push       dword ptr [ebp + 8]
00472bf2  call       dword ptr [0x498174]

// block 00472bf8
00472bf8  add        edi, 0x3e8
00472bfe  cmp        edi, 0xea60
00472c04  ja         0x472c0a

// block 00472c06
00472c06  test       eax, eax
00472c08  je         0x472be8

// block 00472c0a
00472c0a  pop        edi
00472c0b  pop        ebp
00472c0c  ret        
