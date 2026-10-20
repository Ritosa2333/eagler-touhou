
// block 0049112f
0049112f  mov        edi, edi
00491131  push       ebp
00491132  mov        ebp, esp
00491134  push       esi
00491135  mov        esi, dword ptr [ebp + 8]
00491138  mov        eax, dword ptr [esi + 0xc]
0049113b  test       al, 0x83
0049113d  je         0x49115d

// block 0049113f
0049113f  test       al, 8
00491141  je         0x49115d

// block 00491143
00491143  push       dword ptr [esi + 8]
00491146  call       0x46c941

// block 0049114b
0049114b  and        dword ptr [esi + 0xc], 0xfffffbf7
00491152  xor        eax, eax
00491154  pop        ecx
00491155  mov        dword ptr [esi], eax
00491157  mov        dword ptr [esi + 8], eax
0049115a  mov        dword ptr [esi + 4], eax

// block 0049115d
0049115d  pop        esi
0049115e  pop        ebp
0049115f  ret        
