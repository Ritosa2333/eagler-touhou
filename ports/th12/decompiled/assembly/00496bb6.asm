
// block 00496bb6
00496bb6  mov        edi, edi
00496bb8  push       ebx
00496bb9  mov        ebx, esp
00496bbb  push       ecx
00496bbc  push       ecx
00496bbd  and        esp, 0xfffffff0
00496bc0  add        esp, 4
00496bc3  push       ebp
00496bc4  mov        ebp, dword ptr [ebx + 4]
00496bc7  mov        dword ptr [esp + 4], ebp
00496bcb  mov        ebp, esp
00496bcd  sub        esp, 0x88
00496bd3  mov        eax, dword ptr [0x4ad138]
00496bd8  xor        eax, ebp
00496bda  mov        dword ptr [ebp - 4], eax
00496bdd  mov        eax, dword ptr [ebx + 0x10]
00496be0  movzx      ecx, word ptr [eax]
00496be3  push       esi
00496be4  mov        esi, dword ptr [ebx + 0xc]
00496be7  mov        eax, dword ptr [esi]
00496be9  dec        eax
00496bea  push       edi
00496beb  mov        dword ptr [ebp - 0x88], ecx
00496bf1  je         0x496c45

// block 00496bf3
00496bf3  dec        eax
00496bf4  je         0x496c39

// block 00496bf6
00496bf6  dec        eax
00496bf7  je         0x496c2d

// block 00496bf9
00496bf9  dec        eax
00496bfa  je         0x496c21

// block 00496bfc
00496bfc  dec        eax
00496bfd  je         0x496c45

// block 00496bff
00496bff  dec        eax
00496c00  dec        eax
00496c01  je         0x496c16

// block 00496c03
00496c03  dec        eax
00496c04  jne        0x496caf

// block 00496c0a
00496c0a  mov        dword ptr [ebp - 0x84], 0x10
00496c14  jmp        0x496c4f

// block 00496c16
00496c16  mov        dword ptr [esi], 1
00496c1c  jmp        0x496caf

// block 00496c21
00496c21  mov        dword ptr [ebp - 0x84], 0x12
00496c2b  jmp        0x496c4f

// block 00496c2d
00496c2d  mov        dword ptr [ebp - 0x84], 0x11
00496c37  jmp        0x496c4f

// block 00496c39
00496c39  mov        dword ptr [ebp - 0x84], 4
00496c43  jmp        0x496c4f

// block 00496c45
00496c45  mov        dword ptr [ebp - 0x84], 8

// block 00496c4f
00496c4f  push       ecx
00496c50  lea        edi, [esi + 0x18]
00496c53  push       edi
00496c54  push       dword ptr [ebp - 0x84]
00496c5a  call       0x496491

// block 00496c5f
00496c5f  add        esp, 0xc
00496c62  test       eax, eax
00496c64  jne        0x496caf

// block 00496c66
00496c66  mov        eax, dword ptr [ebx + 8]
00496c69  cmp        eax, 0x10
00496c6c  je         0x496c7e

// block 00496c6e
00496c6e  cmp        eax, 0x16
00496c71  je         0x496c7e

// block 00496c73
00496c73  cmp        eax, 0x1d
00496c76  je         0x496c7e

// block 00496c78
00496c78  and        dword ptr [ebp - 0x40], 0xfffffffe
00496c7c  jmp        0x496c90

// block 00496c7e
00496c7e  mov        ecx, dword ptr [ebp - 0x40]
00496c81  fld        qword ptr [esi + 0x10]
00496c84  and        ecx, 0xffffffe3
00496c87  fstp       qword ptr [ebp - 0x50]
00496c8a  or         ecx, 3
00496c8d  mov        dword ptr [ebp - 0x40], ecx

// block 00496c90
00496c90  push       edi
00496c91  lea        ecx, [esi + 8]
00496c94  push       ecx
00496c95  push       eax
00496c96  push       dword ptr [ebp - 0x84]
00496c9c  lea        eax, [ebp - 0x88]
00496ca2  push       eax
00496ca3  lea        eax, [ebp - 0x80]
00496ca6  push       eax
00496ca7  call       0x49644b

// block 00496cac
00496cac  add        esp, 0x18

// block 00496caf
00496caf  push       0xffff
00496cb4  push       dword ptr [ebp - 0x88]
00496cba  call       0x491181

// block 00496cbf
00496cbf  cmp        dword ptr [esi], 8
00496cc2  pop        ecx
00496cc3  pop        ecx
00496cc4  je         0x496cda

// block 00496cc6
00496cc6  cmp        dword ptr [0x4b37a0], 0
00496ccd  jne        0x496cda

// block 00496ccf
00496ccf  push       esi
00496cd0  call       0x49616c

// block 00496cd5
00496cd5  pop        ecx
00496cd6  test       eax, eax
00496cd8  jne        0x496ce2

// block 00496cda
00496cda  push       dword ptr [esi]
00496cdc  call       0x496673

// block 00496ce1
00496ce1  pop        ecx

// block 00496ce2
00496ce2  mov        ecx, dword ptr [ebp - 4]
00496ce5  pop        edi
00496ce6  xor        ecx, ebp
00496ce8  pop        esi
00496ce9  call       0x46c932

// block 00496cee
00496cee  mov        esp, ebp
00496cf0  pop        ebp
00496cf1  mov        esp, ebx
00496cf3  pop        ebx
00496cf4  ret        
