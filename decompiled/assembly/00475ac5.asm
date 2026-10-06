
// block 00475ac5
00475ac5  mov        edi, edi
00475ac7  push       ebp
00475ac8  mov        ebp, esp
00475aca  mov        eax, dword ptr [ebp + 8]
00475acd  mov        edx, dword ptr [eax + 8]
00475ad0  mov        ecx, dword ptr [eax + 4]
00475ad3  push       esi
00475ad4  push       edi
00475ad5  mov        edi, ecx
00475ad7  mov        esi, edx
00475ad9  shr        ecx, 1
00475adb  shl        esi, 0x1f
00475ade  or         ecx, esi
00475ae0  mov        dword ptr [eax + 4], ecx
00475ae3  mov        ecx, dword ptr [eax]
00475ae5  shl        edi, 0x1f
00475ae8  shr        edx, 1
00475aea  shr        ecx, 1
00475aec  or         ecx, edi
00475aee  pop        edi
00475aef  mov        dword ptr [eax + 8], edx
00475af2  mov        dword ptr [eax], ecx
00475af4  pop        esi
00475af5  pop        ebp
00475af6  ret        
