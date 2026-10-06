
// block 00407780
00407780  fld        dword ptr [0x4a3d78]
00407786  sub        esp, 0x10
00407789  push       ebx
0040778a  push       ebp
0040778b  mov        ebp, dword ptr [esp + 0x1c]
0040778f  fstp       dword ptr [ebp + 0x514]
00407795  push       esi
00407796  mov        esi, dword ptr [0x4b4514]
0040779c  fld        dword ptr [0x4a4114]
004077a2  fstp       dword ptr [ebp + 0x518]
004077a8  mov        eax, dword ptr [esi + 0x97c]
004077ae  push       edi
004077af  lea        edi, [ebp + 0x508]
004077b5  mov        dword ptr [edi], eax
004077b7  mov        ecx, dword ptr [esi + 0x980]
004077bd  mov        dword ptr [edi + 4], ecx
004077c0  mov        edx, dword ptr [esi + 0x984]
004077c6  mov        dword ptr [edi + 8], edx
004077c9  mov        edx, 0x33
004077ce  call       0x453d90

// block 004077d3
004077d3  mov        ecx, dword ptr [esi + 0x10]
004077d6  push       0
004077d8  push       0x18
004077da  lea        eax, [esp + 0x2c]
004077de  push       eax
004077df  push       ecx
004077e0  mov        eax, 0x17
004077e5  xor        ecx, ecx
004077e7  call       0x4615a0

// block 004077ec
004077ec  mov        edx, dword ptr [esp + 0x24]
004077f0  mov        ecx, dword ptr [0x4b43cc]
004077f6  mov        dword ptr [ebp + 0x40], edx
004077f9  mov        eax, dword ptr [ecx + 0x7c]
004077fc  test       al, 1
004077fe  je         0x407827

// block 00407800
00407800  cmp        dword ptr [ecx + 0x28], 0x3c
00407804  jl         0x407815

// block 00407806
00407806  mov        dword ptr [ecx + 0x80], 0
00407810  and        eax, 0xffffffdd
00407813  jmp        0x407824

// block 00407815
00407815  mov        edx, dword ptr [0x4b43c4]
0040781b  cmp        dword ptr [edx + 0x3c], 0
0040781f  je         0x407827

// block 00407821
00407821  or         eax, 0x20

// block 00407824
00407824  mov        dword ptr [ecx + 0x7c], eax

// block 00407827
00407827  mov        eax, dword ptr [0x4b43dc]
0040782c  inc        dword ptr [eax + 0x14]
0040782f  push       0x44
00407831  call       0x46c9ea

// block 00407836
00407836  mov        esi, eax
00407838  add        esp, 4
0040783b  test       esi, esi
0040783d  je         0x407865

// block 0040783f
0040783f  and        dword ptr [esi + 0x40], 0xfffffffe
00407843  push       0x44
00407845  push       0
00407847  push       esi
00407848  call       0x477420

// block 0040784d
0040784d  or         dword ptr [esi], 2
00407850  add        esp, 0xc
00407853  push       0x46
00407855  push       0x1e
00407857  push       0x78
00407859  push       0x3c
0040785b  push       2
0040785d  push       8
0040785f  push       esi
00407860  call       0x452a60

// block 00407865
00407865  test       dword ptr [0x4cee78], 0x8000
0040786f  mov        eax, dword ptr [0x4b43e4]
00407874  mov        ebx, dword ptr [eax + 0x6d44]
0040787a  je         0x40788d

// block 0040787c
0040787c  push       0x4cf1d0
00407881  call       dword ptr [0x498088]

// block 00407887
00407887  inc        byte ptr [0x4cf221]

// block 0040788d
0040788d  inc        dword ptr [ebx + 0x130]
00407893  call       0x4621c0

// block 00407898
00407898  mov        esi, eax
0040789a  or         dword ptr [esi + 0x480], 1
004078a1  mov        dword ptr [esi + 0x20], 0x17
004078a8  test       edi, edi
004078aa  jne        0x4078da

// block 004078ac
004078ac  fldz       
004078ae  fst        dword ptr [esp + 0x14]
004078b2  mov        ecx, dword ptr [esp + 0x14]
004078b6  fst        dword ptr [esp + 0x18]
004078ba  mov        edx, dword ptr [esp + 0x18]
004078be  fstp       dword ptr [esp + 0x1c]
004078c2  mov        eax, dword ptr [esp + 0x1c]
004078c6  mov        dword ptr [esi + 0x430], ecx
004078cc  mov        dword ptr [esi + 0x434], edx
004078d2  mov        dword ptr [esi + 0x438], eax
004078d8  jmp        0x407906

// block 004078da
004078da  fld        dword ptr [edi]
004078dc  fadd       qword ptr [0x4a3d70]
004078e2  fadd       qword ptr [0x4a3d28]
004078e8  fstp       dword ptr [esi + 0x430]
004078ee  fld        dword ptr [edi + 4]
004078f1  fadd       qword ptr [0x4a3d68]
004078f7  fstp       dword ptr [esi + 0x434]
004078fd  fld        dword ptr [edi + 8]
00407900  fstp       dword ptr [esi + 0x438]

// block 00407906
00407906  push       1
00407908  push       esi
00407909  mov        ecx, ebx
0040790b  call       0x454d10

// block 00407910
00407910  mov        ebx, esi
00407912  lea        eax, [esp + 0x24]
00407916  call       0x461250

// block 0040791b
0040791b  test       dword ptr [0x4cee78], 0x8000
00407925  mov        esi, dword ptr [eax]
00407927  je         0x40793a

// block 00407929
00407929  push       0x4cf1d0
0040792e  call       dword ptr [0x49808c]

// block 00407934
00407934  dec        byte ptr [0x4cf221]

// block 0040793a
0040793a  pop        edi
0040793b  mov        dword ptr [ebp + 0x48], esi
0040793e  pop        esi
0040793f  pop        ebp
00407940  xor        eax, eax
00407942  pop        ebx
00407943  add        esp, 0x10
00407946  ret        4
