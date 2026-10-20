
// block 00462340
00462340  and        dword ptr [eax + 4], 0xfffffffe
00462344  xor        edx, edx
00462346  mov        dword ptr [eax + 8], edx
00462349  mov        dword ptr [eax + 0xc], edx
0046234c  mov        dword ptr [eax + 0x10], edx
0046234f  mov        dword ptr [eax], edx
00462351  mov        dword ptr [eax + 0x14], eax
00462354  mov        dword ptr [eax + 0x18], edx
00462357  mov        dword ptr [eax + 0x1c], edx
0046235a  and        dword ptr [eax + 0x28], 0xfffffffe
0046235e  lea        ecx, [eax + 0x24]
00462361  mov        dword ptr [ecx + 8], edx
00462364  mov        dword ptr [ecx + 0xc], edx
00462367  mov        dword ptr [ecx + 0x10], edx
0046236a  mov        dword ptr [ecx], edx
0046236c  mov        dword ptr [ecx + 0x14], ecx
0046236f  mov        dword ptr [ecx + 0x18], edx
00462372  mov        dword ptr [ecx + 0x1c], edx
00462375  mov        dword ptr [eax + 0x48], edx
00462378  ret        
