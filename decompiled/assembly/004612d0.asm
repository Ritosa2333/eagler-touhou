
// block 004612d0
004612d0  mov        edx, dword ptr [0x4ce8cc]
004612d6  lea        ecx, [ebx + 4]
004612d9  push       esi
004612da  mov        dword ptr [ecx], ebx
004612dc  mov        dword ptr [ecx + 4], 0
004612e3  mov        dword ptr [ecx + 8], 0
004612ea  mov        esi, dword ptr [edx + 0x8856b8]
004612f0  test       esi, esi
004612f2  jne        0x4612fc

// block 004612f4
004612f4  mov        dword ptr [edx + 0x8856bc], ecx
004612fa  jmp        0x461314

// block 004612fc
004612fc  push       edi
004612fd  mov        edi, dword ptr [ecx + 4]
00461300  test       edi, edi
00461302  je         0x46130d

// block 00461304
00461304  mov        dword ptr [esi + 4], edi
00461307  mov        edi, dword ptr [ecx + 4]
0046130a  mov        dword ptr [edi + 8], esi

// block 0046130d
0046130d  mov        dword ptr [ecx + 4], esi
00461310  mov        dword ptr [esi + 8], ecx
00461313  pop        edi

// block 00461314
00461314  mov        dword ptr [edx + 0x8856b8], ecx
0046131a  mov        ecx, 1
0046131f  add        dword ptr [edx + 0x88ed48], ecx
00461325  cmp        dword ptr [edx + 0x88ed48], 0
0046132c  pop        esi
0046132d  jne        0x461335

// block 0046132f
0046132f  add        dword ptr [edx + 0x88ed48], ecx

// block 00461335
00461335  mov        ecx, dword ptr [edx + 0x88ed48]
0046133b  mov        dword ptr [ebx], ecx
0046133d  mov        edx, dword ptr [edx + 0x88ed48]
00461343  mov        dword ptr [eax], edx
00461345  ret        
