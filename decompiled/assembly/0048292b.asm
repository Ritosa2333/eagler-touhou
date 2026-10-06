
// block 0048292b
0048292b  push       0x10
0048292d  push       0x4aaf18
00482932  call       0x46fcec

// block 00482937
00482937  xor        edi, edi
00482939  push       edi
0048293a  call       0x46ec9a

// block 0048293f
0048293f  pop        ecx
00482940  mov        dword ptr [ebp - 4], edi
00482943  cmp        dword ptr [ebp + 8], edi
00482946  jne        0x482964

// block 00482948
00482948  mov        esi, 0x4b4368
0048294d  push       dword ptr [0x4b4368]
00482953  call       0x4751de

// block 00482958
00482958  mov        dword ptr [ebp - 0x1c], eax
0048295b  mov        dword ptr [ebp - 0x20], 2
00482962  jmp        0x48297e

// block 00482964
00482964  mov        esi, 0x4b436c
00482969  push       dword ptr [0x4b436c]
0048296f  call       0x4751de

// block 00482974
00482974  mov        dword ptr [ebp - 0x1c], eax
00482977  mov        dword ptr [ebp - 0x20], 0x15

// block 0048297e
0048297e  pop        ecx
0048297f  cmp        eax, edi
00482981  je         0x48298f

// block 00482983
00482983  cmp        eax, 1
00482986  je         0x48298f

// block 00482988
00482988  call       0x4751d5

// block 0048298d
0048298d  mov        dword ptr [esi], eax

// block 0048298f
0048298f  mov        dword ptr [ebp - 4], 0xfffffffe
00482996  call       0x4829a6

// block 0048299b
0048299b  cmp        dword ptr [ebp - 0x1c], edi
0048299e  jne        0x4829ae

// block 004829a0
004829a0  xor        eax, eax
004829a2  jmp        0x4829be

// block 004829ae
004829ae  cmp        dword ptr [ebp - 0x1c], 1
004829b2  je         0x4829bb

// block 004829b4
004829b4  push       dword ptr [ebp - 0x20]
004829b7  call       dword ptr [ebp - 0x1c]

// block 004829ba
004829ba  pop        ecx

// block 004829bb
004829bb  xor        eax, eax
004829bd  inc        eax

// block 004829be
004829be  call       0x46fd31

// block 004829c3
004829c3  ret        4
