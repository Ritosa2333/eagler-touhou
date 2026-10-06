
// block 00432720
00432720  push       ecx
00432721  fldz       
00432723  push       ebx
00432724  push       ebp
00432725  mov        dword ptr [esi + 4], 1
0043272c  mov        eax, dword ptr [esi + 0x20]
0043272f  xor        ebp, ebp
00432731  push       edi
00432732  mov        edx, 0xfff0bdc1
00432737  mov        ecx, 0x4b2ed0
0043273c  test       al, 1
0043273e  jne        0x432752

// block 00432740
00432740  or         eax, 1
00432743  fst        dword ptr [esi + 0x18]
00432746  mov        dword ptr [esi + 0x14], ebp
00432749  mov        dword ptr [esi + 0x10], edx
0043274c  mov        dword ptr [esi + 0x1c], ecx
0043274f  mov        dword ptr [esi + 0x20], eax

// block 00432752
00432752  or         edi, 0xffffffff
00432755  fst        dword ptr [esi + 0x18]
00432758  mov        dword ptr [esi + 0x14], ebp
0043275b  mov        dword ptr [esi + 0x10], edi
0043275e  mov        eax, dword ptr [esi + 0x34]
00432761  test       al, 1
00432763  jne        0x432777

// block 00432765
00432765  or         eax, 1
00432768  fst        dword ptr [esi + 0x2c]
0043276b  mov        dword ptr [esi + 0x28], ebp
0043276e  mov        dword ptr [esi + 0x24], edx
00432771  mov        dword ptr [esi + 0x30], ecx
00432774  mov        dword ptr [esi + 0x34], eax

// block 00432777
00432777  mov        eax, dword ptr [0x4b44e8]
0043277c  fstp       dword ptr [esi + 0x2c]
0043277f  mov        dword ptr [esi + 0x28], ebp
00432782  mov        dword ptr [esi + 0x24], edi
00432785  or         dword ptr [eax + 0x60], 0x10
00432789  mov        edi, dword ptr [0x4cee70]
0043278f  push       0x4b
00432791  lea        eax, [esp + 0x10]
00432795  push       eax
00432796  call       0x4357c0

// block 0043279b
0043279b  mov        eax, dword ptr [eax]
0043279d  push       eax
0043279e  mov        dword ptr [esi + 0x1ec], eax
004327a4  call       0x435750

// block 004327a9
004327a9  mov        ecx, dword ptr [0x4b43e4]
004327af  mov        edi, dword ptr [ecx + 0x6d44]
004327b5  mov        edx, dword ptr [0x4b44e8]
004327bb  mov        dword ptr [esi + 0x2dc], edi
004327c1  cmp        dword ptr [edx + 0x74], ebp
004327c4  je         0x4327dc

// block 004327c6
004327c6  push       0x67
004327c8  lea        eax, [esp + 0x10]
004327cc  push       eax
004327cd  call       0x4357c0

// block 004327d2
004327d2  mov        ecx, dword ptr [eax]
004327d4  mov        dword ptr [esi + 0x1e8], ecx
004327da  jmp        0x4327f0

// block 004327dc
004327dc  push       0x66
004327de  lea        edx, [esp + 0x10]
004327e2  push       edx
004327e3  call       0x4357c0

// block 004327e8
004327e8  mov        eax, dword ptr [eax]
004327ea  mov        dword ptr [esi + 0x1e8], eax

// block 004327f0
004327f0  mov        ecx, dword ptr [esi + 0x1e8]
004327f6  push       ecx
004327f7  mov        ebx, 3
004327fc  call       0x461970

// block 00432801
00432801  call       0x423020

// block 00432806
00432806  lea        edx, [ebx + 0xb]
00432809  call       0x453d90

// block 0043280e
0043280e  push       ebp
0043280f  push       6
00432811  mov        edi, 0x4a1044
00432816  mov        eax, 0x4cf4e8
0043281b  call       0x454960

// block 00432820
00432820  fld        dword ptr [0x4b2ed0]
00432826  fstp       dword ptr [esi + 0x2d8]
0043282c  mov        edx, dword ptr [0x4cf468]
00432832  fld1       
00432834  pop        edi
00432835  fstp       dword ptr [0x4b2ed0]
0043283b  mov        dword ptr [esi + 0x200], edx
00432841  mov        dword ptr [0x4cf468], ebp
00432847  pop        ebp
00432848  pop        ebx
00432849  pop        ecx
0043284a  ret        
