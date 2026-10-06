
// block 00491e59
00491e59  push       eax
00491e5a  push       dword ptr fs:[0]
00491e61  lea        eax, [esp + 0xc]
00491e65  sub        esp, dword ptr [esp + 0xc]
00491e69  push       ebx
00491e6a  push       esi
00491e6b  push       edi
00491e6c  mov        dword ptr [eax], ebp
00491e6e  mov        ebp, eax
00491e70  mov        eax, dword ptr [0x4ad138]
00491e75  xor        eax, ebp
00491e77  push       eax
00491e78  push       dword ptr [ebp - 4]
00491e7b  mov        dword ptr [ebp - 4], 0xffffffff
00491e82  lea        eax, [ebp - 0xc]
00491e85  mov        dword ptr fs:[0], eax
00491e8b  ret        
