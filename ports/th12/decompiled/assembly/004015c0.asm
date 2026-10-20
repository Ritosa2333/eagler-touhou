
// block 004015c0
004015c0  sub        esp, 0x104
004015c6  mov        eax, dword ptr [0x4ad138]
004015cb  xor        eax, esp
004015cd  mov        dword ptr [esp + 0x100], eax
004015d4  mov        ecx, dword ptr [esp + 0x108]
004015db  lea        eax, [esp + 0x10c]
004015e2  push       eax
004015e3  push       ecx
004015e4  lea        edx, [esp + 8]
004015e8  push       edx
004015e9  call       0x46cad8

// block 004015ee
004015ee  add        esp, 0xc
004015f1  lea        eax, [esp]
004015f4  mov        ecx, esi
004015f6  call       0x4014f0

// block 004015fb
004015fb  mov        ecx, dword ptr [esp + 0x100]
00401602  xor        ecx, esp
00401604  call       0x46c932

// block 00401609
00401609  add        esp, 0x104
0040160f  ret        
