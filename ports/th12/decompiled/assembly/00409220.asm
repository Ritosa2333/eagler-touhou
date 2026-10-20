
// block 00409220
00409220  push       ecx
00409221  mov        ecx, dword ptr [eax + 0x18]
00409224  cmp        ecx, 0xb4
0040922a  jge        0x409243

// block 0040922c
0040922c  fld        dword ptr [eax + 0x1c]
0040922f  fmul       qword ptr [0x4a4390]
00409235  fdiv       qword ptr [0x4a4388]
0040923b  fadd       qword ptr [0x4a3d68]
00409241  jmp        0x40926e

// block 00409243
00409243  cmp        ecx, 0xc8
00409249  jge        0x409268

// block 0040924b
0040924b  fld        dword ptr [eax + 0x1c]
0040924e  fsub       qword ptr [0x4a4388]
00409254  fmul       qword ptr [0x4a3e80]
0040925a  fdiv       qword ptr [0x4a4170]
00409260  fadd       qword ptr [0x4a3e88]
00409266  jmp        0x40926e

// block 00409268
00409268  fld        dword ptr [0x4a3ec0]

// block 0040926e
0040926e  push       ebx
0040926f  fstp       dword ptr [esp + 4]
00409273  fld        dword ptr [esp + 4]
00409277  push       esi
00409278  lea        esi, [eax + 0x508]
0040927e  mov        eax, dword ptr [0x4b43cc]
00409283  mov        ecx, dword ptr [eax + 0x7c]
00409286  not        ecx
00409288  push       1
0040928a  and        ecx, 1
0040928d  push       ecx
0040928e  push       ecx
0040928f  mov        ebx, esi
00409291  fstp       dword ptr [esp]
00409294  call       0x40caa0

// block 00409299
00409299  fld        dword ptr [esp + 8]
0040929d  mov        edx, dword ptr [0x4b43cc]
004092a3  mov        eax, dword ptr [edx + 0x7c]
004092a6  not        eax
004092a8  push       1
004092aa  and        eax, 1
004092ad  push       eax
004092ae  push       ecx
004092af  fstp       dword ptr [esp]
004092b2  call       0x4286f0

// block 004092b7
004092b7  pop        esi
004092b8  xor        eax, eax
004092ba  pop        ebx
004092bb  pop        ecx
004092bc  ret        
