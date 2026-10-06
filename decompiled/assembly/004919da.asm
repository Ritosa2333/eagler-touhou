
// block 004919da
004919da  mov        edi, edi
004919dc  push       ebp
004919dd  mov        ebp, esp
004919df  push       ecx
004919e0  push       ebx
004919e1  mov        eax, dword ptr [ebp + 0xc]
004919e4  add        eax, 0xc
004919e7  mov        dword ptr [ebp - 4], eax
004919ea  mov        ebx, dword ptr fs:[0]
004919f1  mov        eax, dword ptr [ebx]
004919f3  mov        dword ptr fs:[0], eax
004919f9  mov        eax, dword ptr [ebp + 8]
004919fc  mov        ebx, dword ptr [ebp + 0xc]
004919ff  mov        ebp, dword ptr [ebp - 4]
00491a02  mov        esp, dword ptr [ebx - 4]
00491a05  jmp        eax
