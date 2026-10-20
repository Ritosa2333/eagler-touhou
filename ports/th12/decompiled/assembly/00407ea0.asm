
// block 00407ea0
00407ea0  mov        ecx, dword ptr [eax]
00407ea2  mov        dword ptr [esi + 0x10], ecx
00407ea5  mov        edx, dword ptr [eax + 4]
00407ea8  mov        ecx, dword ptr [0x4b4514]
00407eae  mov        dword ptr [esi + 0x14], edx
00407eb1  mov        eax, dword ptr [eax + 8]
00407eb4  sub        esp, 0x10
00407eb7  push       ebp
00407eb8  mov        dword ptr [esi + 0x18], eax
00407ebb  test       dword ptr [0x4cee78], 0x8000
00407ec5  push       edi
00407ec6  mov        edi, dword ptr [ecx + 0x10]
00407ec9  lea        ebp, [esi + 4]
00407ecc  je         0x407edf

// block 00407ece
00407ece  push       0x4cf1d0
00407ed3  call       dword ptr [0x498088]

// block 00407ed9
00407ed9  inc        byte ptr [0x4cf221]

// block 00407edf
00407edf  inc        dword ptr [edi + 0x130]
00407ee5  push       ebx
00407ee6  call       0x4621c0

// block 00407eeb
00407eeb  fldz       
00407eed  mov        ebx, eax
00407eef  or         dword ptr [ebx + 0x480], 1
00407ef6  mov        dword ptr [ebx + 0x20], 0x17
00407efd  test       ebp, ebp
00407eff  jne        0x407f2d

// block 00407f01
00407f01  fst        dword ptr [esp + 0x10]
00407f05  mov        edx, dword ptr [esp + 0x10]
00407f09  fst        dword ptr [esp + 0x14]
00407f0d  mov        eax, dword ptr [esp + 0x14]
00407f11  fstp       dword ptr [esp + 0x18]
00407f15  mov        ecx, dword ptr [esp + 0x18]
00407f19  mov        dword ptr [ebx + 0x430], edx
00407f1f  mov        dword ptr [ebx + 0x434], eax
00407f25  mov        dword ptr [ebx + 0x438], ecx
00407f2b  jmp        0x407f5c

// block 00407f2d
00407f2d  fstp       st(0)
00407f2f  fld        dword ptr [ebp]
00407f32  fadd       qword ptr [0x4a3d70]
00407f38  fadd       qword ptr [0x4a3d28]
00407f3e  fstp       dword ptr [ebx + 0x430]
00407f44  fld        dword ptr [ebp + 4]
00407f47  fadd       qword ptr [0x4a3d68]
00407f4d  fstp       dword ptr [ebx + 0x434]
00407f53  fld        dword ptr [ebp + 8]
00407f56  fstp       dword ptr [ebx + 0x438]

// block 00407f5c
00407f5c  push       0x18
00407f5e  push       ebx
00407f5f  mov        ecx, edi
00407f61  call       0x454d10

// block 00407f66
00407f66  lea        eax, [esp + 0xc]
00407f6a  call       0x461250

// block 00407f6f
00407f6f  test       dword ptr [0x4cee78], 0x8000
00407f79  mov        edi, dword ptr [eax]
00407f7b  pop        ebx
00407f7c  je         0x407f8f

// block 00407f7e
00407f7e  push       0x4cf1d0
00407f83  call       dword ptr [0x49808c]

// block 00407f89
00407f89  dec        byte ptr [0x4cf221]

// block 00407f8f
00407f8f  fldz       
00407f91  mov        dword ptr [esi], edi
00407f93  mov        dword ptr [esi + 0x84], 1
00407f9d  mov        eax, dword ptr [esi + 0x98]
00407fa3  test       al, 1
00407fa5  jne        0x407fd4

// block 00407fa7
00407fa7  or         eax, 1
00407faa  fst        dword ptr [esi + 0x90]
00407fb0  mov        dword ptr [esi + 0x8c], 0
00407fba  mov        dword ptr [esi + 0x88], 0xfff0bdc1
00407fc4  mov        dword ptr [esi + 0x94], 0x4b2ed0
00407fce  mov        dword ptr [esi + 0x98], eax

// block 00407fd4
00407fd4  mov        edx, dword ptr [esp + 0x1c]
00407fd8  fst        dword ptr [esi + 0x90]
00407fde  mov        eax, dword ptr [0x4b4514]
00407fe3  push       0xa
00407fe5  push       0x270f
00407fea  sub        esp, 8
00407fed  fstp       dword ptr [esp + 4]
00407ff1  mov        dword ptr [esi + 0x8c], 0
00407ffb  fld        dword ptr [0x4a42d0]
00408001  mov        dword ptr [esi + 0x88], 0xffffffff
0040800b  fstp       dword ptr [esp]
0040800e  push       ebp
0040800f  mov        dword ptr [esi + 0xac], edx
00408015  call       0x4390f0

// block 0040801a
0040801a  pop        edi
0040801b  mov        dword ptr [esi + 0xb0], eax
00408021  pop        ebp
00408022  add        esp, 0x10
00408025  ret        4
