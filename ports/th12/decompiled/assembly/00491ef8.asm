
// block 00491ef8
00491ef8  push       eax
00491ef9  push       dword ptr fs:[0]
00491f00  lea        eax, [esp + 0xc]
00491f04  sub        esp, dword ptr [esp + 0xc]
00491f08  push       ebx
00491f09  push       esi
00491f0a  push       edi
00491f0b  mov        dword ptr [eax], ebp
00491f0d  mov        ebp, eax
00491f0f  mov        eax, dword ptr [0x4ad138]
00491f14  xor        eax, ebp
00491f16  push       eax
00491f17  mov        dword ptr [ebp - 0x14], eax
00491f1a  mov        dword ptr [ebp - 0x10], esp
00491f1d  push       dword ptr [ebp - 4]
00491f20  mov        dword ptr [ebp - 4], 0xffffffff
00491f27  lea        eax, [ebp - 0xc]
00491f2a  mov        dword ptr fs:[0], eax
00491f30  ret        
