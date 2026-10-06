
// block 00408cb0
00408cb0  fld        dword ptr [0x4a3d78]
00408cb6  sub        esp, 0x10
00408cb9  push       ebx
00408cba  push       ebp
00408cbb  mov        ebp, dword ptr [esp + 0x1c]
00408cbf  fstp       dword ptr [ebp + 0x514]
00408cc5  push       esi
00408cc6  mov        esi, dword ptr [0x4b4514]
00408ccc  fld        dword ptr [0x4a4114]
00408cd2  fstp       dword ptr [ebp + 0x518]
00408cd8  mov        eax, dword ptr [esi + 0x97c]
00408cde  push       edi
00408cdf  lea        edi, [ebp + 0x508]
00408ce5  mov        dword ptr [edi], eax
00408ce7  mov        ecx, dword ptr [esi + 0x980]
00408ced  mov        dword ptr [edi + 4], ecx
00408cf0  mov        edx, dword ptr [esi + 0x984]
00408cf6  mov        dword ptr [edi + 8], edx
00408cf9  mov        edx, 0x1f
00408cfe  call       0x453d90

// block 00408d03
00408d03  test       dword ptr [0x4cee78], 0x8000
00408d0d  mov        esi, dword ptr [esi + 0x10]
00408d10  je         0x408d23

// block 00408d12
00408d12  push       0x4cf1d0
00408d17  call       dword ptr [0x498088]

// block 00408d1d
00408d1d  inc        byte ptr [0x4cf221]

// block 00408d23
00408d23  inc        dword ptr [esi + 0x130]
00408d29  call       0x4621c0

// block 00408d2e
00408d2e  fldz       
00408d30  mov        ebx, eax
00408d32  or         dword ptr [ebx + 0x480], 1
00408d39  mov        dword ptr [ebx + 0x20], 0x17
00408d40  test       edi, edi
00408d42  jne        0x408d70

// block 00408d44
00408d44  fst        dword ptr [esp + 0x14]
00408d48  mov        eax, dword ptr [esp + 0x14]
00408d4c  fst        dword ptr [esp + 0x18]
00408d50  mov        ecx, dword ptr [esp + 0x18]
00408d54  fstp       dword ptr [esp + 0x1c]
00408d58  mov        edx, dword ptr [esp + 0x1c]
00408d5c  mov        dword ptr [ebx + 0x430], eax
00408d62  mov        dword ptr [ebx + 0x434], ecx
00408d68  mov        dword ptr [ebx + 0x438], edx
00408d6e  jmp        0x408d9e

// block 00408d70
00408d70  fstp       st(0)
00408d72  fld        dword ptr [edi]
00408d74  fadd       qword ptr [0x4a3d70]
00408d7a  fadd       qword ptr [0x4a3d28]
00408d80  fstp       dword ptr [ebx + 0x430]
00408d86  fld        dword ptr [edi + 4]
00408d89  fadd       qword ptr [0x4a3d68]
00408d8f  fstp       dword ptr [ebx + 0x434]
00408d95  fld        dword ptr [edi + 8]
00408d98  fstp       dword ptr [ebx + 0x438]

// block 00408d9e
00408d9e  push       0x15
00408da0  push       ebx
00408da1  mov        ecx, esi
00408da3  call       0x454d10

// block 00408da8
00408da8  lea        eax, [esp + 0x24]
00408dac  call       0x461250

// block 00408db1
00408db1  test       dword ptr [0x4cee78], 0x8000
00408dbb  mov        esi, dword ptr [eax]
00408dbd  je         0x408dd0

// block 00408dbf
00408dbf  push       0x4cf1d0
00408dc4  call       dword ptr [0x49808c]

// block 00408dca
00408dca  dec        byte ptr [0x4cf221]

// block 00408dd0
00408dd0  mov        ecx, dword ptr [0x4b43cc]
00408dd6  mov        dword ptr [ebp + 0x40], esi
00408dd9  mov        eax, dword ptr [ecx + 0x7c]
00408ddc  test       al, 1
00408dde  je         0x408e07

// block 00408de0
00408de0  cmp        dword ptr [ecx + 0x28], 0x3c
00408de4  jl         0x408df5

// block 00408de6
00408de6  mov        dword ptr [ecx + 0x80], 0
00408df0  and        eax, 0xffffffdd
00408df3  jmp        0x408e04

// block 00408df5
00408df5  mov        edx, dword ptr [0x4b43c4]
00408dfb  cmp        dword ptr [edx + 0x3c], 0
00408dff  je         0x408e07

// block 00408e01
00408e01  or         eax, 0x20

// block 00408e04
00408e04  mov        dword ptr [ecx + 0x7c], eax

// block 00408e07
00408e07  mov        eax, dword ptr [0x4b43dc]
00408e0c  mov        esi, 1
00408e11  add        dword ptr [eax + 0x14], esi
00408e14  test       dword ptr [0x4cee78], 0x8000
00408e1e  mov        eax, dword ptr [0x4b43e4]
00408e23  mov        ebx, dword ptr [eax + 0x6d44]
00408e29  je         0x408e3c

// block 00408e2b
00408e2b  push       0x4cf1d0
00408e30  call       dword ptr [0x498088]

// block 00408e36
00408e36  inc        byte ptr [0x4cf221]

// block 00408e3c
00408e3c  add        dword ptr [ebx + 0x130], esi
00408e42  call       0x4621c0

// block 00408e47
00408e47  mov        esi, eax
00408e49  or         dword ptr [esi + 0x480], 1
00408e50  mov        dword ptr [esi + 0x20], 0x17
00408e57  test       edi, edi
00408e59  jne        0x408e89

// block 00408e5b
00408e5b  fldz       
00408e5d  fst        dword ptr [esp + 0x14]
00408e61  mov        ecx, dword ptr [esp + 0x14]
00408e65  fst        dword ptr [esp + 0x18]
00408e69  mov        edx, dword ptr [esp + 0x18]
00408e6d  fstp       dword ptr [esp + 0x1c]
00408e71  mov        eax, dword ptr [esp + 0x1c]
00408e75  mov        dword ptr [esi + 0x430], ecx
00408e7b  mov        dword ptr [esi + 0x434], edx
00408e81  mov        dword ptr [esi + 0x438], eax
00408e87  jmp        0x408eb5

// block 00408e89
00408e89  fld        dword ptr [edi]
00408e8b  fadd       qword ptr [0x4a3d70]
00408e91  fadd       qword ptr [0x4a3d28]
00408e97  fstp       dword ptr [esi + 0x430]
00408e9d  fld        dword ptr [edi + 4]
00408ea0  fadd       qword ptr [0x4a3d68]
00408ea6  fstp       dword ptr [esi + 0x434]
00408eac  fld        dword ptr [edi + 8]
00408eaf  fstp       dword ptr [esi + 0x438]

// block 00408eb5
00408eb5  push       1
00408eb7  push       esi
00408eb8  mov        ecx, ebx
00408eba  call       0x454d10

// block 00408ebf
00408ebf  mov        ebx, esi
00408ec1  lea        eax, [esp + 0x24]
00408ec5  call       0x461250

// block 00408eca
00408eca  test       dword ptr [0x4cee78], 0x8000
00408ed4  mov        esi, dword ptr [eax]
00408ed6  je         0x408ee9

// block 00408ed8
00408ed8  push       0x4cf1d0
00408edd  call       dword ptr [0x49808c]

// block 00408ee3
00408ee3  dec        byte ptr [0x4cf221]

// block 00408ee9
00408ee9  pop        edi
00408eea  mov        dword ptr [ebp + 0x48], esi
00408eed  pop        esi
00408eee  pop        ebp
00408eef  xor        eax, eax
00408ef1  pop        ebx
00408ef2  add        esp, 0x10
00408ef5  ret        4
