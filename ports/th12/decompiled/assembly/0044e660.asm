
// block 0044e660
0044e660  sub        esp, 0x24
0044e663  push       ebx
0044e664  push       ebp
0044e665  push       esi
0044e666  push       edi
0044e667  mov        ebx, ecx
0044e669  mov        edi, edx
0044e66b  test       eax, eax
0044e66d  jne        0x44e677

// block 0044e66f
0044e66f  mov        ebp, dword ptr [0x4ce554]
0044e675  jmp        0x44e695

// block 0044e677
0044e677  cmp        eax, 1
0044e67a  jne        0x44e684

// block 0044e67c
0044e67c  mov        ebp, dword ptr [0x4ce550]
0044e682  jmp        0x44e695

// block 0044e684
0044e684  mov        ebp, dword ptr [0x4cc54c]
0044e68a  cmp        eax, 2
0044e68d  je         0x44e695

// block 0044e68f
0044e68f  mov        ebp, dword ptr [0x4b453c]

// block 0044e695
0044e695  mov        eax, 0x11
0044e69a  cmp        dword ptr [esp + 0x3c], eax
0044e69e  jge        0x44e6a4

// block 0044e6a0
0044e6a0  mov        dword ptr [esp + 0x3c], eax

// block 0044e6a4
0044e6a4  mov        eax, dword ptr [0x4b0e54]
0044e6a9  mov        ecx, dword ptr [0x4b0e68]
0044e6af  push       eax
0044e6b0  push       0
0044e6b2  push       ecx
0044e6b3  call       0x477420

// block 0044e6b8
0044e6b8  mov        esi, dword ptr [0x4b0e5c]
0044e6be  add        esp, 0xc
0044e6c1  push       ebp
0044e6c2  push       esi
0044e6c3  call       dword ptr [0x49802c]

// block 0044e6c9
0044e6c9  mov        dword ptr [esp + 0x18], eax
0044e6cd  mov        eax, dword ptr [esp + 0x3c]
0044e6d1  lea        eax, [eax + eax + 6]
0044e6d5  push       eax
0044e6d6  call       0x44dc20

// block 0044e6db
0044e6db  push       1
0044e6dd  push       esi
0044e6de  call       dword ptr [0x49801c]

// block 0044e6e4
0044e6e4  mov        ebp, ebx
0044e6e6  lea        eax, [ebp + 1]
0044e6e9  lea        esp, [esp]

// block 0044e6f0
0044e6f0  mov        cl, byte ptr [ebp]
0044e6f3  inc        ebp
0044e6f4  test       cl, cl
0044e6f6  jne        0x44e6f0

// block 0044e6f8
0044e6f8  sub        ebp, eax
0044e6fa  mov        eax, dword ptr [esp + 0x4c]
0044e6fe  test       eax, eax
0044e700  jne        0x44e770

// block 0044e702
0044e702  mov        edx, dword ptr [esp + 0x44]
0044e706  push       edx
0044e707  push       esi
0044e708  call       dword ptr [0x498018]

// block 0044e70e
0044e70e  push       ebp
0044e70f  lea        eax, [edi + edi]
0044e712  mov        edi, dword ptr [0x498014]
0044e718  push       ebx
0044e719  mov        dword ptr [esp + 0x4c], eax
0044e71d  push       4
0044e71f  add        eax, 2
0044e722  push       eax
0044e723  push       esi
0044e724  call       edi

// block 0044e726
0044e726  mov        eax, dword ptr [esp + 0x44]
0044e72a  push       ebp
0044e72b  push       ebx
0044e72c  push       4
0044e72e  add        eax, -2
0044e731  push       eax
0044e732  push       esi
0044e733  call       edi

// block 0044e735
0044e735  mov        eax, dword ptr [esp + 0x44]
0044e739  push       ebp
0044e73a  push       ebx
0044e73b  push       0
0044e73d  add        eax, 2
0044e740  push       eax
0044e741  push       esi
0044e742  call       edi

// block 0044e744
0044e744  mov        eax, dword ptr [esp + 0x44]
0044e748  push       ebp
0044e749  push       ebx
0044e74a  push       0
0044e74c  add        eax, -2
0044e74f  push       eax
0044e750  push       esi
0044e751  call       edi

// block 0044e753
0044e753  mov        eax, dword ptr [esp + 0x40]
0044e757  push       eax
0044e758  push       esi
0044e759  call       dword ptr [0x498018]

// block 0044e75f
0044e75f  mov        ecx, dword ptr [esp + 0x44]
0044e763  push       ebp
0044e764  push       ebx
0044e765  push       2
0044e767  push       ecx
0044e768  push       esi
0044e769  call       edi

// block 0044e76b
0044e76b  jmp        0x44e820

// block 0044e770
0044e770  mov        byte ptr [esp + 0x4c], 0
0044e775  mov        byte ptr [esp + 0x4d], 0
0044e77a  mov        byte ptr [esp + 0x4e], 0
0044e77f  test       ebp, ebp
0044e781  jle        0x44e820

// block 0044e787
0044e787  lea        edx, [eax + eax]
0044e78a  lea        eax, [ebp - 1]
0044e78d  mov        ebp, dword ptr [0x498014]
0044e793  shr        eax, 1
0044e795  inc        ebx
0044e796  inc        eax
0044e797  mov        dword ptr [esp + 0x1c], edx
0044e79b  lea        edi, [edi + edi - 2]
0044e79f  mov        dword ptr [esp + 0x14], eax

// block 0044e7a3
0044e7a3  mov        eax, dword ptr [esp + 0x44]
0044e7a7  push       eax
0044e7a8  push       esi
0044e7a9  call       dword ptr [0x498018]

// block 0044e7af
0044e7af  mov        cl, byte ptr [ebx - 1]
0044e7b2  mov        dl, byte ptr [ebx]
0044e7b4  push       2
0044e7b6  mov        byte ptr [esp + 0x50], cl
0044e7ba  lea        ecx, [esp + 0x50]
0044e7be  push       ecx
0044e7bf  push       4
0044e7c1  lea        eax, [edi + 4]
0044e7c4  push       eax
0044e7c5  push       esi
0044e7c6  mov        byte ptr [esp + 0x61], dl
0044e7ca  call       ebp

// block 0044e7cc
0044e7cc  push       2
0044e7ce  lea        edx, [esp + 0x50]
0044e7d2  push       edx
0044e7d3  push       4
0044e7d5  push       edi
0044e7d6  push       esi
0044e7d7  call       ebp

// block 0044e7d9
0044e7d9  push       2
0044e7db  lea        eax, [esp + 0x50]
0044e7df  push       eax
0044e7e0  push       0
0044e7e2  lea        eax, [edi + 4]
0044e7e5  push       eax
0044e7e6  push       esi
0044e7e7  call       ebp

// block 0044e7e9
0044e7e9  push       2
0044e7eb  lea        ecx, [esp + 0x50]
0044e7ef  push       ecx
0044e7f0  push       0
0044e7f2  push       edi
0044e7f3  push       esi
0044e7f4  call       ebp

// block 0044e7f6
0044e7f6  mov        edx, dword ptr [esp + 0x40]
0044e7fa  push       edx
0044e7fb  push       esi
0044e7fc  call       dword ptr [0x498018]

// block 0044e802
0044e802  push       2
0044e804  lea        eax, [esp + 0x50]
0044e808  push       eax
0044e809  push       2
0044e80b  lea        ecx, [edi + 2]
0044e80e  push       ecx
0044e80f  push       esi
0044e810  call       ebp

// block 0044e812
0044e812  add        edi, dword ptr [esp + 0x1c]
0044e816  add        ebx, 2
0044e819  sub        dword ptr [esp + 0x14], 1
0044e81e  jne        0x44e7a3

// block 0044e820
0044e820  mov        edx, dword ptr [esp + 0x18]
0044e824  push       edx
0044e825  push       esi
0044e826  call       dword ptr [0x49802c]

// block 0044e82c
0044e82c  mov        edi, dword ptr [esp + 0x3c]
0044e830  lea        edi, [edi + edi + 6]
0044e834  push       edi
0044e835  call       0x44dc20

// block 0044e83a
0044e83a  push       edi
0044e83b  call       0x44d8b0

// block 0044e840
0044e840  mov        eax, dword ptr [esp + 0x18]
0044e844  push       eax
0044e845  push       esi
0044e846  call       dword ptr [0x49802c]

// block 0044e84c
0044e84c  mov        esi, dword ptr [esp + 0x38]
0044e850  mov        eax, dword ptr [esi + 8]
0044e853  sub        eax, dword ptr [esi]
0044e855  mov        ecx, dword ptr [esp + 0x3c]
0044e859  xor        edi, edi
0044e85b  lea        eax, [eax + eax + 0x16]
0044e85f  cmp        eax, 0x400
0044e864  lea        edx, [ecx + ecx + 2]
0044e868  mov        dword ptr [esp + 0x20], edi
0044e86c  mov        dword ptr [esp + 0x24], edi
0044e870  mov        dword ptr [esp + 0x28], eax
0044e874  mov        dword ptr [esp + 0x2c], edx
0044e878  jle        0x44e882

// block 0044e87a
0044e87a  mov        dword ptr [esp + 0x28], 0x400

// block 0044e882
0044e882  mov        eax, dword ptr [esp + 0x48]
0044e886  mov        ecx, dword ptr [eax]
0044e888  lea        edx, [esp + 0x3c]
0044e88c  push       edx
0044e88d  push       edi
0044e88e  push       eax
0044e88f  mov        eax, dword ptr [ecx + 0x48]
0044e892  call       eax

// block 0044e894
0044e894  mov        edx, dword ptr [0x4b0e58]
0044e89a  mov        eax, dword ptr [0x4b0e48]
0044e89f  push       edi
0044e8a0  push       4
0044e8a2  lea        ecx, [esp + 0x28]
0044e8a6  push       ecx
0044e8a7  mov        ecx, dword ptr [0x4b0e68]
0044e8ad  push       edi
0044e8ae  push       edx
0044e8af  mov        edx, dword ptr [esp + 0x50]
0044e8b3  push       eax
0044e8b4  push       ecx
0044e8b5  push       esi
0044e8b6  push       edi
0044e8b7  push       edx
0044e8b8  call       0x46c6ec

// block 0044e8bd
0044e8bd  mov        eax, dword ptr [esp + 0x3c]
0044e8c1  cmp        eax, edi
0044e8c3  je         0x44e8cd

// block 0044e8c5
0044e8c5  mov        ecx, dword ptr [eax]
0044e8c7  mov        edx, dword ptr [ecx + 8]
0044e8ca  push       eax
0044e8cb  call       edx

// block 0044e8cd
0044e8cd  pop        edi
0044e8ce  pop        esi
0044e8cf  pop        ebp
0044e8d0  pop        ebx
0044e8d1  add        esp, 0x24
0044e8d4  ret        0x18
