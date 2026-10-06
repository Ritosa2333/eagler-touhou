
// block 0046e69f
0046e69f  push       0x58
0046e6a1  push       0x4aaaf8
0046e6a6  call       0x46fcec

// block 0046e6ab
0046e6ab  xor        esi, esi
0046e6ad  mov        dword ptr [ebp - 4], esi
0046e6b0  lea        eax, [ebp - 0x68]
0046e6b3  push       eax
0046e6b4  call       dword ptr [0x4981bc]

// block 0046e6ba
0046e6ba  push       -2
0046e6bc  pop        edi
0046e6bd  mov        dword ptr [ebp - 4], edi
0046e6c0  mov        eax, 0x5a4d
0046e6c5  cmp        word ptr [0x400000], ax
0046e6cc  jne        0x46e706

// block 0046e6ce
0046e6ce  mov        eax, dword ptr [0x40003c]
0046e6d3  cmp        dword ptr [eax + 0x400000], 0x4550
0046e6dd  jne        0x46e706

// block 0046e6df
0046e6df  mov        ecx, 0x10b
0046e6e4  cmp        word ptr [eax + 0x400018], cx
0046e6eb  jne        0x46e706

// block 0046e6ed
0046e6ed  cmp        dword ptr [eax + 0x400074], 0xe
0046e6f4  jbe        0x46e706

// block 0046e6f6
0046e6f6  xor        ecx, ecx
0046e6f8  cmp        dword ptr [eax + 0x4000e8], esi
0046e6fe  setne      cl
0046e701  mov        dword ptr [ebp - 0x1c], ecx
0046e704  jmp        0x46e709

// block 0046e706
0046e706  mov        dword ptr [ebp - 0x1c], esi

// block 0046e709
0046e709  xor        ebx, ebx
0046e70b  inc        ebx
0046e70c  push       ebx
0046e70d  call       0x46ea5c

// block 0046e712
0046e712  pop        ecx
0046e713  test       eax, eax
0046e715  jne        0x46e71f

// block 0046e717
0046e717  push       0x1c
0046e719  call       0x46e62f

// block 0046e71f
0046e71f  call       0x47562a

// block 0046e724
0046e724  test       eax, eax
0046e726  jne        0x46e730

// block 0046e728
0046e728  push       0x10
0046e72a  call       0x46e62f

// block 0046e730
0046e730  call       0x47b31d

// block 0046e735
0046e735  mov        dword ptr [ebp - 4], ebx
0046e738  call       0x47b07b

// block 0046e73d
0046e73d  test       eax, eax
0046e73f  jge        0x46e749

// block 0046e741
0046e741  push       0x1b
0046e743  call       0x472c0d

// block 0046e748
0046e748  pop        ecx

// block 0046e749
0046e749  call       dword ptr [0x4981f4]

// block 0046e74f
0046e74f  mov        dword ptr [0x4d645c], eax
0046e754  call       0x47af44

// block 0046e759
0046e759  mov        dword ptr [0x4b38d0], eax
0046e75e  call       0x47ae89

// block 0046e763
0046e763  test       eax, eax
0046e765  jge        0x46e76f

// block 0046e767
0046e767  push       8
0046e769  call       0x472c0d

// block 0046e76e
0046e76e  pop        ecx

// block 0046e76f
0046e76f  call       0x47ac02

// block 0046e774
0046e774  test       eax, eax
0046e776  jge        0x46e780

// block 0046e778
0046e778  push       9
0046e77a  call       0x472c0d

// block 0046e77f
0046e77f  pop        ecx

// block 0046e780
0046e780  push       ebx
0046e781  call       0x472d44

// block 0046e786
0046e786  pop        ecx
0046e787  cmp        eax, esi
0046e789  je         0x46e792

// block 0046e78b
0046e78b  push       eax
0046e78c  call       0x472c0d

// block 0046e791
0046e791  pop        ecx

// block 0046e792
0046e792  call       0x47aba3

// block 0046e797
0046e797  test       byte ptr [ebp - 0x3c], bl
0046e79a  je         0x46e7a2

// block 0046e79c
0046e79c  movzx      ecx, word ptr [ebp - 0x38]
0046e7a0  jmp        0x46e7a5

// block 0046e7a2
0046e7a2  push       0xa
0046e7a4  pop        ecx

// block 0046e7a5
0046e7a5  push       ecx
0046e7a6  push       eax
0046e7a7  push       esi
0046e7a8  push       0x400000
0046e7ad  call       0x44f560

// block 0046e7b2
0046e7b2  mov        dword ptr [ebp - 0x20], eax
0046e7b5  cmp        dword ptr [ebp - 0x1c], esi
0046e7b8  jne        0x46e7c0

// block 0046e7ba
0046e7ba  push       eax
0046e7bb  call       0x472ef5

// block 0046e7c0
0046e7c0  call       0x472f21

// block 0046e7c5
0046e7c5  mov        dword ptr [ebp - 4], edi
0046e7c8  jmp        0x46e7ff

// block 0046e7ff
0046e7ff  mov        eax, dword ptr [ebp - 0x20]
0046e802  jmp        0x46e817

// block 0046e817
0046e817  call       0x46fd31

// block 0046e81c
0046e81c  ret        
