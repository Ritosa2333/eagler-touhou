
// block 00414c60
00414c60  sub        esp, 0x1c
00414c63  mov        ecx, dword ptr [0x4b43dc]
00414c69  mov        eax, dword ptr [ecx + 0x68]
00414c6c  push       ebx
00414c6d  push       ebp
00414c6e  push       esi
00414c6f  push       edi
00414c70  mov        dword ptr [esp + 0x18], ecx
00414c74  test       eax, eax
00414c76  je         0x414dae

// block 00414c7c
00414c7c  mov        ecx, dword ptr [eax + 4]
00414c7f  mov        ebp, dword ptr [eax]
00414c81  mov        eax, dword ptr [ebp + 0x26f8]
00414c87  mov        dword ptr [esp + 0x14], ecx
00414c8b  test       al, 0xa0
00414c8d  jne        0x414c96

// block 00414c8f
00414c8f  test       eax, 0x600400
00414c94  je         0x414ca1

// block 00414c96
00414c96  test       eax, 0x100
00414c9b  je         0x414da0

// block 00414ca1
00414ca1  mov        eax, dword ptr [ebp + 0x26bc]
00414ca7  mov        dword ptr [esp + 0x10], eax
00414cab  test       eax, eax
00414cad  jl         0x414d92

// block 00414cb3
00414cb3  test       dword ptr [0x4cee78], 0x8000
00414cbd  mov        eax, dword ptr [ebp + 0x26c0]
00414cc3  mov        ecx, dword ptr [0x4b43dc]
00414cc9  mov        esi, dword ptr [ecx + eax*4 + 0x40]
00414ccd  lea        edi, [ebp + 0x1074]
00414cd3  je         0x414ce6

// block 00414cd5
00414cd5  push       0x4cf1d0
00414cda  call       dword ptr [0x498088]

// block 00414ce0
00414ce0  inc        byte ptr [0x4cf221]

// block 00414ce6
00414ce6  inc        dword ptr [esi + 0x130]
00414cec  call       0x4621c0

// block 00414cf1
00414cf1  mov        ebx, eax
00414cf3  or         dword ptr [ebx + 0x480], 1
00414cfa  mov        dword ptr [ebx + 0x20], 4
00414d01  test       edi, edi
00414d03  jne        0x414d33

// block 00414d05
00414d05  fldz       
00414d07  fst        dword ptr [esp + 0x20]
00414d0b  mov        edx, dword ptr [esp + 0x20]
00414d0f  fst        dword ptr [esp + 0x24]
00414d13  mov        eax, dword ptr [esp + 0x24]
00414d17  fstp       dword ptr [esp + 0x28]
00414d1b  mov        ecx, dword ptr [esp + 0x28]
00414d1f  mov        dword ptr [ebx + 0x430], edx
00414d25  mov        dword ptr [ebx + 0x434], eax
00414d2b  mov        dword ptr [ebx + 0x438], ecx
00414d31  jmp        0x414d5f

// block 00414d33
00414d33  fld        dword ptr [edi]
00414d35  fadd       qword ptr [0x4a3d70]
00414d3b  fadd       qword ptr [0x4a3d28]
00414d41  fstp       dword ptr [ebx + 0x430]
00414d47  fld        dword ptr [edi + 4]
00414d4a  fadd       qword ptr [0x4a3d68]
00414d50  fstp       dword ptr [ebx + 0x434]
00414d56  fld        dword ptr [edi + 8]
00414d59  fstp       dword ptr [ebx + 0x438]

// block 00414d5f
00414d5f  mov        edx, dword ptr [esp + 0x10]
00414d63  push       edx
00414d64  push       ebx
00414d65  mov        ecx, esi
00414d67  call       0x454d10

// block 00414d6c
00414d6c  lea        eax, [esp + 0x1c]
00414d70  call       0x461250

// block 00414d75
00414d75  test       dword ptr [0x4cee78], 0x8000
00414d7f  je         0x414d92

// block 00414d81
00414d81  push       0x4cf1d0
00414d86  call       dword ptr [0x49808c]

// block 00414d8c
00414d8c  dec        byte ptr [0x4cf221]

// block 00414d92
00414d92  or         dword ptr [ebp + 0x26f8], 0x1000000
00414d9c  mov        ecx, dword ptr [esp + 0x14]

// block 00414da0
00414da0  mov        eax, ecx
00414da2  test       ecx, ecx
00414da4  jne        0x414c7c

// block 00414daa
00414daa  mov        ecx, dword ptr [esp + 0x18]

// block 00414dae
00414dae  lea        esi, [ecx + 0x50]
00414db1  call       0x464a80

// block 00414db6
00414db6  pop        edi
00414db7  pop        esi
00414db8  pop        ebp
00414db9  pop        ebx
00414dba  add        esp, 0x1c
00414dbd  ret        
