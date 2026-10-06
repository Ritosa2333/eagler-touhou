
// block 004641e0
004641e0  mov        eax, dword ptr [0x4ae590]
004641e5  cmp        eax, -1
004641e8  je         0x46420e

// block 004641ea
004641ea  push       eax
004641eb  call       dword ptr [0x4980a4]

// block 004641f1
004641f1  test       dword ptr [0x4cee78], 0x8000
004641fb  je         0x46420e

// block 004641fd
004641fd  push       0x4cf128
00464202  call       dword ptr [0x49808c]

// block 00464208
00464208  dec        byte ptr [0x4cf21a]

// block 0046420e
0046420e  xor        eax, eax
00464210  ret        
