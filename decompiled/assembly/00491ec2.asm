
// block 00491ec2
00491ec2  push       eax
00491ec3  push       dword ptr fs:[0]
00491eca  lea        eax, [esp + 0xc]
00491ece  sub        esp, dword ptr [esp + 0xc]
00491ed2  push       ebx
00491ed3  push       esi
00491ed4  push       edi
00491ed5  mov        dword ptr [eax], ebp
00491ed7  mov        ebp, eax
00491ed9  mov        eax, dword ptr [0x4ad138]
00491ede  xor        eax, ebp
00491ee0  push       eax
00491ee1  mov        dword ptr [ebp - 0x10], eax
00491ee4  push       dword ptr [ebp - 4]
00491ee7  mov        dword ptr [ebp - 4], 0xffffffff
00491eee  lea        eax, [ebp - 0xc]
00491ef1  mov        dword ptr fs:[0], eax
00491ef7  ret        
