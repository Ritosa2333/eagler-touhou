
// block 00491e8c
00491e8c  push       eax
00491e8d  push       dword ptr fs:[0]
00491e94  lea        eax, [esp + 0xc]
00491e98  sub        esp, dword ptr [esp + 0xc]
00491e9c  push       ebx
00491e9d  push       esi
00491e9e  push       edi
00491e9f  mov        dword ptr [eax], ebp
00491ea1  mov        ebp, eax
00491ea3  mov        eax, dword ptr [0x4ad138]
00491ea8  xor        eax, ebp
00491eaa  push       eax
00491eab  mov        dword ptr [ebp - 0x10], esp
00491eae  push       dword ptr [ebp - 4]
00491eb1  mov        dword ptr [ebp - 4], 0xffffffff
00491eb8  lea        eax, [ebp - 0xc]
00491ebb  mov        dword ptr fs:[0], eax
00491ec1  ret        
