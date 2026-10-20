
// block 0048b606
0048b606  push       0x10
0048b608  push       0x4ab018
0048b60d  call       0x46fcec

// block 0048b612
0048b612  mov        ebx, dword ptr [ebp + 8]
0048b615  test       ebx, ebx
0048b617  jne        0x48b627

// block 0048b619
0048b619  push       dword ptr [ebp + 0xc]
0048b61c  call       0x46d04a

// block 0048b621
0048b621  pop        ecx
0048b622  jmp        0x48b7f3

// block 0048b627
0048b627  mov        esi, dword ptr [ebp + 0xc]
0048b62a  test       esi, esi
0048b62c  jne        0x48b63a

// block 0048b62e
0048b62e  push       ebx
0048b62f  call       0x46c941

// block 0048b634
0048b634  pop        ecx
0048b635  jmp        0x48b7f1

// block 0048b63a
0048b63a  cmp        dword ptr [0x4d6458], 3
0048b641  jne        0x48b7da

// block 0048b647
0048b647  xor        edi, edi
0048b649  mov        dword ptr [ebp - 0x1c], edi
0048b64c  cmp        esi, -0x20
0048b64f  ja         0x48b7df

// block 0048b655
0048b655  push       4
0048b657  call       0x46ec9a

// block 0048b65c
0048b65c  pop        ecx
0048b65d  mov        dword ptr [ebp - 4], edi
0048b660  push       ebx
0048b661  call       0x46edc8

// block 0048b666
0048b666  pop        ecx
0048b667  mov        dword ptr [ebp - 0x20], eax
0048b66a  cmp        eax, edi
0048b66c  je         0x48b710

// block 0048b672
0048b672  cmp        esi, dword ptr [0x4d6448]
0048b678  ja         0x48b6c3

// block 0048b67a
0048b67a  push       esi
0048b67b  push       ebx
0048b67c  push       eax
0048b67d  call       0x46f2c6

// block 0048b682
0048b682  add        esp, 0xc
0048b685  test       eax, eax
0048b687  je         0x48b68e

// block 0048b689
0048b689  mov        dword ptr [ebp - 0x1c], ebx
0048b68c  jmp        0x48b6c3

// block 0048b68e
0048b68e  push       esi
0048b68f  call       0x46fa07

// block 0048b694
0048b694  pop        ecx
0048b695  mov        dword ptr [ebp - 0x1c], eax
0048b698  cmp        eax, edi
0048b69a  je         0x48b6c3

// block 0048b69c
0048b69c  mov        eax, dword ptr [ebx - 4]
0048b69f  dec        eax
0048b6a0  cmp        eax, esi
0048b6a2  jb         0x48b6a6

// block 0048b6a4
0048b6a4  mov        eax, esi

// block 0048b6a6
0048b6a6  push       eax
0048b6a7  push       ebx
0048b6a8  push       dword ptr [ebp - 0x1c]
0048b6ab  call       0x47a330

// block 0048b6b0
0048b6b0  push       ebx
0048b6b1  call       0x46edc8

// block 0048b6b6
0048b6b6  mov        dword ptr [ebp - 0x20], eax
0048b6b9  push       ebx
0048b6ba  push       eax
0048b6bb  call       0x46edf8

// block 0048b6c0
0048b6c0  add        esp, 0x18

// block 0048b6c3
0048b6c3  cmp        dword ptr [ebp - 0x1c], edi
0048b6c6  jne        0x48b710

// block 0048b6c8
0048b6c8  cmp        esi, edi
0048b6ca  jne        0x48b6d2

// block 0048b6cc
0048b6cc  xor        esi, esi
0048b6ce  inc        esi
0048b6cf  mov        dword ptr [ebp + 0xc], esi

// block 0048b6d2
0048b6d2  add        esi, 0xf
0048b6d5  and        esi, 0xfffffff0
0048b6d8  mov        dword ptr [ebp + 0xc], esi
0048b6db  push       esi
0048b6dc  push       edi
0048b6dd  push       dword ptr [0x4b3c04]
0048b6e3  call       dword ptr [0x4981d4]

// block 0048b6e9
0048b6e9  mov        dword ptr [ebp - 0x1c], eax
0048b6ec  cmp        eax, edi
0048b6ee  je         0x48b710

// block 0048b6f0
0048b6f0  mov        eax, dword ptr [ebx - 4]
0048b6f3  dec        eax
0048b6f4  cmp        eax, esi
0048b6f6  jb         0x48b6fa

// block 0048b6f8
0048b6f8  mov        eax, esi

// block 0048b6fa
0048b6fa  push       eax
0048b6fb  push       ebx
0048b6fc  push       dword ptr [ebp - 0x1c]
0048b6ff  call       0x47a330

// block 0048b704
0048b704  push       ebx
0048b705  push       dword ptr [ebp - 0x20]
0048b708  call       0x46edf8

// block 0048b70d
0048b70d  add        esp, 0x14

// block 0048b710
0048b710  mov        dword ptr [ebp - 4], 0xfffffffe
0048b717  call       0x48b74a

// block 0048b71c
0048b71c  cmp        dword ptr [ebp - 0x20], 0
0048b720  jne        0x48b753

// block 0048b722
0048b722  test       esi, esi
0048b724  jne        0x48b727

// block 0048b726
0048b726  inc        esi

// block 0048b727
0048b727  add        esi, 0xf
0048b72a  and        esi, 0xfffffff0
0048b72d  mov        dword ptr [ebp + 0xc], esi
0048b730  push       esi
0048b731  push       ebx
0048b732  push       0
0048b734  push       dword ptr [0x4b3c04]
0048b73a  call       dword ptr [0x498180]

// block 0048b740
0048b740  mov        edi, eax
0048b742  jmp        0x48b756

// block 0048b753
0048b753  mov        edi, dword ptr [ebp - 0x1c]

// block 0048b756
0048b756  test       edi, edi
0048b758  jne        0x48b81d

// block 0048b75e
0048b75e  cmp        dword ptr [0x4b40bc], edi
0048b764  je         0x48b792

// block 0048b766
0048b766  push       esi
0048b767  call       0x46ff67

// block 0048b76c
0048b76c  pop        ecx
0048b76d  test       eax, eax
0048b76f  jne        0x48b647

// block 0048b775
0048b775  call       0x46e96f

// block 0048b77a
0048b77a  cmp        dword ptr [ebp - 0x20], edi
0048b77d  jne        0x48b7eb

// block 0048b77f
0048b77f  mov        esi, eax
0048b781  call       dword ptr [0x4980e4]

// block 0048b787
0048b787  push       eax
0048b788  call       0x46e92d

// block 0048b78d
0048b78d  pop        ecx
0048b78e  mov        dword ptr [esi], eax
0048b790  jmp        0x48b7f1

// block 0048b792
0048b792  test       edi, edi
0048b794  jne        0x48b81d

// block 0048b79a
0048b79a  call       0x46e96f

// block 0048b79f
0048b79f  cmp        dword ptr [ebp - 0x20], edi
0048b7a2  je         0x48b80c

// block 0048b7a4
0048b7a4  mov        dword ptr [eax], 0xc
0048b7aa  jmp        0x48b81d

// block 0048b7ac
0048b7ac  test       esi, esi
0048b7ae  jne        0x48b7b1

// block 0048b7b0
0048b7b0  inc        esi

// block 0048b7b1
0048b7b1  push       esi
0048b7b2  push       ebx
0048b7b3  push       0
0048b7b5  push       dword ptr [0x4b3c04]
0048b7bb  call       dword ptr [0x498180]

// block 0048b7c1
0048b7c1  mov        edi, eax
0048b7c3  test       edi, edi
0048b7c5  jne        0x48b81d

// block 0048b7c7
0048b7c7  cmp        dword ptr [0x4b40bc], eax
0048b7cd  je         0x48b803

// block 0048b7cf
0048b7cf  push       esi
0048b7d0  call       0x46ff67

// block 0048b7d5
0048b7d5  pop        ecx
0048b7d6  test       eax, eax
0048b7d8  je         0x48b7f9

// block 0048b7da
0048b7da  cmp        esi, -0x20
0048b7dd  jbe        0x48b7ac

// block 0048b7df
0048b7df  push       esi
0048b7e0  call       0x46ff67

// block 0048b7e5
0048b7e5  pop        ecx
0048b7e6  call       0x46e96f

// block 0048b7eb
0048b7eb  mov        dword ptr [eax], 0xc

// block 0048b7f1
0048b7f1  xor        eax, eax

// block 0048b7f3
0048b7f3  call       0x46fd31

// block 0048b7f8
0048b7f8  ret        

// block 0048b7f9
0048b7f9  call       0x46e96f

// block 0048b7fe
0048b7fe  jmp        0x48b77f

// block 0048b803
0048b803  test       edi, edi
0048b805  jne        0x48b81d

// block 0048b807
0048b807  call       0x46e96f

// block 0048b80c
0048b80c  mov        esi, eax
0048b80e  call       dword ptr [0x4980e4]

// block 0048b814
0048b814  push       eax
0048b815  call       0x46e92d

// block 0048b81a
0048b81a  mov        dword ptr [esi], eax
0048b81c  pop        ecx

// block 0048b81d
0048b81d  mov        eax, edi
0048b81f  jmp        0x48b7f3
