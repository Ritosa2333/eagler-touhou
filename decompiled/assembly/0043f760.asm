
// block 0043f760
0043f760  push       ebp
0043f761  mov        ebp, dword ptr [esp + 8]
0043f765  push       esi
0043f766  mov        esi, 1
0043f76b  cmp        dword ptr [ebp + 0x1c], esi
0043f76e  jne        0x43f882

// block 0043f774
0043f774  movzx      eax, word ptr [0x4d48b8]
0043f77b  add        dword ptr [0x4b0ce4], esi
0043f781  test       eax, eax
0043f783  jne        0x43f878

// block 0043f789
0043f789  cmp        dword ptr [0x4b0ce4], 0x708
0043f793  jl         0x43f882

// block 0043f799
0043f799  mov        ecx, dword ptr [0x4b0ce0]
0043f79f  mov        edx, dword ptr [0x4b0ce8]
0043f7a5  mov        eax, dword ptr [edx*4 + 0x4b33a4]
0043f7ac  and        ecx, 0xffffffbf
0043f7af  or         ecx, 0x20
0043f7b2  mov        edx, 0x4b43e8
0043f7b7  mov        dword ptr [0x4b0ce0], ecx
0043f7bd  sub        edx, eax
0043f7bf  nop        

// block 0043f7c0
0043f7c0  mov        cl, byte ptr [eax]
0043f7c2  mov        byte ptr [edx + eax], cl
0043f7c5  add        eax, esi
0043f7c7  test       cl, cl
0043f7c9  jne        0x43f7c0

// block 0043f7cb
0043f7cb  push       0x4b43e8
0043f7d0  call       0x43b6f0

// block 0043f7d5
0043f7d5  mov        ecx, dword ptr [0x4b0ce8]
0043f7db  mov        esi, eax
0043f7dd  inc        ecx
0043f7de  mov        eax, 0x55555555
0043f7e3  imul       ecx
0043f7e5  sub        edx, ecx
0043f7e7  sar        edx, 1
0043f7e9  mov        eax, edx
0043f7eb  shr        eax, 0x1f
0043f7ee  add        eax, edx
0043f7f0  add        ecx, eax
0043f7f2  lea        edx, [ecx + eax*2]
0043f7f5  mov        dword ptr [0x4b0ce8], edx
0043f7fb  xor        eax, eax
0043f7fd  lea        ecx, [esi + 0xb8]

// block 0043f803
0043f803  cmp        dword ptr [ecx], 0
0043f806  jne        0x43f811

// block 0043f808
0043f808  inc        eax
0043f809  add        ecx, 0x24
0043f80c  cmp        eax, 8
0043f80f  jl         0x43f803

// block 0043f811
0043f811  mov        ecx, eax
0043f813  mov        dword ptr [0x4b0cb0], eax
0043f818  mov        dword ptr [0x4b0cb4], eax
0043f81d  mov        dword ptr [0x4cee40], 0xd
0043f827  mov        eax, dword ptr [esi + 0x1c]
0043f82a  mov        edx, dword ptr [eax + 0x5c]
0043f82d  shl        ecx, 6
0043f830  add        ecx, 0x4aebf0
0043f836  mov        dword ptr [0x4b0c90], edx
0043f83c  mov        edx, dword ptr [0x4b0ca8]
0043f842  mov        dword ptr [0x4b452c], ecx
0043f848  mov        ecx, dword ptr [eax + 0x60]
0043f84b  mov        dword ptr [0x4b0c94], ecx
0043f851  mov        dword ptr [0x4b0cac], edx
0043f857  mov        eax, dword ptr [eax + 0x64]
0043f85a  push       esi
0043f85b  mov        dword ptr [0x4b0ca8], eax
0043f860  call       0x43b450

// block 0043f865
0043f865  push       esi
0043f866  call       0x46ca4f

// block 0043f86b
0043f86b  add        esp, 4
0043f86e  mov        dword ptr [0x4ce8b0], 1

// block 0043f878
0043f878  mov        dword ptr [0x4b0ce4], 0

// block 0043f882
0043f882  test       byte ptr [ebp + 0x5c18], 1
0043f889  je         0x43f8f5

// block 0043f88b
0043f88b  inc        dword ptr [ebp + 0x5c14]
0043f891  cmp        dword ptr [ebp + 0x5c14], 5
0043f898  jl         0x43f8f5

// block 0043f89a
0043f89a  push       edi
0043f89b  push       0x4a1f48
0043f8a0  push       0
0043f8a2  call       0x4300d0

// block 0043f8a7
0043f8a7  test       byte ptr [0x4ceae8], 0x10
0043f8ae  je         0x43f8c3

// block 0043f8b0
0043f8b0  push       0
0043f8b2  push       4
0043f8b4  mov        edi, 0x4a0a20
0043f8b9  mov        eax, 0x4cf4e8
0043f8be  call       0x454960

// block 0043f8c3
0043f8c3  push       0
0043f8c5  push       2
0043f8c7  mov        edi, 0x4a0a20
0043f8cc  mov        eax, 0x4cf4e8
0043f8d1  call       0x454960

// block 0043f8d6
0043f8d6  mov        ecx, dword ptr [0x4b451c]
0043f8dc  mov        byte ptr [ecx + 0x1e9da], 1
0043f8e3  and        dword ptr [ebp + 0x5c18], 0xfffffffe
0043f8ea  mov        dword ptr [ebp + 0x5c14], 0
0043f8f4  pop        edi

// block 0043f8f5
0043f8f5  mov        eax, dword ptr [ebp + 0x1c]
0043f8f8  cmp        eax, 0xf
0043f8fb  ja         0x43fb40

// block 0043f901
0043f901  jmp        dword ptr [eax*4 + 0x43fbc0]

// block 0043f908
0043f908  mov        esi, dword ptr [0x4ce8cc]
0043f90e  mov        edx, dword ptr [esi + 0x4b50d4]
0043f914  call       0x461be0

// block 0043f919
0043f919  mov        edx, dword ptr [esi + 0x4b50d8]
0043f91f  call       0x461be0

// block 0043f924
0043f924  mov        edx, dword ptr [esi + 0x4b50c8]
0043f92a  call       0x461be0

// block 0043f92f
0043f92f  mov        edx, dword ptr [esi + 0x4b50c0]
0043f935  call       0x461be0

// block 0043f93a
0043f93a  call       0x411b00

// block 0043f93f
0043f93f  mov        eax, dword ptr [0x4ce8b0]
0043f944  cmp        eax, 3
0043f947  jne        0x43f99b

// block 0043f949
0043f949  lea        eax, [ebp + 0x28]
0043f94c  mov        dword ptr [ebp + 0x30], 0xa
0043f953  mov        ecx, dword ptr [eax + 8]
0043f956  test       ecx, ecx
0043f958  je         0x43f967

// block 0043f95a
0043f95a  jg         0x43f961

// block 0043f95c
0043f95c  dec        ecx
0043f95d  mov        dword ptr [eax], ecx
0043f95f  jmp        0x43f96d

// block 0043f961
0043f961  xor        ecx, ecx
0043f963  mov        dword ptr [eax], ecx
0043f965  jmp        0x43f96d

// block 0043f967
0043f967  mov        dword ptr [eax], 0

// block 0043f96d
0043f96d  call       0x464900

// block 0043f972
0043f972  mov        edx, 0xe
0043f977  mov        eax, ebp
0043f979  call       0x43eee0

// block 0043f97e
0043f97e  mov        eax, dword ptr [ebp + 0x10]
0043f981  or         dword ptr [eax + 4], 2
0043f985  mov        dword ptr [0x4ce8b0], 1

// block 0043f98f
0043f98f  mov        eax, ebp
0043f991  call       0x4480c0

// block 0043f996
0043f996  jmp        0x43fb40

// block 0043f99b
0043f99b  test       byte ptr [0x4b0ce0], 0x20
0043f9a2  mov        esi, 1
0043f9a7  jne        0x43f9bb

// block 0043f9a9
0043f9a9  or         dword ptr [ebp + 0x5c18], esi
0043f9af  mov        dword ptr [ebp + 0x5c14], 0
0043f9b9  jmp        0x43f9ce

// block 0043f9bb
0043f9bb  and        dword ptr [ebp + 0x5c18], 0xfffffffe
0043f9c2  mov        edx, dword ptr [0x4b0cac]
0043f9c8  mov        dword ptr [0x4b0ca8], edx

// block 0043f9ce
0043f9ce  and        dword ptr [0x4b0ce0], 0xffffffdf
0043f9d5  test       eax, eax
0043f9d7  jne        0x43fa01

// block 0043f9d9
0043f9d9  or         dword ptr [ebp + 0x5c18], 2
0043f9e0  mov        edx, esi
0043f9e2  mov        eax, ebp
0043f9e4  call       0x43eee0

// block 0043f9e9
0043f9e9  mov        dword ptr [0x4ce8b0], esi

// block 0043f9ef
0043f9ef  mov        eax, dword ptr [ebp + 0x10]
0043f9f2  or         dword ptr [eax + 4], 2
0043f9f6  push       ebp
0043f9f7  call       0x43fde0

// block 0043f9fc
0043f9fc  jmp        0x43fb40

// block 0043fa01
0043fa01  and        dword ptr [ebp + 0x5c18], 0xfffffffd
0043fa08  cmp        eax, esi
0043fa0a  jne        0x43fa7d

// block 0043fa0c
0043fa0c  cmp        dword ptr [0x4b0ca8], 4
0043fa13  jne        0x43fa62

// block 0043fa15
0043fa15  mov        eax, dword ptr [ebp + 0x30]
0043fa18  test       eax, eax
0043fa1a  je         0x43fa5f

// block 0043fa1c
0043fa1c  cmp        eax, esi
0043fa1e  jg         0x43fa3f

// block 0043fa20
0043fa20  dec        eax
0043fa21  mov        dword ptr [ebp + 0x28], eax
0043fa24  mov        edx, esi
0043fa26  mov        eax, ebp
0043fa28  call       0x43eee0

// block 0043fa2d
0043fa2d  mov        eax, dword ptr [ebp + 0x10]
0043fa30  or         dword ptr [eax + 4], 2
0043fa34  push       ebp
0043fa35  call       0x43fde0

// block 0043fa3a
0043fa3a  jmp        0x43fb40

// block 0043fa3f
0043fa3f  mov        eax, esi
0043fa41  mov        dword ptr [ebp + 0x28], eax
0043fa44  mov        edx, esi
0043fa46  mov        eax, ebp
0043fa48  call       0x43eee0

// block 0043fa4d
0043fa4d  mov        eax, dword ptr [ebp + 0x10]
0043fa50  or         dword ptr [eax + 4], 2
0043fa54  push       ebp
0043fa55  call       0x43fde0

// block 0043fa5a
0043fa5a  jmp        0x43fb40

// block 0043fa5f
0043fa5f  mov        dword ptr [ebp + 0x28], esi

// block 0043fa62
0043fa62  mov        edx, esi
0043fa64  mov        eax, ebp
0043fa66  call       0x43eee0

// block 0043fa6b
0043fa6b  mov        eax, dword ptr [ebp + 0x10]
0043fa6e  or         dword ptr [eax + 4], 2
0043fa72  push       ebp
0043fa73  call       0x43fde0

// block 0043fa78
0043fa78  jmp        0x43fb40

// block 0043fa7d
0043fa7d  cmp        eax, 2
0043fa80  jne        0x43f9ef

// block 0043fa86
0043fa86  lea        eax, [ebp + 0x28]
0043fa89  mov        dword ptr [ebp + 0x30], 0xa
0043fa90  mov        ecx, dword ptr [eax + 8]
0043fa93  test       ecx, ecx
0043fa95  je         0x43faaa

// block 0043fa97
0043fa97  cmp        ecx, 3
0043fa9a  jg         0x43faa1

// block 0043fa9c
0043fa9c  dec        ecx
0043fa9d  mov        dword ptr [eax], ecx
0043fa9f  jmp        0x43fab0

// block 0043faa1
0043faa1  mov        ecx, 3
0043faa6  mov        dword ptr [eax], ecx
0043faa8  jmp        0x43fab0

// block 0043faaa
0043faaa  mov        dword ptr [eax], 3

// block 0043fab0
0043fab0  call       0x464900

// block 0043fab5
0043fab5  mov        edx, 0xb
0043faba  mov        eax, ebp
0043fabc  call       0x43eee0

// block 0043fac1
0043fac1  mov        eax, dword ptr [ebp + 0x10]
0043fac4  or         dword ptr [eax + 4], 2
0043fac8  mov        dword ptr [0x4ce8b0], 1

// block 0043fad2
0043fad2  push       ebp
0043fad3  call       0x4466a0

// block 0043fad8
0043fad8  jmp        0x43fb40

// block 0043fada
0043fada  mov        esi, ebp
0043fadc  call       0x4411d0

// block 0043fae1
0043fae1  jmp        0x43fb40

// block 0043fae3
0043fae3  mov        eax, ebp
0043fae5  call       0x4435b0

// block 0043faea
0043faea  jmp        0x43fb40

// block 0043faec
0043faec  mov        eax, dword ptr [0x4cee78]
0043faf1  shr        eax, 0xd
0043faf4  not        eax
0043faf6  and        eax, 1
0043faf9  or         eax, 2
0043fafc  mov        dword ptr [0x4cee40], eax

// block 0043fb01
0043fb01  call       0x430240

// block 0043fb06
0043fb06  jmp        0x43fb40

// block 0043fb08
0043fb08  push       ebp
0043fb09  call       0x444c80

// block 0043fb0e
0043fb0e  jmp        0x43fb40

// block 0043fb10
0043fb10  push       ebp
0043fb11  call       0x4452d0

// block 0043fb16
0043fb16  jmp        0x43fb40

// block 0043fb18
0043fb18  push       ebp
0043fb19  call       0x445a40

// block 0043fb1e
0043fb1e  jmp        0x43fb40

// block 0043fb20
0043fb20  mov        eax, ebp
0043fb22  call       0x445fc0

// block 0043fb27
0043fb27  jmp        0x43fb40

// block 0043fb29
0043fb29  push       ebp
0043fb2a  call       0x449360

// block 0043fb2f
0043fb2f  jmp        0x43fb40

// block 0043fb31
0043fb31  push       ebp
0043fb32  call       0x4470c0

// block 0043fb37
0043fb37  jmp        0x43fb40

// block 0043fb39
0043fb39  mov        ecx, ebp
0043fb3b  call       0x4489e0

// block 0043fb40
0043fb40  mov        edx, dword ptr [ebp + 0x2b8]
0043fb46  mov        ecx, dword ptr [ebp + 0x2c0]
0043fb4c  mov        dword ptr [ebp + 0x2b4], edx
0043fb52  fld        dword ptr [ecx]
0043fb54  fcomp      qword ptr [0x4a3db0]
0043fb5a  fnstsw     ax
0043fb5c  test       ah, 0x41
0043fb5f  jne        0x43fb93

// block 0043fb61
0043fb61  fld        dword ptr [0x4a3dac]
0043fb67  fcomp      dword ptr [ecx]
0043fb69  fnstsw     ax
0043fb6b  test       ah, 0x41
0043fb6e  jne        0x43fb93

// block 0043fb70
0043fb70  fld        dword ptr [ebp + 0x2bc]
0043fb76  inc        edx
0043fb77  fadd       qword ptr [0x4a3ce0]
0043fb7d  pop        esi
0043fb7e  mov        dword ptr [ebp + 0x2b8], edx
0043fb84  mov        eax, 1
0043fb89  fstp       dword ptr [ebp + 0x2bc]
0043fb8f  pop        ebp
0043fb90  ret        4

// block 0043fb93
0043fb93  fld        dword ptr [ecx]
0043fb95  fadd       dword ptr [ebp + 0x2bc]
0043fb9b  fstp       dword ptr [esp + 0xc]
0043fb9f  fld        dword ptr [esp + 0xc]
0043fba3  fst        dword ptr [ebp + 0x2bc]
0043fba9  call       0x4931e0

// block 0043fbae
0043fbae  mov        dword ptr [ebp + 0x2b8], eax
0043fbb4  pop        esi
0043fbb5  mov        eax, 1
0043fbba  pop        ebp
0043fbbb  ret        4
