
// block 00474cbe
00474cbe  mov        edi, edi
00474cc0  push       ebp
00474cc1  mov        ebp, esp
00474cc3  sub        esp, 0x98
00474cc9  mov        eax, dword ptr [0x4ad138]
00474cce  xor        eax, ebp
00474cd0  mov        dword ptr [ebp - 4], eax
00474cd3  mov        eax, dword ptr [ebp + 8]
00474cd6  push       ebx
00474cd7  push       esi
00474cd8  xor        ebx, ebx
00474cda  mov        esi, edx
00474cdc  push       edi
00474cdd  mov        dword ptr [ebp - 0x98], esi
00474ce3  cmp        eax, ebx
00474ce5  je         0x474d03

// block 00474ce7
00474ce7  cmp        ecx, ebx
00474ce9  je         0x474cf7

// block 00474ceb
00474ceb  push       eax
00474cec  call       0x4749bc

// block 00474cf1
00474cf1  pop        ecx
00474cf2  jmp        0x474eca

// block 00474cf7
00474cf7  shl        eax, 4
00474cfa  mov        eax, dword ptr [eax + esi + 0x48]
00474cfe  jmp        0x474eca

// block 00474d03
00474d03  mov        dword ptr [ebp - 0x90], 1
00474d0d  mov        dword ptr [ebp - 0x8c], ebx
00474d13  cmp        ecx, ebx
00474d15  je         0x474ec5

// block 00474d1b
00474d1b  cmp        byte ptr [ecx], 0x4c
00474d1e  jne        0x474e54

// block 00474d24
00474d24  cmp        byte ptr [ecx + 1], 0x43
00474d28  jne        0x474e54

// block 00474d2e
00474d2e  cmp        byte ptr [ecx + 2], 0x5f
00474d32  jne        0x474e54

// block 00474d38
00474d38  mov        edi, ecx

// block 00474d3a
00474d3a  push       0x49d508
00474d3f  push       edi
00474d40  call       0x488df0

// block 00474d45
00474d45  mov        ebx, eax
00474d47  pop        ecx
00474d48  pop        ecx
00474d49  test       ebx, ebx
00474d4b  je         0x474e50

// block 00474d51
00474d51  sub        eax, edi
00474d53  mov        dword ptr [ebp - 0x90], eax
00474d59  je         0x474e50

// block 00474d5f
00474d5f  cmp        byte ptr [ebx], 0x3b
00474d62  je         0x474e50

// block 00474d68
00474d68  mov        dword ptr [ebp - 0x94], 1
00474d72  mov        esi, 0x49d43c
00474d77  jmp        0x474d7f

// block 00474d79
00474d79  mov        eax, dword ptr [ebp - 0x90]

// block 00474d7f
00474d7f  push       eax
00474d80  push       edi
00474d81  push       dword ptr [esi]
00474d83  call       0x46dc03

// block 00474d88
00474d88  add        esp, 0xc
00474d8b  test       eax, eax
00474d8d  jne        0x474d9f

// block 00474d8f
00474d8f  push       dword ptr [esi]
00474d91  call       0x475860

// block 00474d96
00474d96  pop        ecx
00474d97  cmp        dword ptr [ebp - 0x90], eax
00474d9d  je         0x474db0

// block 00474d9f
00474d9f  inc        dword ptr [ebp - 0x94]
00474da5  add        esi, 0xc
00474da8  cmp        esi, 0x49d46c
00474dae  jle        0x474d79

// block 00474db0
00474db0  inc        ebx
00474db1  push       0x49d4fc
00474db6  push       ebx
00474db7  call       0x4859c0

// block 00474dbc
00474dbc  mov        edi, eax
00474dbe  xor        esi, esi
00474dc0  pop        ecx
00474dc1  pop        ecx
00474dc2  cmp        edi, esi
00474dc4  jne        0x474dcf

// block 00474dc6
00474dc6  cmp        byte ptr [ebx], 0x3b
00474dc9  jne        0x474e50

// block 00474dcf
00474dcf  cmp        dword ptr [ebp - 0x94], 5
00474dd6  jg         0x474e29

// block 00474dd8
00474dd8  push       edi
00474dd9  push       ebx
00474dda  lea        eax, [ebp - 0x88]
00474de0  push       0x83
00474de5  push       eax
00474de6  call       0x4830fd

// block 00474deb
00474deb  add        esp, 0x10
00474dee  test       eax, eax
00474df0  je         0x474dff

// block 00474df2
00474df2  push       esi
00474df3  push       esi
00474df4  push       esi
00474df5  push       esi
00474df6  push       esi
00474df7  call       0x470dc6

// block 00474dfc
00474dfc  add        esp, 0x14

// block 00474dff
00474dff  push       dword ptr [ebp - 0x94]
00474e05  mov        esi, dword ptr [ebp - 0x98]
00474e0b  lea        ecx, [ebp - 0x88]
00474e11  mov        byte ptr [ebp + edi - 0x88], 0
00474e19  call       0x4749bc

// block 00474e1e
00474e1e  pop        ecx
00474e1f  test       eax, eax
00474e21  je         0x474e29

// block 00474e23
00474e23  inc        dword ptr [ebp - 0x8c]

// block 00474e29
00474e29  add        edi, ebx
00474e2b  cmp        byte ptr [edi], 0
00474e2e  je         0x474e3a

// block 00474e30
00474e30  inc        edi
00474e31  cmp        byte ptr [edi], 0
00474e34  jne        0x474d3a

// block 00474e3a
00474e3a  xor        eax, eax
00474e3c  cmp        dword ptr [ebp - 0x8c], eax
00474e42  je         0x474eca

// block 00474e48
00474e48  mov        esi, dword ptr [ebp - 0x98]
00474e4e  jmp        0x474ec5

// block 00474e50
00474e50  xor        eax, eax
00474e52  jmp        0x474eca

// block 00474e54
00474e54  push       ebx
00474e55  push       ebx
00474e56  push       ebx
00474e57  push       0x83
00474e5c  lea        eax, [ebp - 0x88]
00474e62  push       eax
00474e63  push       ecx
00474e64  call       0x47478b

// block 00474e69
00474e69  add        esp, 0x18
00474e6c  cmp        eax, ebx
00474e6e  je         0x474eca

// block 00474e70
00474e70  lea        edi, [esi + 0x48]

// block 00474e73
00474e73  test       ebx, ebx
00474e75  je         0x474eaa

// block 00474e77
00474e77  push       dword ptr [edi]
00474e79  lea        eax, [ebp - 0x88]
00474e7f  push       eax
00474e80  call       0x472ac0

// block 00474e85
00474e85  pop        ecx
00474e86  pop        ecx
00474e87  test       eax, eax
00474e89  je         0x474ea4

// block 00474e8b
00474e8b  push       ebx
00474e8c  lea        ecx, [ebp - 0x88]
00474e92  call       0x4749bc

// block 00474e97
00474e97  pop        ecx
00474e98  test       eax, eax
00474e9a  jne        0x474ea4

// block 00474e9c
00474e9c  and        dword ptr [ebp - 0x90], eax
00474ea2  jmp        0x474eaa

// block 00474ea4
00474ea4  inc        dword ptr [ebp - 0x8c]

// block 00474eaa
00474eaa  inc        ebx
00474eab  add        edi, 0x10
00474eae  cmp        ebx, 5
00474eb1  jle        0x474e73

// block 00474eb3
00474eb3  xor        eax, eax
00474eb5  cmp        dword ptr [ebp - 0x90], eax
00474ebb  jne        0x474ec5

// block 00474ebd
00474ebd  cmp        dword ptr [ebp - 0x8c], eax
00474ec3  je         0x474eca

// block 00474ec5
00474ec5  call       0x47460e

// block 00474eca
00474eca  mov        ecx, dword ptr [ebp - 4]
00474ecd  pop        edi
00474ece  pop        esi
00474ecf  xor        ecx, ebp
00474ed1  pop        ebx
00474ed2  call       0x46c932

// block 00474ed7
00474ed7  leave      
00474ed8  ret        
