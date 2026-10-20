
// block 0047e0c2
0047e0c2  mov        edi, edi
0047e0c4  push       ebp
0047e0c5  mov        ebp, esp
0047e0c7  sub        esp, 0x10
0047e0ca  xor        ecx, ecx
0047e0cc  mov        eax, 0xffff0000
0047e0d1  and        dword ptr [ebp - 4], eax
0047e0d4  and        dword ptr [ebp - 0xc], eax
0047e0d7  push       ecx
0047e0d8  lea        eax, [ebp - 8]
0047e0db  push       eax
0047e0dc  push       ecx
0047e0dd  lea        eax, [ebp - 0x10]
0047e0e0  push       eax
0047e0e1  push       dword ptr [ebp + 8]
0047e0e4  mov        dword ptr [ebp - 8], ecx
0047e0e7  mov        dword ptr [ebp - 0x10], ecx
0047e0ea  call       0x481a74

// block 0047e0ef
0047e0ef  mov        eax, dword ptr [ebp + 8]
0047e0f2  add        esp, 0x14
0047e0f5  leave      
0047e0f6  ret        
