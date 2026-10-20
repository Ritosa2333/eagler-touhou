
// block 0048d678
0048d678  mov        edi, edi
0048d67a  push       ebp
0048d67b  mov        ebp, esp
0048d67d  sub        esp, 0x34
0048d680  mov        eax, dword ptr [0x4ad138]
0048d685  xor        eax, ebp
0048d687  mov        dword ptr [ebp - 4], eax
0048d68a  mov        eax, dword ptr [ebp + 0x10]
0048d68d  mov        ecx, dword ptr [ebp + 0x18]
0048d690  mov        dword ptr [ebp - 0x28], eax
0048d693  mov        eax, dword ptr [ebp + 0x14]
0048d696  push       ebx
0048d697  mov        dword ptr [ebp - 0x30], eax
0048d69a  mov        eax, dword ptr [eax]
0048d69c  push       esi
0048d69d  mov        dword ptr [ebp - 0x24], eax
0048d6a0  mov        eax, dword ptr [ebp + 8]
0048d6a3  push       edi
0048d6a4  xor        edi, edi
0048d6a6  mov        dword ptr [ebp - 0x34], ecx
0048d6a9  mov        dword ptr [ebp - 0x20], edi
0048d6ac  mov        dword ptr [ebp - 0x2c], edi
0048d6af  cmp        eax, dword ptr [ebp + 0xc]
0048d6b2  je         0x48d817

// block 0048d6b8
0048d6b8  mov        esi, dword ptr [0x498164]
0048d6be  lea        ecx, [ebp - 0x18]
0048d6c1  push       ecx
0048d6c2  push       eax
0048d6c3  call       esi

// block 0048d6c5
0048d6c5  mov        ebx, dword ptr [0x4980dc]
0048d6cb  test       eax, eax
0048d6cd  je         0x48d72d

// block 0048d6cf
0048d6cf  cmp        dword ptr [ebp - 0x18], 1
0048d6d3  jne        0x48d72d

// block 0048d6d5
0048d6d5  lea        eax, [ebp - 0x18]
0048d6d8  push       eax
0048d6d9  push       dword ptr [ebp + 0xc]
0048d6dc  call       esi

// block 0048d6de
0048d6de  test       eax, eax
0048d6e0  je         0x48d72d

// block 0048d6e2
0048d6e2  cmp        dword ptr [ebp - 0x18], 1
0048d6e6  jne        0x48d72d

// block 0048d6e8
0048d6e8  mov        esi, dword ptr [ebp - 0x24]
0048d6eb  mov        dword ptr [ebp - 0x2c], 1
0048d6f2  cmp        esi, -1
0048d6f5  jne        0x48d703

// block 0048d6f7
0048d6f7  push       dword ptr [ebp - 0x28]
0048d6fa  call       0x475860

// block 0048d6ff
0048d6ff  mov        esi, eax
0048d701  pop        ecx
0048d702  inc        esi

// block 0048d703
0048d703  cmp        esi, edi

// block 0048d705
0048d705  jle        0x48d762

// block 0048d707
0048d707  cmp        esi, 0x7ffffff0
0048d70d  ja         0x48d762

// block 0048d70f
0048d70f  lea        eax, [esi + esi + 8]
0048d713  cmp        eax, 0x400
0048d718  ja         0x48d749

// block 0048d71a
0048d71a  call       0x48d830

// block 0048d71f
0048d71f  mov        eax, esp
0048d721  cmp        eax, edi
0048d723  je         0x48d75d

// block 0048d725
0048d725  mov        dword ptr [eax], 0xcccc
0048d72b  jmp        0x48d75a

// block 0048d72d
0048d72d  push       edi
0048d72e  push       edi
0048d72f  push       dword ptr [ebp - 0x24]
0048d732  push       dword ptr [ebp - 0x28]
0048d735  push       1
0048d737  push       dword ptr [ebp + 8]
0048d73a  call       ebx

// block 0048d73c
0048d73c  mov        esi, eax
0048d73e  cmp        esi, edi
0048d740  jne        0x48d705

// block 0048d742
0048d742  xor        eax, eax
0048d744  jmp        0x48d81a

// block 0048d749
0048d749  push       eax
0048d74a  call       0x46d04a

// block 0048d74f
0048d74f  pop        ecx
0048d750  cmp        eax, edi
0048d752  je         0x48d75d

// block 0048d754
0048d754  mov        dword ptr [eax], 0xdddd

// block 0048d75a
0048d75a  add        eax, 8

// block 0048d75d
0048d75d  mov        dword ptr [ebp - 0x1c], eax
0048d760  jmp        0x48d765

// block 0048d762
0048d762  mov        dword ptr [ebp - 0x1c], edi

// block 0048d765
0048d765  cmp        dword ptr [ebp - 0x1c], edi
0048d768  je         0x48d742

// block 0048d76a
0048d76a  lea        eax, [esi + esi]
0048d76d  push       eax
0048d76e  push       edi
0048d76f  push       dword ptr [ebp - 0x1c]
0048d772  call       0x477420

// block 0048d777
0048d777  add        esp, 0xc
0048d77a  push       esi
0048d77b  push       dword ptr [ebp - 0x1c]
0048d77e  push       dword ptr [ebp - 0x24]
0048d781  push       dword ptr [ebp - 0x28]
0048d784  push       1
0048d786  push       dword ptr [ebp + 8]
0048d789  call       ebx

// block 0048d78b
0048d78b  test       eax, eax
0048d78d  je         0x48d80e

// block 0048d78f
0048d78f  mov        ebx, dword ptr [ebp - 0x34]
0048d792  cmp        ebx, edi
0048d794  je         0x48d7b3

// block 0048d796
0048d796  push       edi
0048d797  push       edi
0048d798  push       dword ptr [ebp + 0x1c]
0048d79b  push       ebx
0048d79c  push       esi
0048d79d  push       dword ptr [ebp - 0x1c]
0048d7a0  push       edi
0048d7a1  push       dword ptr [ebp + 0xc]
0048d7a4  call       dword ptr [0x498134]

// block 0048d7aa
0048d7aa  test       eax, eax
0048d7ac  je         0x48d80e

// block 0048d7ae
0048d7ae  mov        dword ptr [ebp - 0x20], ebx
0048d7b1  jmp        0x48d80e

// block 0048d7b3
0048d7b3  mov        ebx, dword ptr [0x498134]
0048d7b9  cmp        dword ptr [ebp - 0x2c], edi
0048d7bc  jne        0x48d7d2

// block 0048d7be
0048d7be  push       edi
0048d7bf  push       edi
0048d7c0  push       edi
0048d7c1  push       edi
0048d7c2  push       esi
0048d7c3  push       dword ptr [ebp - 0x1c]
0048d7c6  push       edi
0048d7c7  push       dword ptr [ebp + 0xc]
0048d7ca  call       ebx

// block 0048d7cc
0048d7cc  mov        esi, eax
0048d7ce  cmp        esi, edi
0048d7d0  je         0x48d80e

// block 0048d7d2
0048d7d2  push       esi
0048d7d3  push       1
0048d7d5  call       0x477813

// block 0048d7da
0048d7da  pop        ecx
0048d7db  pop        ecx
0048d7dc  mov        dword ptr [ebp - 0x20], eax
0048d7df  cmp        eax, edi
0048d7e1  je         0x48d80e

// block 0048d7e3
0048d7e3  push       edi
0048d7e4  push       edi
0048d7e5  push       esi
0048d7e6  push       eax
0048d7e7  push       esi
0048d7e8  push       dword ptr [ebp - 0x1c]
0048d7eb  push       edi
0048d7ec  push       dword ptr [ebp + 0xc]
0048d7ef  call       ebx

// block 0048d7f1
0048d7f1  cmp        eax, edi
0048d7f3  jne        0x48d803

// block 0048d7f5
0048d7f5  push       dword ptr [ebp - 0x20]
0048d7f8  call       0x46c941

// block 0048d7fd
0048d7fd  pop        ecx
0048d7fe  mov        dword ptr [ebp - 0x20], edi
0048d801  jmp        0x48d80e

// block 0048d803
0048d803  cmp        dword ptr [ebp - 0x24], -1
0048d807  je         0x48d80e

// block 0048d809
0048d809  mov        ecx, dword ptr [ebp - 0x30]
0048d80c  mov        dword ptr [ecx], eax

// block 0048d80e
0048d80e  push       dword ptr [ebp - 0x1c]
0048d811  call       0x48326a

// block 0048d816
0048d816  pop        ecx

// block 0048d817
0048d817  mov        eax, dword ptr [ebp - 0x20]

// block 0048d81a
0048d81a  lea        esp, [ebp - 0x40]
0048d81d  pop        edi
0048d81e  pop        esi
0048d81f  pop        ebx
0048d820  mov        ecx, dword ptr [ebp - 4]
0048d823  xor        ecx, ebp
0048d825  call       0x46c932

// block 0048d82a
0048d82a  leave      
0048d82b  ret        
