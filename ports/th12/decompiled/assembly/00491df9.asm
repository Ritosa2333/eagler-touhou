
// block 00491df9
00491df9  mov        edi, edi
00491dfb  push       ebp
00491dfc  mov        ebp, esp
00491dfe  sub        esp, 0x18
00491e01  mov        eax, dword ptr [0x4ad138]
00491e06  and        dword ptr [ebp - 0x18], 0
00491e0a  lea        ecx, [ebp - 0x18]
00491e0d  xor        eax, ecx
00491e0f  mov        ecx, dword ptr [ebp + 8]
00491e12  mov        dword ptr [ebp - 0x10], eax
00491e15  mov        eax, dword ptr [ebp + 0xc]
00491e18  mov        dword ptr [ebp - 0xc], eax
00491e1b  mov        eax, dword ptr [ebp + 0x14]
00491e1e  inc        eax
00491e1f  mov        dword ptr [ebp - 0x14], 0x491b36
00491e26  mov        dword ptr [ebp - 8], ecx
00491e29  mov        dword ptr [ebp - 4], eax
00491e2c  mov        eax, dword ptr fs:[0]
00491e32  mov        dword ptr [ebp - 0x18], eax
00491e35  lea        eax, [ebp - 0x18]
00491e38  mov        dword ptr fs:[0], eax
00491e3e  push       dword ptr [ebp + 0x18]
00491e41  push       ecx
00491e42  push       dword ptr [ebp + 0x10]
00491e45  call       0x493150

// block 00491e4a
00491e4a  mov        ecx, eax
00491e4c  mov        eax, dword ptr [ebp - 0x18]
00491e4f  mov        dword ptr fs:[0], eax
00491e55  mov        eax, ecx
00491e57  leave      
00491e58  ret        
