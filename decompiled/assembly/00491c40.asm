
// block 00491c40
00491c40  mov        edi, edi
00491c42  push       ebp
00491c43  mov        ebp, esp
00491c45  push       ecx
00491c46  push       ebx
00491c47  cld        
00491c48  mov        eax, dword ptr [ebp + 0xc]
00491c4b  mov        ecx, dword ptr [eax + 8]
00491c4e  xor        ecx, dword ptr [ebp + 0xc]
00491c51  call       0x46c932

// block 00491c56
00491c56  mov        eax, dword ptr [ebp + 8]
00491c59  mov        eax, dword ptr [eax + 4]
00491c5c  and        eax, 0x66
00491c5f  je         0x491c72

// block 00491c61
00491c61  mov        eax, dword ptr [ebp + 0xc]
00491c64  mov        dword ptr [eax + 0x24], 1
00491c6b  xor        eax, eax
00491c6d  inc        eax
00491c6e  jmp        0x491cdc

// block 00491c72
00491c72  push       1
00491c74  mov        eax, dword ptr [ebp + 0xc]
00491c77  push       dword ptr [eax + 0x18]
00491c7a  mov        eax, dword ptr [ebp + 0xc]
00491c7d  push       dword ptr [eax + 0x14]
00491c80  mov        eax, dword ptr [ebp + 0xc]
00491c83  push       dword ptr [eax + 0xc]
00491c86  push       0
00491c88  push       dword ptr [ebp + 0x10]
00491c8b  mov        eax, dword ptr [ebp + 0xc]
00491c8e  push       dword ptr [eax + 0x10]
00491c91  push       dword ptr [ebp + 8]
00491c94  call       0x493064

// block 00491c99
00491c99  add        esp, 0x20
00491c9c  mov        eax, dword ptr [ebp + 0xc]
00491c9f  cmp        dword ptr [eax + 0x24], 0
00491ca3  jne        0x491cb0

// block 00491ca5
00491ca5  push       dword ptr [ebp + 8]
00491ca8  push       dword ptr [ebp + 0xc]
00491cab  call       0x491a21

// block 00491cb0
00491cb0  push       0
00491cb2  push       0
00491cb4  push       0
00491cb6  push       0
00491cb8  push       0
00491cba  lea        eax, [ebp - 4]
00491cbd  push       eax
00491cbe  push       0x123
00491cc3  call       0x491b69

// block 00491cc8
00491cc8  add        esp, 0x1c
00491ccb  mov        eax, dword ptr [ebp - 4]
00491cce  mov        ebx, dword ptr [ebp + 0xc]
00491cd1  mov        esp, dword ptr [ebx + 0x1c]
00491cd4  mov        ebp, dword ptr [ebx + 0x20]
00491cd7  jmp        eax

// block 00491cdc
00491cdc  pop        ebx
00491cdd  leave      
00491cde  ret        
