
// block 00493047
00493047  mov        edi, edi
00493049  push       ebp
0049304a  mov        ebp, esp
0049304c  push       esi
0049304d  push       dword ptr [ebp + 8]
00493050  mov        esi, ecx
00493052  call       0x46dfce

// block 00493057
00493057  mov        dword ptr [esi], 0x49f3f8
0049305d  mov        eax, esi
0049305f  pop        esi
00493060  pop        ebp
00493061  ret        4
