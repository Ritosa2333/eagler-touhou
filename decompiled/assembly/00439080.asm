
// block 00439080
00439080  mov        eax, dword ptr [0x4b4514]
00439085  push       ebx
00439086  push       edi
00439087  mov        edi, dword ptr [eax + 0xc598]
0043908d  test       edi, edi
0043908f  jne        0x4390bd

// block 00439091
00439091  cmp        dword ptr [esi + 0xcc], edi
00439097  je         0x4390a8

// block 00439099
00439099  mov        ecx, dword ptr [esi + 0xb0]
0043909f  push       ecx
004390a0  lea        ebx, [edi + 6]
004390a3  call       0x461970

// block 004390a8
004390a8  mov        edx, dword ptr [esi + 0x5c]
004390ab  mov        dword ptr [esi + 0x6c], edx
004390ae  mov        eax, dword ptr [esi + 0x60]
004390b1  mov        dword ptr [esi + 0x70], eax
004390b4  mov        dword ptr [esi + 0xcc], edi
004390ba  pop        edi
004390bb  pop        ebx
004390bc  ret        

// block 004390bd
004390bd  cmp        dword ptr [esi + 0xcc], 0
004390c4  jne        0x4390d7

// block 004390c6
004390c6  mov        ecx, dword ptr [esi + 0xb0]
004390cc  push       ecx
004390cd  mov        ebx, 3
004390d2  call       0x461970

// block 004390d7
004390d7  mov        edx, dword ptr [esi + 0x6c]
004390da  mov        dword ptr [esi + 0x54], edx
004390dd  mov        eax, dword ptr [esi + 0x70]
004390e0  mov        dword ptr [esi + 0x58], eax
004390e3  mov        dword ptr [esi + 0xcc], edi
004390e9  pop        edi
004390ea  pop        ebx
004390eb  ret        
