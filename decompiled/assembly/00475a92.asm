
// block 00475a92
00475a92  mov        edi, edi
00475a94  push       ebp
00475a95  mov        ebp, esp
00475a97  mov        eax, dword ptr [ebp + 8]
00475a9a  push       esi
00475a9b  mov        esi, dword ptr [eax]
00475a9d  mov        ecx, esi
00475a9f  add        esi, esi
00475aa1  push       edi
00475aa2  mov        edi, dword ptr [eax + 4]
00475aa5  shr        ecx, 0x1f
00475aa8  mov        dword ptr [eax], esi
00475aaa  lea        esi, [edi + edi]
00475aad  or         esi, ecx
00475aaf  mov        ecx, dword ptr [eax + 8]
00475ab2  mov        edx, edi
00475ab4  shr        edx, 0x1f
00475ab7  add        ecx, ecx
00475ab9  or         ecx, edx
00475abb  pop        edi
00475abc  mov        dword ptr [eax + 4], esi
00475abf  mov        dword ptr [eax + 8], ecx
00475ac2  pop        esi
00475ac3  pop        ebp
00475ac4  ret        
