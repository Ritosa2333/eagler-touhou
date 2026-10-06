
// block 00470dc6
00470dc6  mov        edi, edi
00470dc8  push       ebp
00470dc9  mov        ebp, esp
00470dcb  sub        esp, 0x328
00470dd1  mov        eax, dword ptr [0x4ad138]
00470dd6  xor        eax, ebp
00470dd8  mov        dword ptr [ebp - 4], eax
00470ddb  and        dword ptr [ebp - 0x328], 0
00470de2  push       ebx
00470de3  push       0x4c
00470de5  lea        eax, [ebp - 0x324]
00470deb  push       0
00470ded  push       eax
00470dee  call       0x477420

// block 00470df3
00470df3  lea        eax, [ebp - 0x328]
00470df9  mov        dword ptr [ebp - 0x2d8], eax
00470dff  lea        eax, [ebp - 0x2d0]
00470e05  add        esp, 0xc
00470e08  mov        dword ptr [ebp - 0x2d4], eax
00470e0e  mov        dword ptr [ebp - 0x220], eax
00470e14  mov        dword ptr [ebp - 0x224], ecx
00470e1a  mov        dword ptr [ebp - 0x228], edx
00470e20  mov        dword ptr [ebp - 0x22c], ebx
00470e26  mov        dword ptr [ebp - 0x230], esi
00470e2c  mov        dword ptr [ebp - 0x234], edi
00470e32  mov        word ptr [ebp - 0x208], ss
00470e39  mov        word ptr [ebp - 0x214], cs
00470e40  mov        word ptr [ebp - 0x238], ds
00470e47  mov        word ptr [ebp - 0x23c], es
00470e4e  mov        word ptr [ebp - 0x240], fs
00470e55  mov        word ptr [ebp - 0x244], gs
00470e5c  pushfd     
00470e5d  pop        dword ptr [ebp - 0x210]
00470e63  mov        eax, dword ptr [ebp + 4]
00470e66  lea        ecx, [ebp + 4]
00470e69  mov        dword ptr [ebp - 0x2d0], 0x10001
00470e73  mov        dword ptr [ebp - 0x218], eax
00470e79  mov        dword ptr [ebp - 0x20c], ecx
00470e7f  mov        ecx, dword ptr [ecx - 4]
00470e82  mov        dword ptr [ebp - 0x21c], ecx
00470e88  mov        dword ptr [ebp - 0x328], 0xc0000417
00470e92  mov        dword ptr [ebp - 0x324], 1
00470e9c  mov        dword ptr [ebp - 0x31c], eax
00470ea2  call       dword ptr [0x4981c4]

// block 00470ea8
00470ea8  push       0
00470eaa  mov        ebx, eax
00470eac  call       dword ptr [0x4981c8]

// block 00470eb2
00470eb2  lea        eax, [ebp - 0x2d8]
00470eb8  push       eax
00470eb9  call       dword ptr [0x4981cc]

// block 00470ebf
00470ebf  test       eax, eax
00470ec1  jne        0x470ecf

// block 00470ec3
00470ec3  test       ebx, ebx
00470ec5  jne        0x470ecf

// block 00470ec7
00470ec7  push       2
00470ec9  call       0x47b3ff

// block 00470ece
00470ece  pop        ecx

// block 00470ecf
00470ecf  push       0xc0000417
00470ed4  call       dword ptr [0x4981fc]

// block 00470eda
00470eda  push       eax
00470edb  call       dword ptr [0x4981f8]

// block 00470ee1
00470ee1  mov        ecx, dword ptr [ebp - 4]
00470ee4  xor        ecx, ebp
00470ee6  pop        ebx
00470ee7  call       0x46c932

// block 00470eec
00470eec  leave      
00470eed  ret        
