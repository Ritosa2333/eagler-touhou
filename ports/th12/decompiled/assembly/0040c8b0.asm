
// block 0040c8b0
0040c8b0  movzx      eax, word ptr [esi + 0x532]
0040c8b7  sub        esp, 0x10
0040c8ba  push       ebx
0040c8bb  push       ebp
0040c8bc  push       edi
0040c8bd  cmp        ax, 2
0040c8c1  je         0x40c8d2

// block 0040c8c3
0040c8c3  cmp        ax, 1
0040c8c7  je         0x40c8d2

// block 0040c8c9
0040c8c9  xor        eax, eax
0040c8cb  pop        edi
0040c8cc  pop        ebp
0040c8cd  pop        ebx
0040c8ce  add        esp, 0x10
0040c8d1  ret        

// block 0040c8d2
0040c8d2  fld        dword ptr [0x4a3e54]
0040c8d8  sub        esp, 8
0040c8db  fst        dword ptr [esp + 4]
0040c8df  lea        ecx, [esi + 0x4bc]
0040c8e5  fstp       dword ptr [esp]
0040c8e8  call       0x40d560

// block 0040c8ed
0040c8ed  lea        edi, [esi + 8]
0040c8f0  test       eax, eax
0040c8f2  jne        0x40ca69

// block 0040c8f8
0040c8f8  mov        eax, 3
0040c8fd  mov        word ptr [esi + 0x532], ax
0040c904  mov        eax, dword ptr [edi + 0x494]
0040c90a  test       eax, eax
0040c90c  je         0x40c917

// block 0040c90e
0040c90e  mov        edx, 1
0040c913  mov        ecx, edi
0040c915  call       eax

// block 0040c917
0040c917  mov        ecx, 1
0040c91c  mov        word ptr [edi + 0x3c4], cx
0040c923  mov        ebp, dword ptr [esi + 0x524]
0040c929  test       ebp, ebp
0040c92b  jl         0x40ca08

// block 0040c931
0040c931  test       dword ptr [0x4cee78], 0x8000
0040c93b  mov        edx, dword ptr [0x4b43c8]
0040c941  mov        edi, dword ptr [edx + 0x4debdc]
0040c947  je         0x40c95a

// block 0040c949
0040c949  push       0x4cf1d0
0040c94e  call       dword ptr [0x498088]

// block 0040c954
0040c954  inc        byte ptr [0x4cf221]

// block 0040c95a
0040c95a  inc        dword ptr [edi + 0x130]
0040c960  call       0x4621c0

// block 0040c965
0040c965  mov        ebx, eax
0040c967  or         dword ptr [ebx + 0x480], 1
0040c96e  lea        eax, [esi + 0x4bc]
0040c974  mov        dword ptr [ebx + 0x20], 0x17
0040c97b  test       eax, eax
0040c97d  jne        0x40c9ad

// block 0040c97f
0040c97f  fldz       
0040c981  fst        dword ptr [esp + 0x10]
0040c985  mov        eax, dword ptr [esp + 0x10]
0040c989  fst        dword ptr [esp + 0x14]
0040c98d  mov        ecx, dword ptr [esp + 0x14]
0040c991  fstp       dword ptr [esp + 0x18]
0040c995  mov        edx, dword ptr [esp + 0x18]
0040c999  mov        dword ptr [ebx + 0x430], eax
0040c99f  mov        dword ptr [ebx + 0x434], ecx
0040c9a5  mov        dword ptr [ebx + 0x438], edx
0040c9ab  jmp        0x40c9d9

// block 0040c9ad
0040c9ad  fld        dword ptr [eax]
0040c9af  fadd       qword ptr [0x4a3d70]
0040c9b5  fadd       qword ptr [0x4a3d28]
0040c9bb  fstp       dword ptr [ebx + 0x430]
0040c9c1  fld        dword ptr [eax + 4]
0040c9c4  fadd       qword ptr [0x4a3d68]
0040c9ca  fstp       dword ptr [ebx + 0x434]
0040c9d0  fld        dword ptr [eax + 8]
0040c9d3  fstp       dword ptr [ebx + 0x438]

// block 0040c9d9
0040c9d9  push       ebp
0040c9da  push       ebx
0040c9db  mov        ecx, edi
0040c9dd  call       0x454d10

// block 0040c9e2
0040c9e2  lea        eax, [esp + 0xc]
0040c9e6  call       0x461250

// block 0040c9eb
0040c9eb  test       dword ptr [0x4cee78], 0x8000
0040c9f5  je         0x40ca08

// block 0040c9f7
0040c9f7  push       0x4cf1d0
0040c9fc  call       dword ptr [0x49808c]

// block 0040ca02
0040ca02  dec        byte ptr [0x4cf221]

// block 0040ca08
0040ca08  mov        eax, dword ptr [esi + 0x4f4]
0040ca0e  test       al, 1
0040ca10  jne        0x40ca41

// block 0040ca12
0040ca12  fldz       
0040ca14  or         eax, 1
0040ca17  fstp       dword ptr [esi + 0x4ec]
0040ca1d  mov        dword ptr [esi + 0x4e8], 0
0040ca27  mov        dword ptr [esi + 0x4e4], 0xfff0bdc1
0040ca31  mov        dword ptr [esi + 0x4f0], 0x4b2ed0
0040ca3b  mov        dword ptr [esi + 0x4f4], eax

// block 0040ca41
0040ca41  fldz       
0040ca43  mov        dword ptr [esi + 0x4e8], 0
0040ca4d  fstp       dword ptr [esi + 0x4ec]
0040ca53  mov        dword ptr [esi + 0x4e4], 0xffffffff
0040ca5d  mov        eax, 1
0040ca62  pop        edi
0040ca63  pop        ebp
0040ca64  pop        ebx
0040ca65  add        esp, 0x10
0040ca68  ret        

// block 0040ca69
0040ca69  mov        eax, dword ptr [edi + 0x494]
0040ca6f  test       eax, eax
0040ca71  je         0x40ca7c

// block 0040ca73
0040ca73  mov        edx, 1
0040ca78  mov        ecx, edi
0040ca7a  call       eax

// block 0040ca7c
0040ca7c  mov        eax, 1
0040ca81  mov        word ptr [edi + 0x3c4], ax
0040ca88  or         dword ptr [esi], 8
0040ca8b  pop        edi
0040ca8c  mov        ecx, 3
0040ca91  pop        ebp
0040ca92  mov        word ptr [esi + 0x532], cx
0040ca99  pop        ebx
0040ca9a  add        esp, 0x10
0040ca9d  ret        
