
// block 0046f674
0046f674  mov        edi, edi
0046f676  push       ebp
0046f677  mov        ebp, esp
0046f679  mov        eax, dword ptr [0x4d6444]
0046f67e  sub        esp, 0x138
0046f684  push       ebx
0046f685  xor        ebx, ebx
0046f687  cmp        eax, ebx
0046f689  jne        0x46f693

// block 0046f68b
0046f68b  or         eax, 0xffffffff
0046f68e  jmp        0x46f918

// block 0046f693
0046f693  cmp        dword ptr [0x4d6440], ebx
0046f699  push       esi
0046f69a  mov        edx, eax
0046f69c  push       edi
0046f69d  mov        dword ptr [ebp - 0x34], edx
0046f6a0  mov        dword ptr [ebp - 0x1c], ebx
0046f6a3  jle        0x46f914

// block 0046f6a9
0046f6a9  mov        eax, dword ptr [edx + 0x10]
0046f6ac  cmp        eax, ebx
0046f6ae  je         0x46f91b

// block 0046f6b4
0046f6b4  mov        ecx, dword ptr [edx + 0xc]
0046f6b7  mov        dword ptr [ebp - 8], ecx
0046f6ba  lea        ecx, [eax + 0x144]
0046f6c0  mov        dword ptr [ebp - 0x24], ecx
0046f6c3  mov        ecx, dword ptr [edx + 8]
0046f6c6  lea        esi, [eax + 0xc4]
0046f6cc  mov        dword ptr [ebp - 0x20], ecx
0046f6cf  mov        dword ptr [ebp - 0x10], ebx
0046f6d2  mov        dword ptr [ebp - 0x14], ebx
0046f6d5  mov        dword ptr [ebp - 0xc], ebx
0046f6d8  mov        dword ptr [ebp - 0x38], esi

// block 0046f6db
0046f6db  push       0x40
0046f6dd  xor        eax, eax
0046f6df  cmp        dword ptr [ebp - 0x20], ebx
0046f6e2  pop        ecx
0046f6e3  lea        edi, [ebp - 0x138]
0046f6e9  mov        dword ptr [ebp - 0x28], ebx
0046f6ec  mov        dword ptr [ebp - 0x18], ebx
0046f6ef  mov        dword ptr [ebp - 4], ebx

// block 0046f6f2
0046f6f2  rep stosd  dword ptr es:[edi], eax

// block 0046f6f4
0046f6f4  jl         0x46f8b2

// block 0046f6fa
0046f6fa  cmp        dword ptr [ebp - 8], ebx
0046f6fd  je         0x46f91f

// block 0046f703
0046f703  mov        edx, dword ptr [ebp - 8]
0046f706  add        edx, 0xffc

// block 0046f70c
0046f70c  cmp        dword ptr [edx - 0xff4], -1
0046f713  lea        esi, [edx - 0xff0]
0046f719  jne        0x46f92f

// block 0046f71f
0046f71f  cmp        dword ptr [edx], -1
0046f722  jne        0x46f92f

// block 0046f728
0046f728  mov        ecx, dword ptr [esi]
0046f72a  mov        edi, ecx
0046f72c  test       cl, 1
0046f72f  je         0x46f743

// block 0046f731
0046f731  dec        ecx
0046f732  cmp        ecx, 0x400
0046f738  jg         0x46f923

// block 0046f73e
0046f73e  inc        dword ptr [ebp - 4]
0046f741  jmp        0x46f75a

// block 0046f743
0046f743  mov        eax, ecx
0046f745  sar        eax, 4
0046f748  dec        eax
0046f749  cmp        eax, 0x3f
0046f74c  jle        0x46f751

// block 0046f74e
0046f74e  push       0x3f
0046f750  pop        eax

// block 0046f751
0046f751  lea        eax, [ebp + eax*4 - 0x138]
0046f758  inc        dword ptr [eax]

// block 0046f75a
0046f75a  cmp        ecx, 0x10
0046f75d  jl         0x46f92b

// block 0046f763
0046f763  test       cl, 0xf
0046f766  jne        0x46f92b

// block 0046f76c
0046f76c  cmp        ecx, 0xff0
0046f772  jg         0x46f92b

// block 0046f778
0046f778  lea        eax, [ecx + esi]
0046f77b  cmp        dword ptr [eax - 4], edi
0046f77e  jne        0x46f927

// block 0046f784
0046f784  mov        esi, eax
0046f786  cmp        esi, edx
0046f788  jb         0x46f728

// block 0046f78a
0046f78a  jne        0x46f927

// block 0046f790
0046f790  add        edx, 0x1000
0046f796  inc        ebx
0046f797  cmp        ebx, 8
0046f79a  jl         0x46f70c

// block 0046f7a0
0046f7a0  mov        eax, dword ptr [ebp - 4]
0046f7a3  mov        edi, dword ptr [ebp - 0x24]
0046f7a6  cmp        dword ptr [edi], eax
0046f7a8  jne        0x46f933

// block 0046f7ae
0046f7ae  xor        esi, esi

// block 0046f7b0
0046f7b0  and        dword ptr [ebp - 4], 0
0046f7b4  lea        ebx, [edi + 8]
0046f7b7  mov        eax, dword ptr [ebx - 4]
0046f7ba  mov        dword ptr [ebp - 0x2c], edi
0046f7bd  mov        dword ptr [ebp - 0x30], ebx
0046f7c0  cmp        eax, edi
0046f7c2  je         0x46f87a

// block 0046f7c8
0046f7c8  mov        ecx, dword ptr [ebp - 4]
0046f7cb  cmp        ecx, dword ptr [ebp + esi*4 - 0x138]
0046f7d2  je         0x46f853

// block 0046f7d4
0046f7d4  mov        ecx, dword ptr [ebp - 8]
0046f7d7  cmp        eax, ecx
0046f7d9  jb         0x46f943

// block 0046f7df
0046f7df  add        ecx, 0x8000
0046f7e5  cmp        eax, ecx
0046f7e7  jae        0x46f943

// block 0046f7ed
0046f7ed  mov        ecx, eax
0046f7ef  and        ecx, 0xfffff000
0046f7f5  add        ecx, 0xc
0046f7f8  lea        edx, [ecx + 0xff0]
0046f7fe  cmp        ecx, edx
0046f800  je         0x46f937

// block 0046f806
0046f806  cmp        ecx, eax
0046f808  je         0x46f818

// block 0046f80a
0046f80a  mov        ebx, dword ptr [ecx]
0046f80c  and        ebx, 0xfffffffe
0046f80f  add        ecx, ebx
0046f811  mov        ebx, dword ptr [ebp - 0x30]
0046f814  cmp        ecx, edx
0046f816  jne        0x46f806

// block 0046f818
0046f818  cmp        ecx, edx
0046f81a  je         0x46f937

// block 0046f820
0046f820  mov        ecx, dword ptr [eax]
0046f822  sar        ecx, 4
0046f825  dec        ecx
0046f826  cmp        ecx, 0x3f
0046f829  jle        0x46f82e

// block 0046f82b
0046f82b  push       0x3f
0046f82d  pop        ecx

// block 0046f82e
0046f82e  cmp        ecx, esi
0046f830  jne        0x46f93b

// block 0046f836
0046f836  mov        ecx, dword ptr [ebp - 0x2c]
0046f839  cmp        dword ptr [eax + 8], ecx
0046f83c  jne        0x46f93f

// block 0046f842
0046f842  inc        dword ptr [ebp - 4]
0046f845  mov        dword ptr [ebp - 0x2c], eax
0046f848  mov        eax, dword ptr [eax + 4]
0046f84b  cmp        eax, edi
0046f84d  jne        0x46f7c8

// block 0046f853
0046f853  cmp        dword ptr [ebp - 4], 0
0046f857  je         0x46f87a

// block 0046f859
0046f859  cmp        esi, 0x20
0046f85c  mov        eax, 0x80000000
0046f861  jge        0x46f86f

// block 0046f863
0046f863  mov        ecx, esi
0046f865  shr        eax, cl
0046f867  or         dword ptr [ebp - 0x28], eax
0046f86a  or         dword ptr [ebp - 0x10], eax
0046f86d  jmp        0x46f87a

// block 0046f86f
0046f86f  lea        ecx, [esi - 0x20]
0046f872  shr        eax, cl
0046f874  or         dword ptr [ebp - 0x18], eax
0046f877  or         dword ptr [ebp - 0x14], eax

// block 0046f87a
0046f87a  mov        eax, dword ptr [ebp - 0x2c]
0046f87d  cmp        dword ptr [eax + 4], edi
0046f880  jne        0x46f94b

// block 0046f886
0046f886  mov        ecx, dword ptr [ebp - 4]
0046f889  cmp        ecx, dword ptr [ebp + esi*4 - 0x138]
0046f890  jne        0x46f94b

// block 0046f896
0046f896  cmp        dword ptr [ebx], eax
0046f898  jne        0x46f947

// block 0046f89e
0046f89e  inc        esi
0046f89f  cmp        esi, 0x40
0046f8a2  mov        edi, ebx
0046f8a4  jl         0x46f7b0

// block 0046f8aa
0046f8aa  mov        esi, dword ptr [ebp - 0x38]
0046f8ad  mov        edx, dword ptr [ebp - 0x34]
0046f8b0  xor        ebx, ebx

// block 0046f8b2
0046f8b2  mov        eax, dword ptr [ebp - 0x28]
0046f8b5  cmp        eax, dword ptr [esi - 0x80]
0046f8b8  jne        0x46f94f

// block 0046f8be
0046f8be  mov        eax, dword ptr [ebp - 0x18]
0046f8c1  cmp        eax, dword ptr [esi]
0046f8c3  jne        0x46f94f

// block 0046f8c9
0046f8c9  add        dword ptr [ebp - 8], 0x8000
0046f8d0  add        dword ptr [ebp - 0x24], 0x204
0046f8d7  shl        dword ptr [ebp - 0x20], 1
0046f8da  inc        dword ptr [ebp - 0xc]
0046f8dd  add        esi, 4
0046f8e0  cmp        dword ptr [ebp - 0xc], 0x20
0046f8e4  mov        dword ptr [ebp - 0x38], esi
0046f8e7  jl         0x46f6db

// block 0046f8ed
0046f8ed  mov        eax, dword ptr [ebp - 0x10]
0046f8f0  cmp        eax, dword ptr [edx]
0046f8f2  jne        0x46f953

// block 0046f8f4
0046f8f4  mov        eax, dword ptr [ebp - 0x14]
0046f8f7  cmp        eax, dword ptr [edx + 4]
0046f8fa  jne        0x46f953

// block 0046f8fc
0046f8fc  add        edx, 0x14
0046f8ff  inc        dword ptr [ebp - 0x1c]
0046f902  mov        eax, dword ptr [ebp - 0x1c]
0046f905  cmp        eax, dword ptr [0x4d6440]
0046f90b  mov        dword ptr [ebp - 0x34], edx
0046f90e  jl         0x46f6a9

// block 0046f914
0046f914  xor        eax, eax

// block 0046f916
0046f916  pop        edi
0046f917  pop        esi

// block 0046f918
0046f918  pop        ebx
0046f919  leave      
0046f91a  ret        

// block 0046f91b
0046f91b  push       -2
0046f91d  jmp        0x46f955

// block 0046f91f
0046f91f  push       -4
0046f921  jmp        0x46f955

// block 0046f923
0046f923  push       -6
0046f925  jmp        0x46f955

// block 0046f927
0046f927  push       -8
0046f929  jmp        0x46f955

// block 0046f92b
0046f92b  push       -7
0046f92d  jmp        0x46f955

// block 0046f92f
0046f92f  push       -5
0046f931  jmp        0x46f955

// block 0046f933
0046f933  push       -9
0046f935  jmp        0x46f955

// block 0046f937
0046f937  push       -0xb
0046f939  jmp        0x46f955

// block 0046f93b
0046f93b  push       -0xc
0046f93d  jmp        0x46f955

// block 0046f93f
0046f93f  push       -0xd
0046f941  jmp        0x46f955

// block 0046f943
0046f943  push       -0xa
0046f945  jmp        0x46f955

// block 0046f947
0046f947  push       -0xf
0046f949  jmp        0x46f955

// block 0046f94b
0046f94b  push       -0xe
0046f94d  jmp        0x46f955

// block 0046f94f
0046f94f  push       -0x10
0046f951  jmp        0x46f955

// block 0046f953
0046f953  push       -0x11

// block 0046f955
0046f955  pop        eax
0046f956  jmp        0x46f916
