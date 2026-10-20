
// block 00491517
00491517  push       4
00491519  mov        eax, 0x496d08
0049151e  call       0x491e59

// block 00491523
00491523  mov        esi, ecx
00491525  mov        dword ptr [ebp - 0x10], esi
00491528  call       0x46df4d

// block 0049152d
0049152d  push       dword ptr [ebp + 8]
00491530  and        dword ptr [ebp - 4], 0
00491534  lea        ecx, [esi + 0xc]
00491537  mov        dword ptr [esi], 0x49f384
0049153d  call       0x491756

// block 00491542
00491542  mov        eax, esi
00491544  call       0x491f31

// block 00491549
00491549  ret        4

// block 00496d00
00496d00  mov        ecx, dword ptr [ebp - 0x10]
00496d03  jmp        0x46e086

// block 00496d08
00496d08  mov        edx, dword ptr [esp + 8]
00496d0c  lea        eax, [edx + 0xc]
00496d0f  mov        ecx, dword ptr [edx - 0x14]
00496d12  xor        ecx, eax
00496d14  call       0x46c932

// block 00496d19
00496d19  mov        eax, 0x4ab22c
00496d1e  jmp        0x491aab
