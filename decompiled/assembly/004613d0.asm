
// block 004613d0
004613d0  mov        edx, dword ptr [0x4ce8cc]
004613d6  lea        ecx, [ebx + 4]
004613d9  push       esi
004613da  mov        dword ptr [ecx], ebx
004613dc  mov        dword ptr [ecx + 4], 0
004613e3  mov        dword ptr [ecx + 8], 0
004613ea  mov        esi, dword ptr [edx + 0x8856c0]
004613f0  test       esi, esi
004613f2  jne        0x4613fc

// block 004613f4
004613f4  mov        dword ptr [edx + 0x8856c4], ecx
004613fa  jmp        0x461414

// block 004613fc
004613fc  push       edi
004613fd  mov        edi, dword ptr [ecx + 4]
00461400  test       edi, edi
00461402  je         0x46140d

// block 00461404
00461404  mov        dword ptr [esi + 4], edi
00461407  mov        edi, dword ptr [ecx + 4]
0046140a  mov        dword ptr [edi + 8], esi

// block 0046140d
0046140d  mov        dword ptr [ecx + 4], esi
00461410  mov        dword ptr [esi + 8], ecx
00461413  pop        edi

// block 00461414
00461414  mov        dword ptr [edx + 0x8856c0], ecx
0046141a  mov        ecx, 1
0046141f  add        dword ptr [edx + 0x88ed48], ecx
00461425  cmp        dword ptr [edx + 0x88ed48], 0
0046142c  pop        esi
0046142d  jne        0x461435

// block 0046142f
0046142f  add        dword ptr [edx + 0x88ed48], ecx

// block 00461435
00461435  mov        ecx, dword ptr [edx + 0x88ed48]
0046143b  mov        dword ptr [ebx], ecx
0046143d  mov        edx, dword ptr [edx + 0x88ed48]
00461443  mov        dword ptr [eax], edx
00461445  ret        
