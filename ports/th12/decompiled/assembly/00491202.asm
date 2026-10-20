
// block 00491202
00491202  mov        edi, edi
00491204  push       ebp
00491205  mov        ebp, esp
00491207  push       ecx
00491208  cmp        dword ptr [0x4d52dc], 0
0049120f  je         0x491217

// block 00491211
00491211  stmxcsr    dword ptr [ebp - 4]
00491215  jmp        0x49121b

// block 00491217
00491217  and        dword ptr [ebp - 4], 0

// block 0049121b
0049121b  mov        eax, dword ptr [ebp - 4]
0049121e  leave      
0049121f  ret        
