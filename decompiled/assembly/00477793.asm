
// block 00477793
00477793  mov        edi, edi
00477795  push       ebp
00477796  mov        ebp, esp
00477798  push       esi
00477799  mov        esi, dword ptr [ebp + 8]
0047779c  push       esi
0047779d  call       dword ptr [0x498084]

// block 004777a3
004777a3  add        esi, 0x3e8
004777a9  cmp        esi, dword ptr [0x4b41d0]
004777af  jbe        0x4777b4

// block 004777b1
004777b1  or         esi, 0xffffffff

// block 004777b4
004777b4  mov        eax, esi
004777b6  pop        esi
004777b7  pop        ebp
004777b8  ret        
