
// block 0042ab80
0042ab80  push       ebp
0042ab81  mov        ebp, esp
0042ab83  and        esp, 0xfffffff8
0042ab86  sub        esp, 0x14
0042ab89  push       ebx
0042ab8a  mov        ebx, ecx
0042ab8c  push       esi
0042ab8d  mov        esi, dword ptr [ebp + 8]
0042ab90  push       edi
0042ab91  lea        edi, [ebx + 0x454]
0042ab97  mov        ecx, 0x82

// block 0042ab9c
0042ab9c  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0042ab9e
0042ab9e  mov        ax, word ptr [ebx + 0x4a4]
0042aba5  mov        cx, word ptr [ebx + 0x4a6]
0042abac  lea        esi, [ebx + 0x660]
0042abb2  mov        dword ptr [ebx + 0xc], 3
0042abb9  mov        dword ptr [ebx + 0x10], 1
0042abc0  mov        word ptr [ebx + 0x450], ax
0042abc7  mov        word ptr [ebx + 0x452], cx
0042abce  call       0x402520

// block 0042abd3
0042abd3  movsx      edx, word ptr [ebx + 0x450]
0042abda  mov        ecx, dword ptr [0x4b44f4]
0042abe0  imul       edx, edx, 0xd0
0042abe6  mov        dword ptr [ebx + 0xb0c], 0x428ac0
0042abf0  mov        dword ptr [ebx + 0xb10], ebx
0042abf6  mov        edx, dword ptr [edx + 0x4af280]
0042abfc  mov        ecx, dword ptr [ecx + 0x488]
0042ac02  mov        eax, esi
0042ac04  call       0x454ee0

// block 0042ac09
0042ac09  mov        eax, dword ptr [esi + 0x494]
0042ac0f  test       eax, eax
0042ac11  je         0x42ac1c

// block 0042ac13
0042ac13  mov        edx, 2
0042ac18  mov        ecx, esi
0042ac1a  call       eax

// block 0042ac1c
0042ac1c  mov        edx, 2
0042ac21  push       esi
0042ac22  mov        word ptr [esi + 0x3c4], dx
0042ac29  call       0x455630

// block 0042ac2e
0042ac2e  mov        eax, dword ptr [esi + 0x47c]
0042ac34  mov        edx, dword ptr [0x4b44f4]
0042ac3a  and        eax, 0xffffff3f
0042ac3f  or         eax, 0x20
0042ac42  mov        dword ptr [esi + 0x47c], eax
0042ac48  mov        ecx, dword ptr [ebx + 0xadc]
0042ac4e  movsx      edi, word ptr [ebx + 0x4a6]
0042ac55  and        ecx, 0xf0c7ffff
0042ac5b  or         ecx, 0xc00000
0042ac61  mov        dword ptr [ebx + 0xadc], ecx
0042ac67  mov        eax, dword ptr [edx + 0x488]
0042ac6d  lea        esi, [ebx + 0xb14]
0042ac73  add        edi, 0x35
0042ac76  mov        dword ptr [esp + 0x10], eax
0042ac7a  call       0x402520

// block 0042ac7f
0042ac7f  mov        ecx, dword ptr [esp + 0x10]
0042ac83  mov        al, 0x10
0042ac85  push       edi
0042ac86  push       esi
0042ac87  mov        byte ptr [esi + 0x49d], al
0042ac8d  mov        byte ptr [esi + 0x49c], al
0042ac93  call       0x454d10

// block 0042ac98
0042ac98  mov        eax, dword ptr [esi + 0x494]
0042ac9e  test       eax, eax
0042aca0  je         0x42acab

// block 0042aca2
0042aca2  mov        edx, 2
0042aca7  mov        ecx, esi
0042aca9  call       eax

// block 0042acab
0042acab  mov        ecx, 2
0042acb0  push       esi
0042acb1  mov        word ptr [esi + 0x3c4], cx
0042acb8  call       0x455630

// block 0042acbd
0042acbd  mov        edx, dword ptr [esi + 0x47c]
0042acc3  and        edx, 0xffffff3f
0042acc9  or         edx, 0x20
0042accc  mov        dword ptr [esi + 0x47c], edx
0042acd2  mov        eax, dword ptr [ebx + 0xf90]
0042acd8  and        eax, 0xf0ffffff
0042acdd  or         eax, 0x800000
0042ace2  mov        dword ptr [ebx + 0xf90], eax
0042ace8  mov        eax, dword ptr [ebx + 0x494]
0042acee  test       eax, eax
0042acf0  jl         0x42acfd

// block 0042acf2
0042acf2  fldz       
0042acf4  push       ecx
0042acf5  fstp       dword ptr [esp]
0042acf8  call       0x453e20

// block 0042acfd
0042acfd  fldz       
0042acff  mov        ecx, dword ptr [ebx + 0x454]
0042ad05  fcomp      dword ptr [ebx + 0x4a0]
0042ad0b  mov        edx, dword ptr [ebx + 0x458]
0042ad11  mov        eax, dword ptr [ebx + 0x45c]
0042ad17  mov        dword ptr [ebx + 0x50], ecx
0042ad1a  mov        dword ptr [ebx + 0x54], edx
0042ad1d  mov        dword ptr [ebx + 0x58], eax
0042ad20  fnstsw     ax
0042ad22  test       ah, 0x44
0042ad25  jnp        0x42ad5a

// block 0042ad27
0042ad27  fld        dword ptr [ebx + 0x4a0]
0042ad2d  sub        esp, 8
0042ad30  fstp       dword ptr [esp + 4]
0042ad34  lea        ecx, [esp + 0x1c]
0042ad38  fld        dword ptr [ebx + 0x46c]
0042ad3e  fstp       dword ptr [esp]
0042ad41  call       0x42e880

// block 0042ad46
0042ad46  fld        dword ptr [esp + 0x14]
0042ad4a  fadd       dword ptr [ebx + 0x50]
0042ad4d  fstp       dword ptr [ebx + 0x50]
0042ad50  fld        dword ptr [ebx + 0x54]
0042ad53  fadd       dword ptr [esp + 0x18]
0042ad57  fstp       dword ptr [ebx + 0x54]

// block 0042ad5a
0042ad5a  fld        dword ptr [ebx + 0x478]
0042ad60  mov        ecx, dword ptr [ebx + 0x49c]
0042ad66  fstp       dword ptr [ebx + 0x6c]
0042ad69  pop        edi
0042ad6a  fld        dword ptr [0x4a4014]
0042ad70  pop        esi
0042ad71  fstp       dword ptr [ebx + 0x70]
0042ad74  mov        dword ptr [ebx + 0x80], ecx
0042ad7a  fld        dword ptr [ebx + 0x480]
0042ad80  mov        dword ptr [ebx + 0x65c], 0
0042ad8a  fstp       dword ptr [ebx + 0x74]
0042ad8d  xor        eax, eax
0042ad8f  fld        dword ptr [ebx + 0x46c]
0042ad95  fstp       dword ptr [ebx + 0x68]
0042ad98  pop        ebx
0042ad99  mov        esp, ebp
0042ad9b  pop        ebp
0042ad9c  ret        4
