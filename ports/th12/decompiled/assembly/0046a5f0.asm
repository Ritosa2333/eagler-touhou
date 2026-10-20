
// block 0046a5f0
0046a5f0  push       -1
0046a5f2  push       0x49757b
0046a5f7  mov        eax, dword ptr fs:[0]
0046a5fd  push       eax
0046a5fe  sub        esp, 0x14
0046a601  push       ebx
0046a602  push       ebp
0046a603  push       esi
0046a604  push       edi
0046a605  mov        eax, dword ptr [0x4ad138]
0046a60a  xor        eax, esp
0046a60c  push       eax
0046a60d  lea        eax, [esp + 0x28]
0046a611  mov        dword ptr fs:[0], eax
0046a617  mov        edi, ecx
0046a619  mov        eax, dword ptr [0x4b4514]
0046a61e  fld        dword ptr [0x4a3d78]
0046a624  add        eax, 0x97c
0046a629  fstp       dword ptr [edi + 0x514]
0046a62f  fld        dword ptr [0x4a4114]
0046a635  push       eax
0046a636  push       4
0046a638  fstp       dword ptr [edi + 0x518]
0046a63e  lea        eax, [esp + 0x1c]
0046a642  push       eax
0046a643  call       0x40fbe0

// block 0046a648
0046a648  mov        esi, dword ptr [edi + 0x504]
0046a64e  mov        ecx, dword ptr [eax]
0046a650  xor        ebx, ebx
0046a652  mov        dword ptr [edi + 0x40], ecx
0046a655  cmp        esi, ebx
0046a657  je         0x46a667

// block 0046a659
0046a659  call       0x402870

// block 0046a65e
0046a65e  push       esi
0046a65f  call       0x46ca4f

// block 0046a664
0046a664  add        esp, 4

// block 0046a667
0046a667  push       0x18
0046a669  mov        dword ptr [edi + 0x504], ebx
0046a66f  call       0x46c9ea

// block 0046a674
0046a674  mov        esi, eax
0046a676  add        esp, 4
0046a679  mov        dword ptr [esp + 0x14], esi
0046a67d  mov        dword ptr [esp + 0x30], ebx
0046a681  cmp        esi, ebx
0046a683  je         0x46a693

// block 0046a685
0046a685  push       0x11
0046a687  mov        eax, 4
0046a68c  call       0x40ff10

// block 0046a691
0046a691  jmp        0x46a695

// block 0046a693
0046a693  xor        eax, eax

// block 0046a695
0046a695  mov        dword ptr [esp + 0x30], 0xffffffff
0046a69d  fld        dword ptr [0x4a434c]
0046a6a3  mov        ebp, dword ptr [0x4b4514]
0046a6a9  sub        esp, 0x10
0046a6ac  fstp       dword ptr [esp + 0xc]
0046a6b0  mov        dword ptr [edi + 0x504], eax
0046a6b6  fld        dword ptr [0x4a4348]
0046a6bc  fstp       dword ptr [esp + 8]
0046a6c0  fldz       
0046a6c2  fstp       dword ptr [esp + 4]
0046a6c6  fld        dword ptr [ebp + 0x97c]
0046a6cc  fsub       qword ptr [0x4a4340]
0046a6d2  fstp       dword ptr [esp + 0x24]
0046a6d6  fld        dword ptr [esp + 0x24]
0046a6da  fstp       dword ptr [esp]
0046a6dd  call       0x410020

// block 0046a6e2
0046a6e2  mov        ecx, dword ptr [edi + 0x504]
0046a6e8  mov        esi, dword ptr [ecx + 0x10]
0046a6eb  xor        eax, eax
0046a6ed  cmp        dword ptr [ecx + 4], ebx
0046a6f0  jle        0x46a753

// block 0046a6f2
0046a6f2  mov        edx, dword ptr [edi + 0x504]
0046a6f8  mov        ecx, dword ptr [edx + 4]
0046a6fb  xor        ebx, ebx
0046a6fd  lea        edx, [ecx - 1]
0046a700  cmp        eax, edx
0046a702  setge      bl
0046a705  lea        edx, [ecx*8]
0046a70c  sub        edx, ecx
0046a70e  dec        ebx
0046a70f  and        ebx, 0xff000081
0046a715  dec        ebx
0046a716  mov        dword ptr [esi + edx*4 + 0x10], ebx
0046a71a  mov        ecx, dword ptr [edi + 0x504]
0046a720  mov        ecx, dword ptr [ecx + 4]
0046a723  xor        ebx, ebx
0046a725  lea        edx, [ecx - 1]
0046a728  cmp        eax, edx
0046a72a  setge      bl
0046a72d  lea        edx, [ecx*8]
0046a734  sub        edx, ecx
0046a736  inc        eax
0046a737  add        esi, 0x1c
0046a73a  dec        ebx
0046a73b  and        ebx, 0xff000081
0046a741  dec        ebx
0046a742  mov        dword ptr [esi + edx*8 - 0xc], ebx
0046a746  mov        ecx, dword ptr [edi + 0x504]
0046a74c  cmp        eax, dword ptr [ecx + 4]
0046a74f  jl         0x46a6f2

// block 0046a751
0046a751  xor        ebx, ebx

// block 0046a753
0046a753  mov        eax, dword ptr [0x4b43cc]
0046a758  mov        ecx, dword ptr [eax + 0x7c]
0046a75b  test       cl, 1
0046a75e  je         0x46a782

// block 0046a760
0046a760  cmp        dword ptr [eax + 0x28], 0x3c
0046a764  jl         0x46a771

// block 0046a766
0046a766  mov        dword ptr [eax + 0x80], ebx
0046a76c  and        ecx, 0xffffffdd
0046a76f  jmp        0x46a77f

// block 0046a771
0046a771  mov        edx, dword ptr [0x4b43c4]
0046a777  cmp        dword ptr [edx + 0x3c], ebx
0046a77a  je         0x46a782

// block 0046a77c
0046a77c  or         ecx, 0x20

// block 0046a77f
0046a77f  mov        dword ptr [eax + 0x7c], ecx

// block 0046a782
0046a782  mov        ecx, dword ptr [ebp + 0xc410]
0046a788  test       cl, 1
0046a78b  jne        0x46a7b8

// block 0046a78d
0046a78d  fldz       
0046a78f  or         ecx, 1
0046a792  fstp       dword ptr [ebp + 0xc408]
0046a798  mov        dword ptr [ebp + 0xc404], ebx
0046a79e  mov        dword ptr [ebp + 0xc400], 0xfff0bdc1
0046a7a8  mov        dword ptr [ebp + 0xc40c], 0x4b2ed0
0046a7b2  mov        dword ptr [ebp + 0xc410], ecx

// block 0046a7b8
0046a7b8  fld        dword ptr [0x4a41fc]
0046a7be  mov        edx, dword ptr [0x4b43dc]
0046a7c4  fstp       dword ptr [ebp + 0xc408]
0046a7ca  mov        dword ptr [ebp + 0xc404], 0xc8
0046a7d4  mov        dword ptr [ebp + 0xc400], 0xc7
0046a7de  inc        dword ptr [edx + 0x14]
0046a7e1  mov        ecx, dword ptr [ebp + 0x97c]
0046a7e7  lea        esi, [edi + 0x508]
0046a7ed  mov        dword ptr [esi], ecx
0046a7ef  mov        ecx, dword ptr [ebp + 0x980]
0046a7f5  mov        dword ptr [esi + 4], ecx
0046a7f8  mov        ecx, dword ptr [ebp + 0x984]
0046a7fe  mov        dword ptr [esi + 8], ecx
0046a801  mov        ecx, dword ptr [eax + 0x7c]
0046a804  test       cl, 1
0046a807  je         0x46a82c

// block 0046a809
0046a809  cmp        dword ptr [eax + 0x28], 0x3c
0046a80d  jl         0x46a81a

// block 0046a80f
0046a80f  mov        dword ptr [eax + 0x80], ebx
0046a815  and        ecx, 0xffffffdd
0046a818  jmp        0x46a829

// block 0046a81a
0046a81a  mov        ebx, dword ptr [0x4b43c4]
0046a820  cmp        dword ptr [ebx + 0x3c], 0
0046a824  je         0x46a82c

// block 0046a826
0046a826  or         ecx, 0x20

// block 0046a829
0046a829  mov        dword ptr [eax + 0x7c], ecx

// block 0046a82c
0046a82c  inc        dword ptr [edx + 0x14]
0046a82f  fld        dword ptr [ebp + 0x97c]
0046a835  push       ecx
0046a836  mov        eax, 0x33
0046a83b  fstp       dword ptr [esp]
0046a83e  call       0x453e20

// block 0046a843
0046a843  test       dword ptr [0x4cee78], 0x8000
0046a84d  mov        edx, dword ptr [0x4b43e4]
0046a853  mov        ebx, dword ptr [edx + 0x6d44]
0046a859  je         0x46a86c

// block 0046a85b
0046a85b  push       0x4cf1d0
0046a860  call       dword ptr [0x498088]

// block 0046a866
0046a866  inc        byte ptr [0x4cf221]

// block 0046a86c
0046a86c  inc        dword ptr [ebx + 0x130]
0046a872  call       0x4621c0

// block 0046a877
0046a877  mov        ebp, eax
0046a879  or         dword ptr [ebp + 0x480], 1
0046a880  mov        dword ptr [ebp + 0x20], 0x17
0046a887  test       esi, esi
0046a889  jne        0x46a8b9

// block 0046a88b
0046a88b  fldz       
0046a88d  fst        dword ptr [esp + 0x1c]
0046a891  mov        eax, dword ptr [esp + 0x1c]
0046a895  fst        dword ptr [esp + 0x20]
0046a899  mov        ecx, dword ptr [esp + 0x20]
0046a89d  fstp       dword ptr [esp + 0x24]
0046a8a1  mov        edx, dword ptr [esp + 0x24]
0046a8a5  mov        dword ptr [ebp + 0x430], eax
0046a8ab  mov        dword ptr [ebp + 0x434], ecx
0046a8b1  mov        dword ptr [ebp + 0x438], edx
0046a8b7  jmp        0x46a8e5

// block 0046a8b9
0046a8b9  fld        dword ptr [esi]
0046a8bb  fadd       qword ptr [0x4a3d70]
0046a8c1  fadd       qword ptr [0x4a3d28]
0046a8c7  fstp       dword ptr [ebp + 0x430]
0046a8cd  fld        dword ptr [esi + 4]
0046a8d0  fadd       qword ptr [0x4a3d68]
0046a8d6  fstp       dword ptr [ebp + 0x434]
0046a8dc  fld        dword ptr [esi + 8]
0046a8df  fstp       dword ptr [ebp + 0x438]

// block 0046a8e5
0046a8e5  push       1
0046a8e7  push       ebp
0046a8e8  mov        ecx, ebx
0046a8ea  call       0x454d10

// block 0046a8ef
0046a8ef  mov        ebx, ebp
0046a8f1  lea        eax, [esp + 0x18]
0046a8f5  call       0x461250

// block 0046a8fa
0046a8fa  test       dword ptr [0x4cee78], 0x8000
0046a904  mov        esi, dword ptr [eax]
0046a906  je         0x46a919

// block 0046a908
0046a908  push       0x4cf1d0
0046a90d  call       dword ptr [0x49808c]

// block 0046a913
0046a913  dec        byte ptr [0x4cf221]

// block 0046a919
0046a919  push       0x44
0046a91b  mov        dword ptr [edi + 0x48], esi
0046a91e  call       0x46c9ea

// block 0046a923
0046a923  mov        edi, eax
0046a925  add        esp, 4
0046a928  test       edi, edi
0046a92a  je         0x46a955

// block 0046a92c
0046a92c  and        dword ptr [edi + 0x40], 0xfffffffe
0046a930  push       0x44
0046a932  push       0
0046a934  push       edi
0046a935  call       0x477420

// block 0046a93a
0046a93a  or         dword ptr [edi], 2
0046a93d  add        esp, 0xc
0046a940  push       0x46
0046a942  push       0x1e
0046a944  push       0xf0
0046a949  push       0x3c
0046a94b  push       2
0046a94d  push       8
0046a94f  push       edi
0046a950  call       0x452a60

// block 0046a955
0046a955  xor        eax, eax
0046a957  mov        ecx, dword ptr [esp + 0x28]
0046a95b  mov        dword ptr fs:[0], ecx
0046a962  pop        ecx
0046a963  pop        edi
0046a964  pop        esi
0046a965  pop        ebp
0046a966  pop        ebx
0046a967  add        esp, 0x20
0046a96a  ret        
