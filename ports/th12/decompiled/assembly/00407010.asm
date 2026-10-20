
// block 00407010
00407010  fld        dword ptr [0x4a3d78]
00407016  sub        esp, 0x10
00407019  push       ebx
0040701a  push       ebp
0040701b  mov        ebp, dword ptr [esp + 0x1c]
0040701f  fstp       dword ptr [ebp + 0x514]
00407025  push       esi
00407026  mov        esi, dword ptr [0x4b4514]
0040702c  fld        dword ptr [0x4a4114]
00407032  fstp       dword ptr [ebp + 0x518]
00407038  mov        eax, dword ptr [esi + 0x97c]
0040703e  push       edi
0040703f  lea        edi, [ebp + 0x508]
00407045  mov        dword ptr [edi], eax
00407047  mov        ecx, dword ptr [esi + 0x980]
0040704d  mov        dword ptr [edi + 4], ecx
00407050  mov        edx, dword ptr [esi + 0x984]
00407056  mov        dword ptr [edi + 8], edx
00407059  mov        edx, 0x33
0040705e  call       0x453d90

// block 00407063
00407063  test       dword ptr [0x4cee78], 0x8000
0040706d  mov        esi, dword ptr [esi + 0x10]
00407070  je         0x407083

// block 00407072
00407072  push       0x4cf1d0
00407077  call       dword ptr [0x498088]

// block 0040707d
0040707d  inc        byte ptr [0x4cf221]

// block 00407083
00407083  inc        dword ptr [esi + 0x130]
00407089  call       0x4621c0

// block 0040708e
0040708e  fldz       
00407090  mov        ebx, eax
00407092  or         dword ptr [ebx + 0x480], 1
00407099  mov        dword ptr [ebx + 0x20], 0x17
004070a0  test       edi, edi
004070a2  jne        0x4070d0

// block 004070a4
004070a4  fst        dword ptr [esp + 0x14]
004070a8  mov        eax, dword ptr [esp + 0x14]
004070ac  fst        dword ptr [esp + 0x18]
004070b0  mov        ecx, dword ptr [esp + 0x18]
004070b4  fstp       dword ptr [esp + 0x1c]
004070b8  mov        edx, dword ptr [esp + 0x1c]
004070bc  mov        dword ptr [ebx + 0x430], eax
004070c2  mov        dword ptr [ebx + 0x434], ecx
004070c8  mov        dword ptr [ebx + 0x438], edx
004070ce  jmp        0x4070fe

// block 004070d0
004070d0  fstp       st(0)
004070d2  fld        dword ptr [edi]
004070d4  fadd       qword ptr [0x4a3d70]
004070da  fadd       qword ptr [0x4a3d28]
004070e0  fstp       dword ptr [ebx + 0x430]
004070e6  fld        dword ptr [edi + 4]
004070e9  fadd       qword ptr [0x4a3d68]
004070ef  fstp       dword ptr [ebx + 0x434]
004070f5  fld        dword ptr [edi + 8]
004070f8  fstp       dword ptr [ebx + 0x438]

// block 004070fe
004070fe  push       0x10
00407100  push       ebx
00407101  mov        ecx, esi
00407103  call       0x454d10

// block 00407108
00407108  lea        eax, [esp + 0x24]
0040710c  call       0x461250

// block 00407111
00407111  test       dword ptr [0x4cee78], 0x8000
0040711b  mov        esi, dword ptr [eax]
0040711d  je         0x407130

// block 0040711f
0040711f  push       0x4cf1d0
00407124  call       dword ptr [0x49808c]

// block 0040712a
0040712a  dec        byte ptr [0x4cf221]

// block 00407130
00407130  mov        ecx, dword ptr [0x4b43cc]
00407136  mov        dword ptr [ebp + 0x40], esi
00407139  mov        eax, dword ptr [ecx + 0x7c]
0040713c  test       al, 1
0040713e  je         0x407167

// block 00407140
00407140  cmp        dword ptr [ecx + 0x28], 0x3c
00407144  jl         0x407155

// block 00407146
00407146  mov        dword ptr [ecx + 0x80], 0
00407150  and        eax, 0xffffffdd
00407153  jmp        0x407164

// block 00407155
00407155  mov        edx, dword ptr [0x4b43c4]
0040715b  cmp        dword ptr [edx + 0x3c], 0
0040715f  je         0x407167

// block 00407161
00407161  or         eax, 0x20

// block 00407164
00407164  mov        dword ptr [ecx + 0x7c], eax

// block 00407167
00407167  push       0x78
00407169  call       0x406f60

// block 0040716e
0040716e  mov        eax, dword ptr [0x4b43dc]
00407173  inc        dword ptr [eax + 0x14]
00407176  push       0x44
00407178  call       0x46c9ea

// block 0040717d
0040717d  mov        esi, eax
0040717f  add        esp, 4
00407182  test       esi, esi
00407184  je         0x4071af

// block 00407186
00407186  and        dword ptr [esi + 0x40], 0xfffffffe
0040718a  push       0x44
0040718c  push       0
0040718e  push       esi
0040718f  call       0x477420

// block 00407194
00407194  or         dword ptr [esi], 2
00407197  add        esp, 0xc
0040719a  push       0x46
0040719c  push       0x1e
0040719e  push       0xf0
004071a3  push       0x3c
004071a5  push       3
004071a7  push       8
004071a9  push       esi
004071aa  call       0x452a60

// block 004071af
004071af  test       dword ptr [0x4cee78], 0x8000
004071b9  mov        eax, dword ptr [0x4b43e4]
004071be  mov        ebx, dword ptr [eax + 0x6d44]
004071c4  je         0x4071d7

// block 004071c6
004071c6  push       0x4cf1d0
004071cb  call       dword ptr [0x498088]

// block 004071d1
004071d1  inc        byte ptr [0x4cf221]

// block 004071d7
004071d7  inc        dword ptr [ebx + 0x130]
004071dd  call       0x4621c0

// block 004071e2
004071e2  mov        esi, eax
004071e4  or         dword ptr [esi + 0x480], 1
004071eb  mov        dword ptr [esi + 0x20], 0x17
004071f2  test       edi, edi
004071f4  jne        0x407224

// block 004071f6
004071f6  fldz       
004071f8  fst        dword ptr [esp + 0x14]
004071fc  mov        ecx, dword ptr [esp + 0x14]
00407200  fst        dword ptr [esp + 0x18]
00407204  mov        edx, dword ptr [esp + 0x18]
00407208  fstp       dword ptr [esp + 0x1c]
0040720c  mov        eax, dword ptr [esp + 0x1c]
00407210  mov        dword ptr [esi + 0x430], ecx
00407216  mov        dword ptr [esi + 0x434], edx
0040721c  mov        dword ptr [esi + 0x438], eax
00407222  jmp        0x407250

// block 00407224
00407224  fld        dword ptr [edi]
00407226  fadd       qword ptr [0x4a3d70]
0040722c  fadd       qword ptr [0x4a3d28]
00407232  fstp       dword ptr [esi + 0x430]
00407238  fld        dword ptr [edi + 4]
0040723b  fadd       qword ptr [0x4a3d68]
00407241  fstp       dword ptr [esi + 0x434]
00407247  fld        dword ptr [edi + 8]
0040724a  fstp       dword ptr [esi + 0x438]

// block 00407250
00407250  push       1
00407252  push       esi
00407253  mov        ecx, ebx
00407255  call       0x454d10

// block 0040725a
0040725a  mov        ebx, esi
0040725c  lea        eax, [esp + 0x24]
00407260  call       0x461250

// block 00407265
00407265  test       dword ptr [0x4cee78], 0x8000
0040726f  mov        esi, dword ptr [eax]
00407271  je         0x407284

// block 00407273
00407273  push       0x4cf1d0
00407278  call       dword ptr [0x49808c]

// block 0040727e
0040727e  dec        byte ptr [0x4cf221]

// block 00407284
00407284  mov        ecx, esi
00407286  mov        dword ptr [ebp + 0x48], esi
00407289  mov        esi, dword ptr [0x4ce8cc]
0040728f  push       ecx
00407290  mov        edx, esi
00407292  call       0x461920

// block 00407297
00407297  xor        edi, edi
00407299  cmp        eax, edi
0040729b  jne        0x4072a0

// block 0040729d
0040729d  mov        dword ptr [ebp + 0x48], edi

// block 004072a0
004072a0  mov        ebx, 0xe0
004072a5  mov        byte ptr [eax + 0x3bf], bl
004072ab  mov        edx, dword ptr [ebp + 0x48]
004072ae  push       edx
004072af  mov        edx, esi
004072b1  call       0x461920

// block 004072b6
004072b6  cmp        eax, edi
004072b8  jne        0x4072bd

// block 004072ba
004072ba  mov        dword ptr [ebp + 0x48], edi

// block 004072bd
004072bd  pop        edi
004072be  pop        esi
004072bf  mov        dword ptr [eax + 0x27c], ebx
004072c5  pop        ebp
004072c6  xor        eax, eax
004072c8  pop        ebx
004072c9  add        esp, 0x10
004072cc  ret        4
