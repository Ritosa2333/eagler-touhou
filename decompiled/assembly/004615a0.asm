
// block 004615a0
004615a0  sub        esp, 0xc
004615a3  test       dword ptr [0x4cee78], 0x8000
004615ad  push       ebx
004615ae  push       ebp
004615af  mov        ebp, dword ptr [esp + 0x18]
004615b3  push       esi
004615b4  push       edi
004615b5  mov        ebx, eax
004615b7  mov        edi, ecx
004615b9  je         0x4615cc

// block 004615bb
004615bb  push       0x4cf1d0
004615c0  call       dword ptr [0x498088]

// block 004615c6
004615c6  inc        byte ptr [0x4cf221]

// block 004615cc
004615cc  inc        dword ptr [ebp + 0x130]
004615d2  call       0x4621c0

// block 004615d7
004615d7  mov        esi, eax
004615d9  test       ebx, ebx
004615db  jl         0x4615e0

// block 004615dd
004615dd  mov        dword ptr [esi + 0x20], ebx

// block 004615e0
004615e0  or         dword ptr [esi + 0x480], 1
004615e7  mov        ebx, dword ptr [esp + 0x2c]
004615eb  test       edi, edi
004615ed  jne        0x46161c

// block 004615ef
004615ef  test       bl, 8
004615f2  jne        0x46161c

// block 004615f4
004615f4  fldz       
004615f6  fst        dword ptr [esp + 0x10]
004615fa  mov        eax, dword ptr [esp + 0x10]
004615fe  fst        dword ptr [esp + 0x14]
00461602  mov        ecx, dword ptr [esp + 0x14]
00461606  fstp       dword ptr [esp + 0x18]
0046160a  mov        edx, dword ptr [esp + 0x18]
0046160e  mov        dword ptr [esi + 0x430], eax
00461614  mov        dword ptr [esi + 0x434], ecx
0046161a  jmp        0x461663

// block 0046161c
0046161c  test       bl, 1
0046161f  je         0x46164f

// block 00461621
00461621  fld        dword ptr [edi]
00461623  fadd       qword ptr [0x4a3d70]
00461629  fadd       qword ptr [0x4a3d28]
0046162f  fstp       dword ptr [esi + 0x430]
00461635  fld        dword ptr [edi + 4]
00461638  fadd       qword ptr [0x4a3d68]
0046163e  fstp       dword ptr [esi + 0x434]
00461644  fld        dword ptr [edi + 8]
00461647  fstp       dword ptr [esi + 0x438]
0046164d  jmp        0x461669

// block 0046164f
0046164f  mov        eax, dword ptr [edi]
00461651  mov        dword ptr [esi + 0x430], eax
00461657  mov        ecx, dword ptr [edi + 4]
0046165a  mov        dword ptr [esi + 0x434], ecx
00461660  mov        edx, dword ptr [edi + 8]

// block 00461663
00461663  mov        dword ptr [esi + 0x438], edx

// block 00461669
00461669  test       bl, 8
0046166c  jne        0x46167d

// block 0046166e
0046166e  mov        eax, dword ptr [esp + 0x28]
00461672  push       eax
00461673  push       esi
00461674  mov        ecx, ebp
00461676  call       0x454d10

// block 0046167b
0046167b  jmp        0x461691

// block 0046167d
0046167d  mov        ecx, dword ptr [esp + 0x28]
00461681  mov        ebx, edi
00461683  push       ecx
00461684  mov        eax, esi
00461686  mov        edi, ebp
00461688  call       0x454df0

// block 0046168d
0046168d  mov        ebx, dword ptr [esp + 0x2c]

// block 00461691
00461691  mov        eax, ebx
00461693  and        eax, 4
00461696  je         0x4616aa

// block 00461698
00461698  test       bl, 2
0046169b  je         0x4616aa

// block 0046169d
0046169d  mov        ebx, esi
0046169f  lea        eax, [esp + 0x2c]
004616a3  call       0x4613d0

// block 004616a8
004616a8  jmp        0x4616de

// block 004616aa
004616aa  test       eax, eax
004616ac  lea        eax, [esp + 0x2c]
004616b0  je         0x4616c3

// block 004616b2
004616b2  mov        ebx, esi
004616b4  call       0x461350

// block 004616b9
004616b9  mov        ecx, dword ptr [eax]
004616bb  mov        edx, dword ptr [esp + 0x24]
004616bf  mov        dword ptr [edx], ecx
004616c1  jmp        0x4616e6

// block 004616c3
004616c3  test       bl, 2
004616c6  mov        ebx, esi
004616c8  je         0x4616d9

// block 004616ca
004616ca  call       0x4612d0

// block 004616cf
004616cf  mov        eax, dword ptr [eax]
004616d1  mov        ecx, dword ptr [esp + 0x24]
004616d5  mov        dword ptr [ecx], eax
004616d7  jmp        0x4616e6

// block 004616d9
004616d9  call       0x461250

// block 004616de
004616de  mov        edx, dword ptr [eax]
004616e0  mov        eax, dword ptr [esp + 0x24]
004616e4  mov        dword ptr [eax], edx

// block 004616e6
004616e6  test       dword ptr [0x4cee78], 0x8000
004616f0  je         0x461703

// block 004616f2
004616f2  push       0x4cf1d0
004616f7  call       dword ptr [0x49808c]

// block 004616fd
004616fd  dec        byte ptr [0x4cf221]

// block 00461703
00461703  mov        eax, dword ptr [esp + 0x24]
00461707  pop        edi
00461708  pop        esi
00461709  pop        ebp
0046170a  pop        ebx
0046170b  add        esp, 0xc
0046170e  ret        0x10
