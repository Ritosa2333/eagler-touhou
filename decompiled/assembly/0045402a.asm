
// block 0045402a
0045402a  test       byte ptr [0x4ceae8], 0x10
00454031  je         0x45405f

// block 00454033
00454033  cmp        dword ptr [ebp + 8], ebx
00454036  jne        0x4543a1

// block 0045403c
0045403c  mov        esi, 0x4cf4e8
00454041  call       0x453c30

// block 00454046
00454046  mov        ecx, dword ptr [ebp + 4]
00454049  lea        eax, [ebp + 0xc]
0045404c  push       ecx
0045404d  call       0x4538e0

// block 00454052
00454052  mov        dword ptr [esp + 0x10], 1
0045405a  jmp        0x454282

// block 0045405f
0045405f  mov        edx, dword ptr [ebp + 4]
00454062  lea        eax, [ebp + 0xc]
00454065  push       edx
00454066  call       0x4538e0

// block 0045406b
0045406b  mov        dword ptr [esp + 0x10], 1
00454073  jmp        0x454282

// block 004543a1
004543a1  inc        dword ptr [ebp + 8]
