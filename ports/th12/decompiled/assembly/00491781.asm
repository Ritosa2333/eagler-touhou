
// block 00491781
00491781  push       4
00491783  mov        eax, 0x496d94
00491788  call       0x491e59

// block 0049178d
0049178d  mov        esi, ecx
0049178f  mov        dword ptr [ebp - 0x10], esi
00491792  mov        edi, dword ptr [ebp + 8]
00491795  push       edi
00491796  call       0x46dfce

// block 0049179b
0049179b  and        dword ptr [ebp - 4], 0
0049179f  add        edi, 0xc
004917a2  push       edi
004917a3  lea        ecx, [esi + 0xc]
004917a6  mov        dword ptr [esi], 0x49f384
004917ac  call       0x491756

// block 004917b1
004917b1  mov        eax, esi
004917b3  call       0x491f31

// block 004917b8
004917b8  ret        4

// block 00496d8c
00496d8c  mov        ecx, dword ptr [ebp - 0x10]
00496d8f  jmp        0x46e086

// block 00496d94
00496d94  mov        edx, dword ptr [esp + 8]
00496d98  lea        eax, [edx + 0xc]
00496d9b  mov        ecx, dword ptr [edx - 0x14]
00496d9e  xor        ecx, eax
00496da0  call       0x46c932

// block 00496da5
00496da5  mov        eax, 0x4ab3ac
00496daa  jmp        0x491aab
