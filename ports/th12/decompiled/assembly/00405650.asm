
// block 00405650
00405650  mov        dword ptr [eax + 0x2778], 1
0040565a  push       esi
0040565b  mov        esi, dword ptr [edx]
0040565d  mov        dword ptr [eax + 0x277c], esi
00405663  mov        esi, dword ptr [edx + 4]
00405666  mov        dword ptr [eax + 0x2780], esi
0040566c  mov        edx, dword ptr [edx + 8]
0040566f  mov        dword ptr [eax + 0x2784], edx
00405675  mov        edx, dword ptr [ecx]
00405677  mov        dword ptr [eax + 0x2788], edx
0040567d  mov        edx, dword ptr [ecx + 4]
00405680  mov        dword ptr [eax + 0x278c], edx
00405686  mov        ecx, dword ptr [ecx + 8]
00405689  mov        dword ptr [eax + 0x2790], ecx
0040568f  pop        esi
00405690  ret        
