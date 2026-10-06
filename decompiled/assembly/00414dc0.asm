
// block 00414dc0
00414dc0  sub        esp, 0x20
00414dc3  mov        ecx, dword ptr [0x4b43dc]
00414dc9  mov        eax, dword ptr [ecx + 0x68]
00414dcc  push       ebx
00414dcd  push       ebp
00414dce  push       esi
00414dcf  push       edi
00414dd0  mov        dword ptr [esp + 0x1c], ecx
00414dd4  test       eax, eax
00414dd6  je         0x414f1f

// block 00414ddc
00414ddc  mov        ecx, dword ptr [eax + 4]
00414ddf  mov        edi, dword ptr [eax]
00414de1  mov        eax, dword ptr [edi + 0x26f8]
00414de7  mov        dword ptr [esp + 0x18], ecx
00414deb  test       al, 0xa0
00414ded  jne        0x414df6

// block 00414def
00414def  test       eax, 0x600400
00414df4  je         0x414e01

// block 00414df6
00414df6  test       eax, 0x100
00414dfb  je         0x414f11

// block 00414e01
00414e01  mov        eax, dword ptr [esp + 0x34]
00414e05  cmp        dword ptr [edi + 0x1274], eax
00414e0b  jne        0x414f11

// block 00414e11
00414e11  mov        eax, dword ptr [edi + 0x26bc]
00414e17  mov        dword ptr [esp + 0x14], eax
00414e1b  test       eax, eax
00414e1d  jl         0x414f03

// block 00414e23
00414e23  test       dword ptr [0x4cee78], 0x8000
00414e2d  mov        ecx, dword ptr [edi + 0x26c0]
00414e33  mov        edx, dword ptr [0x4b43dc]
00414e39  mov        esi, dword ptr [edx + ecx*4 + 0x40]
00414e3d  lea        ebp, [edi + 0x1074]
00414e43  je         0x414e56

// block 00414e45
00414e45  push       0x4cf1d0
00414e4a  call       dword ptr [0x498088]

// block 00414e50
00414e50  inc        byte ptr [0x4cf221]

// block 00414e56
00414e56  inc        dword ptr [esi + 0x130]
00414e5c  call       0x4621c0

// block 00414e61
00414e61  mov        ebx, eax
00414e63  or         dword ptr [ebx + 0x480], 1
00414e6a  mov        dword ptr [ebx + 0x20], 4
00414e71  test       ebp, ebp
00414e73  jne        0x414ea3

// block 00414e75
00414e75  fldz       
00414e77  fst        dword ptr [esp + 0x24]
00414e7b  mov        eax, dword ptr [esp + 0x24]
00414e7f  fst        dword ptr [esp + 0x28]
00414e83  mov        ecx, dword ptr [esp + 0x28]
00414e87  fstp       dword ptr [esp + 0x2c]
00414e8b  mov        edx, dword ptr [esp + 0x2c]
00414e8f  mov        dword ptr [ebx + 0x430], eax
00414e95  mov        dword ptr [ebx + 0x434], ecx
00414e9b  mov        dword ptr [ebx + 0x438], edx
00414ea1  jmp        0x414ed0

// block 00414ea3
00414ea3  fld        dword ptr [ebp]
00414ea6  fadd       qword ptr [0x4a3d70]
00414eac  fadd       qword ptr [0x4a3d28]
00414eb2  fstp       dword ptr [ebx + 0x430]
00414eb8  fld        dword ptr [ebp + 4]
00414ebb  fadd       qword ptr [0x4a3d68]
00414ec1  fstp       dword ptr [ebx + 0x434]
00414ec7  fld        dword ptr [ebp + 8]
00414eca  fstp       dword ptr [ebx + 0x438]

// block 00414ed0
00414ed0  mov        eax, dword ptr [esp + 0x14]
00414ed4  push       eax
00414ed5  push       ebx
00414ed6  mov        ecx, esi
00414ed8  call       0x454d10

// block 00414edd
00414edd  lea        eax, [esp + 0x20]
00414ee1  call       0x461250

// block 00414ee6
00414ee6  test       dword ptr [0x4cee78], 0x8000
00414ef0  je         0x414f03

// block 00414ef2
00414ef2  push       0x4cf1d0
00414ef7  call       dword ptr [0x49808c]

// block 00414efd
00414efd  dec        byte ptr [0x4cf221]

// block 00414f03
00414f03  or         dword ptr [edi + 0x26f8], 0x1000000
00414f0d  mov        ecx, dword ptr [esp + 0x18]

// block 00414f11
00414f11  mov        eax, ecx
00414f13  test       ecx, ecx
00414f15  jne        0x414ddc

// block 00414f1b
00414f1b  mov        ecx, dword ptr [esp + 0x1c]

// block 00414f1f
00414f1f  lea        esi, [ecx + 0x50]
00414f22  call       0x464a80

// block 00414f27
00414f27  pop        edi
00414f28  pop        esi
00414f29  pop        ebp
00414f2a  pop        ebx
00414f2b  add        esp, 0x20
00414f2e  ret        4
