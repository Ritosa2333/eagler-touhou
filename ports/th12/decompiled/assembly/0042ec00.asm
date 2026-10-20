
// block 0042ec00
0042ec00  push       -1
0042ec02  push       0x497336
0042ec07  mov        eax, dword ptr fs:[0]
0042ec0d  push       eax
0042ec0e  push       ecx
0042ec0f  push       ebx
0042ec10  push       ebp
0042ec11  push       esi
0042ec12  push       edi
0042ec13  mov        eax, dword ptr [0x4ad138]
0042ec18  xor        eax, esp
0042ec1a  push       eax
0042ec1b  lea        eax, [esp + 0x18]
0042ec1f  mov        dword ptr fs:[0], eax
0042ec25  mov        ebp, dword ptr [esp + 0x28]
0042ec29  lea        esi, [ebp + 0x10]
0042ec2c  mov        dword ptr [esp + 0x20], 1
0042ec34  mov        dword ptr [esp + 0x14], esi
0042ec38  call       0x464c40

// block 0042ec3d
0042ec3d  mov        edi, dword ptr [ebp + 8]
0042ec40  mov        ebx, dword ptr [0x4ce89c]
0042ec46  mov        esi, dword ptr [0x498088]
0042ec4c  test       edi, edi
0042ec4e  je         0x42ec8f

// block 0042ec50
0042ec50  test       dword ptr [0x4cee78], 0x8000
0042ec5a  je         0x42ec69

// block 0042ec5c
0042ec5c  push       0x4cf0f8
0042ec61  call       esi

// block 0042ec63
0042ec63  inc        byte ptr [0x4cf218]

// block 0042ec69
0042ec69  mov        ecx, edi
0042ec6b  mov        edx, ebx
0042ec6d  call       0x462890

// block 0042ec72
0042ec72  test       dword ptr [0x4cee78], 0x8000
0042ec7c  je         0x42ec8f

// block 0042ec7e
0042ec7e  push       0x4cf0f8
0042ec83  call       dword ptr [0x49808c]

// block 0042ec89
0042ec89  dec        byte ptr [0x4cf218]

// block 0042ec8f
0042ec8f  mov        edi, dword ptr [ebp + 0xc]
0042ec92  mov        ebx, dword ptr [0x4ce89c]
0042ec98  test       edi, edi
0042ec9a  je         0x42ecdb

// block 0042ec9c
0042ec9c  test       dword ptr [0x4cee78], 0x8000
0042eca6  je         0x42ecb5

// block 0042eca8
0042eca8  push       0x4cf0f8
0042ecad  call       esi

// block 0042ecaf
0042ecaf  inc        byte ptr [0x4cf218]

// block 0042ecb5
0042ecb5  mov        ecx, edi
0042ecb7  mov        edx, ebx
0042ecb9  call       0x462890

// block 0042ecbe
0042ecbe  test       dword ptr [0x4cee78], 0x8000
0042ecc8  je         0x42ecdb

// block 0042ecca
0042ecca  push       0x4cf0f8
0042eccf  call       dword ptr [0x49808c]

// block 0042ecd5
0042ecd5  dec        byte ptr [0x4cf218]

// block 0042ecdb
0042ecdb  call       0x42e950

// block 0042ece0
0042ece0  mov        ebx, dword ptr [0x4ce8cc]
0042ece6  mov        edi, dword ptr [ebx + 0x4b50c4]
0042ecec  add        ebx, 0x4b50c4
0042ecf2  xor        esi, esi
0042ecf4  cmp        edi, esi
0042ecf6  je         0x42ed0a

// block 0042ecf8
0042ecf8  call       0x4604e0

// block 0042ecfd
0042ecfd  mov        eax, dword ptr [ebx]
0042ecff  push       eax
0042ed00  call       0x46ca4f

// block 0042ed05
0042ed05  add        esp, 4
0042ed08  mov        dword ptr [ebx], esi

// block 0042ed0a
0042ed0a  mov        edi, dword ptr [0x4b43b8]
0042ed10  mov        dword ptr [0x4b44f8], esi
0042ed16  cmp        edi, esi
0042ed18  je         0x42ed29

// block 0042ed1a
0042ed1a  push       edi
0042ed1b  call       0x401220

// block 0042ed20
0042ed20  push       edi
0042ed21  call       0x46ca4f

// block 0042ed26
0042ed26  add        esp, 4

// block 0042ed29
0042ed29  mov        ebx, dword ptr [0x4ce8cc]
0042ed2f  mov        edi, dword ptr [ebx + 0x4b50c0]
0042ed35  add        ebx, 0x4b50c0
0042ed3b  cmp        edi, esi
0042ed3d  je         0x42ed51

// block 0042ed3f
0042ed3f  call       0x4604e0

// block 0042ed44
0042ed44  mov        ecx, dword ptr [ebx]
0042ed46  push       ecx
0042ed47  call       0x46ca4f

// block 0042ed4c
0042ed4c  add        esp, 4
0042ed4f  mov        dword ptr [ebx], esi

// block 0042ed51
0042ed51  call       0x43d2e0

// block 0042ed56
0042ed56  mov        edi, dword ptr [0x4b451c]
0042ed5c  cmp        edi, esi
0042ed5e  je         0x42ed8d

// block 0042ed60
0042ed60  mov        eax, dword ptr [edi]
0042ed62  cmp        eax, esi
0042ed64  je         0x42ed71

// block 0042ed66
0042ed66  push       eax
0042ed67  call       0x46c941

// block 0042ed6c
0042ed6c  add        esp, 4
0042ed6f  mov        dword ptr [edi], esi

// block 0042ed71
0042ed71  mov        eax, dword ptr [edi + 4]
0042ed74  cmp        eax, esi
0042ed76  je         0x42ed84

// block 0042ed78
0042ed78  push       eax
0042ed79  call       0x46c941

// block 0042ed7e
0042ed7e  add        esp, 4
0042ed81  mov        dword ptr [edi + 4], esi

// block 0042ed84
0042ed84  push       edi
0042ed85  call       0x46ca4f

// block 0042ed8a
0042ed8a  add        esp, 4

// block 0042ed8d
0042ed8d  mov        eax, dword ptr [ebp + 0x4a8]
0042ed93  mov        dword ptr [0x4b451c], esi
0042ed99  cmp        eax, esi
0042ed9b  je         0x42eda6

// block 0042ed9d
0042ed9d  push       eax
0042ed9e  call       0x46c941

// block 0042eda3
0042eda3  add        esp, 4

// block 0042eda6
0042eda6  mov        dword ptr [ebp + 0x4a8], esi
0042edac  mov        esi, dword ptr [esp + 0x14]
0042edb0  mov        dword ptr [esi], 0x4a3738
0042edb6  call       0x464c40

// block 0042edbb
0042edbb  mov        ecx, dword ptr [esp + 0x18]
0042edbf  mov        dword ptr fs:[0], ecx
0042edc6  pop        ecx
0042edc7  pop        edi
0042edc8  pop        esi
0042edc9  pop        ebp
0042edca  pop        ebx
0042edcb  add        esp, 0x10
0042edce  ret        4
