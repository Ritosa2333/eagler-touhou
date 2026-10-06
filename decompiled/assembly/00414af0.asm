
// block 00414af0
00414af0  mov        eax, dword ptr [esi + 0x26b8]
00414af6  sub        esp, 0x10
00414af9  push       ebx
00414afa  push       ebp
00414afb  push       edi
00414afc  test       eax, eax
00414afe  jl         0x414b0f

// block 00414b00
00414b00  fld        dword ptr [esi + 0x1074]
00414b06  push       ecx
00414b07  fstp       dword ptr [esp]
00414b0a  call       0x453e20

// block 00414b0f
00414b0f  mov        eax, dword ptr [esi + 0x26bc]
00414b15  mov        dword ptr [esp + 0xc], eax
00414b19  test       eax, eax
00414b1b  jl         0x414c01

// block 00414b21
00414b21  test       dword ptr [0x4cee78], 0x8000
00414b2b  mov        eax, dword ptr [esi + 0x26c0]
00414b31  mov        ecx, dword ptr [0x4b43dc]
00414b37  mov        edi, dword ptr [ecx + eax*4 + 0x40]
00414b3b  lea        ebp, [esi + 0x1074]
00414b41  je         0x414b54

// block 00414b43
00414b43  push       0x4cf1d0
00414b48  call       dword ptr [0x498088]

// block 00414b4e
00414b4e  inc        byte ptr [0x4cf221]

// block 00414b54
00414b54  inc        dword ptr [edi + 0x130]
00414b5a  call       0x4621c0

// block 00414b5f
00414b5f  mov        ebx, eax
00414b61  or         dword ptr [ebx + 0x480], 1
00414b68  mov        dword ptr [ebx + 0x20], 4
00414b6f  test       ebp, ebp
00414b71  jne        0x414ba1

// block 00414b73
00414b73  fldz       
00414b75  fst        dword ptr [esp + 0x10]
00414b79  mov        edx, dword ptr [esp + 0x10]
00414b7d  fst        dword ptr [esp + 0x14]
00414b81  mov        eax, dword ptr [esp + 0x14]
00414b85  fstp       dword ptr [esp + 0x18]
00414b89  mov        ecx, dword ptr [esp + 0x18]
00414b8d  mov        dword ptr [ebx + 0x430], edx
00414b93  mov        dword ptr [ebx + 0x434], eax
00414b99  mov        dword ptr [ebx + 0x438], ecx
00414b9f  jmp        0x414bce

// block 00414ba1
00414ba1  fld        dword ptr [ebp]
00414ba4  fadd       qword ptr [0x4a3d70]
00414baa  fadd       qword ptr [0x4a3d28]
00414bb0  fstp       dword ptr [ebx + 0x430]
00414bb6  fld        dword ptr [ebp + 4]
00414bb9  fadd       qword ptr [0x4a3d68]
00414bbf  fstp       dword ptr [ebx + 0x434]
00414bc5  fld        dword ptr [ebp + 8]
00414bc8  fstp       dword ptr [ebx + 0x438]

// block 00414bce
00414bce  mov        edx, dword ptr [esp + 0xc]
00414bd2  push       edx
00414bd3  push       ebx
00414bd4  mov        ecx, edi
00414bd6  call       0x454d10

// block 00414bdb
00414bdb  lea        eax, [esp + 0xc]
00414bdf  call       0x461250

// block 00414be4
00414be4  test       dword ptr [0x4cee78], 0x8000
00414bee  je         0x414c01

// block 00414bf0
00414bf0  push       0x4cf1d0
00414bf5  call       dword ptr [0x49808c]

// block 00414bfb
00414bfb  dec        byte ptr [0x4cf221]

// block 00414c01
00414c01  mov        eax, dword ptr [esi + 0x2660]
00414c07  lea        ebx, [esi + 0x2660]
00414c0d  lea        edi, [esi + 0x1074]
00414c13  test       eax, eax
00414c15  je         0x414c34

// block 00414c17
00414c17  fld        dword ptr [0x4a42c0]
00414c1d  sub        esp, 8
00414c20  fstp       dword ptr [esp + 4]
00414c24  fld        dword ptr [0x4a4060]
00414c2a  fstp       dword ptr [esp]
00414c2d  push       edi
00414c2e  push       eax
00414c2f  call       0x4273f0

// block 00414c34
00414c34  push       ebx
00414c35  call       0x412140

// block 00414c3a
00414c3a  mov        dword ptr [ebx], 0
00414c40  mov        eax, dword ptr [esi + 0x103c]
00414c46  test       eax, eax
00414c48  je         0x414c4e

// block 00414c4a
00414c4a  mov        ecx, esi
00414c4c  call       eax

// block 00414c4e
00414c4e  pop        edi
00414c4f  pop        ebp
00414c50  mov        eax, 1
00414c55  pop        ebx
00414c56  add        esp, 0x10
00414c59  ret        
