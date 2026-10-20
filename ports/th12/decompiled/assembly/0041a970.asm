
// block 0041a970
0041a970  mov        edx, dword ptr [ecx + 0x174c]
0041a976  mov        edx, dword ptr [edx + 4]
0041a979  mov        ecx, dword ptr [edx + 4]
0041a97c  push       esi
0041a97d  movzx      esi, word ptr [ecx + 8]
0041a981  mov        ecx, dword ptr [esp + 8]
0041a985  push       edi
0041a986  mov        edi, 1
0041a98b  shl        edi, cl
0041a98d  test       edi, esi
0041a98f  pop        edi
0041a990  pop        esi
0041a991  je         0x41a99e

// block 0041a993
0041a993  mov        dword ptr [esp + 4], eax
0041a997  mov        ecx, edx
0041a999  jmp        0x468f70

// block 0041a99e
0041a99e  ret        4
