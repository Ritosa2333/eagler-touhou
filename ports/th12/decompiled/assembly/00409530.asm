
// block 00409530
00409530  push       ebx
00409531  push       ebp
00409532  mov        ebx, 0x49f69c
00409537  mov        ecx, 6
0040953c  call       0x45fe60

// block 00409541
00409541  xor        ebp, ebp
00409543  mov        dword ptr [edi + 0x4debdc], eax
00409549  cmp        eax, ebp
0040954b  jne        0x409565

// block 0040954d
0040954d  push       0x49f6a8
00409552  mov        ecx, 0x4b0ec8
00409557  call       0x464220

// block 0040955c
0040955c  add        esp, 4
0040955f  pop        ebp
00409560  or         eax, 0xffffffff
00409563  pop        ebx
00409564  ret        

// block 00409565
00409565  push       esi
00409566  lea        eax, [edi + 0x64]
00409569  mov        ecx, 5
0040956e  push       0x24
00409570  mov        dword ptr [edi + 0x10], eax
00409573  mov        word ptr [edi + 0x4de716], cx
0040957a  call       0x46c9ea

// block 0040957f
0040957f  add        esp, 4
00409582  cmp        eax, ebp
00409584  je         0x4095a2

// block 00409586
00409586  and        dword ptr [eax + 4], 0xfffffffe
0040958a  mov        dword ptr [eax + 8], ebp
0040958d  mov        dword ptr [eax + 0xc], ebp
00409590  mov        dword ptr [eax + 0x10], ebp
00409593  mov        dword ptr [eax], ebp
00409595  mov        dword ptr [eax + 0x14], eax
00409598  mov        dword ptr [eax + 0x18], ebp
0040959b  mov        dword ptr [eax + 0x1c], ebp
0040959e  mov        esi, eax
004095a0  jmp        0x4095a4

// block 004095a2
004095a2  xor        esi, esi

// block 004095a4
004095a4  mov        edx, dword ptr [esi + 4]
004095a7  and        edx, 0xfffffffd
004095aa  or         edx, 1
004095ad  mov        ebx, 0x15
004095b2  mov        dword ptr [esi + 8], 0x40a1f0
004095b9  mov        dword ptr [esi + 0xc], ebp
004095bc  mov        dword ptr [esi + 0x10], ebp
004095bf  mov        dword ptr [esi + 0x20], edi
004095c2  mov        dword ptr [esi + 4], edx
004095c5  call       0x462380

// block 004095ca
004095ca  push       0x24
004095cc  mov        dword ptr [edi + 8], esi
004095cf  call       0x46c9ea

// block 004095d4
004095d4  add        esp, 4
004095d7  cmp        eax, ebp
004095d9  je         0x4095f7

// block 004095db
004095db  and        dword ptr [eax + 4], 0xfffffffe
004095df  mov        dword ptr [eax + 8], ebp
004095e2  mov        dword ptr [eax + 0xc], ebp
004095e5  mov        dword ptr [eax + 0x10], ebp
004095e8  mov        dword ptr [eax], ebp
004095ea  mov        dword ptr [eax + 0x14], eax
004095ed  mov        dword ptr [eax + 0x18], ebp
004095f0  mov        dword ptr [eax + 0x1c], ebp
004095f3  mov        esi, eax
004095f5  jmp        0x4095f9

// block 004095f7
004095f7  xor        esi, esi

// block 004095f9
004095f9  mov        eax, dword ptr [esi + 4]
004095fc  and        eax, 0xfffffffd
004095ff  or         eax, 1
00409602  mov        ebx, 0x1f
00409607  mov        dword ptr [esi + 8], 0x40a220
0040960e  mov        dword ptr [esi + 0xc], ebp
00409611  mov        dword ptr [esi + 0x10], ebp
00409614  mov        dword ptr [esi + 0x20], edi
00409617  mov        dword ptr [esi + 4], eax
0040961a  call       0x462420

// block 0040961f
0040961f  mov        dword ptr [edi + 0xc], esi
00409622  pop        esi
00409623  pop        ebp
00409624  xor        eax, eax
00409626  pop        ebx
00409627  ret        
