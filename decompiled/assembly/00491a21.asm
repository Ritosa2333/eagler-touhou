
// block 00491a21
00491a21  mov        edi, edi
00491a23  push       ebp
00491a24  mov        ebp, esp
00491a26  push       ecx
00491a27  push       ecx
00491a28  push       ebx
00491a29  push       esi
00491a2a  push       edi
00491a2b  mov        esi, dword ptr fs:[0]
00491a32  mov        dword ptr [ebp - 4], esi
00491a35  mov        dword ptr [ebp - 8], 0x491a4c
00491a3c  push       0
00491a3e  push       dword ptr [ebp + 0xc]
00491a41  push       dword ptr [ebp - 8]
00491a44  push       dword ptr [ebp + 8]
00491a47  call       0x49193e

// block 00491a4c
00491a4c  mov        eax, dword ptr [ebp + 0xc]
00491a4f  mov        eax, dword ptr [eax + 4]
00491a52  and        eax, 0xfffffffd
00491a55  mov        ecx, dword ptr [ebp + 0xc]
00491a58  mov        dword ptr [ecx + 4], eax
00491a5b  mov        edi, dword ptr fs:[0]
00491a62  mov        ebx, dword ptr [ebp - 4]
00491a65  mov        dword ptr [ebx], edi
00491a67  mov        dword ptr fs:[0], ebx
00491a6e  pop        edi
00491a6f  pop        esi
00491a70  pop        ebx
00491a71  leave      
00491a72  ret        8
