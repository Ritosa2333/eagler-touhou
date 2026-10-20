
// block 00454265
00454265  cmp        byte ptr [0x4ceacb], 1
0045426c  jne        0x454282

// block 0045426e
0045426e  mov        esi, dword ptr [0x4d4754]
00454274  cmp        dword ptr [esi + 0x78], ebx
00454277  jne        0x4543a4

// block 0045427d
0045427d  call       0x466570
