
// block 004925f4
004925f4  mov        edi, edi
004925f6  push       ebx
004925f7  push       edi
004925f8  xor        ebx, ebx
004925fa  xor        edi, edi
004925fc  cmp        dword ptr [esi], ebx
004925fe  jle        0x49261d

// block 00492600
00492600  mov        eax, dword ptr [esi + 4]
00492603  mov        ecx, dword ptr [ebx + eax + 4]
00492607  push       0x4ae4d8
0049260c  call       0x46cdd0

// block 00492611
00492611  test       al, al
00492613  jne        0x492622

// block 00492615
00492615  inc        edi
00492616  add        ebx, 0x10
00492619  cmp        edi, dword ptr [esi]
0049261b  jl         0x492600

// block 0049261d
0049261d  xor        al, al

// block 0049261f
0049261f  pop        edi
00492620  pop        ebx
00492621  ret        

// block 00492622
00492622  mov        al, 1
00492624  jmp        0x49261f
