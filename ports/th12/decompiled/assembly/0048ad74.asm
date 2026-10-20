
// block 0048ad74
0048ad74  mov        edi, edi
0048ad76  push       ebp
0048ad77  mov        ebp, esp
0048ad79  sub        esp, 0x18
0048ad7c  mov        eax, dword ptr [0x4ad138]
0048ad81  xor        eax, ebp
0048ad83  mov        dword ptr [ebp - 4], eax
0048ad86  mov        eax, dword ptr [ebp + 0x10]
0048ad89  push       ebx
0048ad8a  push       esi
0048ad8b  xor        esi, esi
0048ad8d  push       edi
0048ad8e  mov        dword ptr [ebp - 0x18], 0x404e
0048ad95  mov        dword ptr [eax], esi
0048ad97  mov        dword ptr [eax + 4], esi
0048ad9a  mov        dword ptr [eax + 8], esi
0048ad9d  cmp        dword ptr [ebp + 0xc], esi
0048ada0  jbe        0x48aeec

// block 0048ada6
0048ada6  mov        edx, dword ptr [eax]
0048ada8  mov        ebx, dword ptr [eax + 4]
0048adab  mov        esi, eax
0048adad  lea        edi, [ebp - 0x10]
0048adb0  movsd      dword ptr es:[edi], dword ptr [esi]
0048adb1  movsd      dword ptr es:[edi], dword ptr [esi]
0048adb2  movsd      dword ptr es:[edi], dword ptr [esi]
0048adb3  mov        ecx, edx
0048adb5  shr        ecx, 0x1f
0048adb8  lea        edi, [edx + edx]
0048adbb  lea        edx, [ebx + ebx]
0048adbe  or         edx, ecx
0048adc0  mov        ecx, dword ptr [eax + 8]
0048adc3  mov        esi, ebx
0048adc5  shr        esi, 0x1f
0048adc8  add        ecx, ecx
0048adca  or         ecx, esi
0048adcc  mov        dword ptr [ebp - 0x14], edi
0048adcf  mov        esi, edi
0048add1  and        dword ptr [ebp - 0x14], 0
0048add5  mov        ebx, edx
0048add7  shr        ebx, 0x1f
0048adda  add        ecx, ecx
0048addc  shr        edi, 0x1f
0048addf  or         ecx, ebx
0048ade1  mov        ebx, dword ptr [ebp - 0x10]
0048ade4  add        esi, esi
0048ade6  add        edx, edx
0048ade8  or         edx, edi
0048adea  lea        edi, [esi + ebx]
0048aded  mov        dword ptr [eax], esi
0048adef  mov        dword ptr [eax + 4], edx
0048adf2  mov        dword ptr [eax + 8], ecx
0048adf5  cmp        edi, esi
0048adf7  jb         0x48adfd

// block 0048adf9
0048adf9  cmp        edi, ebx
0048adfb  jae        0x48ae04

// block 0048adfd
0048adfd  mov        dword ptr [ebp - 0x14], 1

// block 0048ae04
0048ae04  xor        ebx, ebx
0048ae06  mov        dword ptr [eax], edi
0048ae08  cmp        dword ptr [ebp - 0x14], ebx
0048ae0b  je         0x48ae27

// block 0048ae0d
0048ae0d  lea        esi, [edx + 1]
0048ae10  cmp        esi, edx
0048ae12  jb         0x48ae19

// block 0048ae14
0048ae14  cmp        esi, 1
0048ae17  jae        0x48ae1c

// block 0048ae19
0048ae19  xor        ebx, ebx
0048ae1b  inc        ebx

// block 0048ae1c
0048ae1c  mov        dword ptr [eax + 4], esi
0048ae1f  test       ebx, ebx
0048ae21  je         0x48ae27

// block 0048ae23
0048ae23  inc        ecx
0048ae24  mov        dword ptr [eax + 8], ecx

// block 0048ae27
0048ae27  mov        ecx, dword ptr [eax + 4]
0048ae2a  mov        edx, dword ptr [ebp - 0xc]
0048ae2d  lea        ebx, [ecx + edx]
0048ae30  xor        esi, esi
0048ae32  cmp        ebx, ecx
0048ae34  jb         0x48ae3a

// block 0048ae36
0048ae36  cmp        ebx, edx
0048ae38  jae        0x48ae3d

// block 0048ae3a
0048ae3a  xor        esi, esi
0048ae3c  inc        esi

// block 0048ae3d
0048ae3d  mov        dword ptr [eax + 4], ebx
0048ae40  test       esi, esi
0048ae42  je         0x48ae47

// block 0048ae44
0048ae44  inc        dword ptr [eax + 8]

// block 0048ae47
0048ae47  mov        ecx, dword ptr [ebp - 8]
0048ae4a  add        dword ptr [eax + 8], ecx
0048ae4d  and        dword ptr [ebp - 0x14], 0
0048ae51  lea        ecx, [edi + edi]
0048ae54  mov        edx, edi
0048ae56  shr        edx, 0x1f
0048ae59  lea        edi, [ebx + ebx]
0048ae5c  or         edi, edx
0048ae5e  mov        edx, dword ptr [eax + 8]
0048ae61  mov        esi, ebx
0048ae63  shr        esi, 0x1f
0048ae66  lea        ebx, [edx + edx]
0048ae69  mov        edx, dword ptr [ebp + 8]
0048ae6c  or         ebx, esi
0048ae6e  mov        dword ptr [eax], ecx
0048ae70  mov        dword ptr [eax + 4], edi
0048ae73  mov        dword ptr [eax + 8], ebx
0048ae76  movsx      edx, byte ptr [edx]
0048ae79  lea        esi, [ecx + edx]
0048ae7c  mov        dword ptr [ebp - 0x10], edx
0048ae7f  cmp        esi, ecx
0048ae81  jb         0x48ae87

// block 0048ae83
0048ae83  cmp        esi, edx
0048ae85  jae        0x48ae8e

// block 0048ae87
0048ae87  mov        dword ptr [ebp - 0x14], 1

// block 0048ae8e
0048ae8e  cmp        dword ptr [ebp - 0x14], 0
0048ae92  mov        dword ptr [eax], esi
0048ae94  je         0x48aeb2

// block 0048ae96
0048ae96  lea        ecx, [edi + 1]
0048ae99  xor        edx, edx
0048ae9b  cmp        ecx, edi
0048ae9d  jb         0x48aea4

// block 0048ae9f
0048ae9f  cmp        ecx, 1
0048aea2  jae        0x48aea7

// block 0048aea4
0048aea4  xor        edx, edx
0048aea6  inc        edx

// block 0048aea7
0048aea7  mov        dword ptr [eax + 4], ecx
0048aeaa  test       edx, edx
0048aeac  je         0x48aeb2

// block 0048aeae
0048aeae  inc        ebx
0048aeaf  mov        dword ptr [eax + 8], ebx

// block 0048aeb2
0048aeb2  dec        dword ptr [ebp + 0xc]
0048aeb5  inc        dword ptr [ebp + 8]
0048aeb8  cmp        dword ptr [ebp + 0xc], 0
0048aebc  ja         0x48ada6

// block 0048aec2
0048aec2  xor        esi, esi
0048aec4  jmp        0x48aeec

// block 0048aec6
0048aec6  mov        ecx, dword ptr [eax + 4]
0048aec9  mov        edx, ecx
0048aecb  shr        edx, 0x10
0048aece  mov        dword ptr [eax + 8], edx
0048aed1  mov        edx, dword ptr [eax]
0048aed3  mov        edi, edx
0048aed5  shl        ecx, 0x10
0048aed8  shr        edi, 0x10
0048aedb  or         ecx, edi
0048aedd  shl        edx, 0x10
0048aee0  add        dword ptr [ebp - 0x18], 0xfff0
0048aee7  mov        dword ptr [eax + 4], ecx
0048aeea  mov        dword ptr [eax], edx

// block 0048aeec
0048aeec  cmp        dword ptr [eax + 8], esi
0048aeef  je         0x48aec6

// block 0048aef1
0048aef1  mov        ebx, 0x8000
0048aef6  test       dword ptr [eax + 8], ebx
0048aef9  jne        0x48af2b

// block 0048aefb
0048aefb  mov        esi, dword ptr [eax]
0048aefd  mov        edi, dword ptr [eax + 4]
0048af00  add        dword ptr [ebp - 0x18], 0xffff
0048af07  mov        ecx, esi
0048af09  add        esi, esi
0048af0b  shr        ecx, 0x1f
0048af0e  mov        dword ptr [eax], esi
0048af10  lea        esi, [edi + edi]
0048af13  or         esi, ecx
0048af15  mov        ecx, dword ptr [eax + 8]
0048af18  mov        edx, edi
0048af1a  shr        edx, 0x1f
0048af1d  add        ecx, ecx
0048af1f  or         ecx, edx
0048af21  mov        dword ptr [eax + 4], esi
0048af24  mov        dword ptr [eax + 8], ecx
0048af27  test       ebx, ecx
0048af29  je         0x48aefb

// block 0048af2b
0048af2b  mov        cx, word ptr [ebp - 0x18]
0048af2f  mov        word ptr [eax + 0xa], cx
0048af33  mov        ecx, dword ptr [ebp - 4]
0048af36  pop        edi
0048af37  pop        esi
0048af38  xor        ecx, ebp
0048af3a  pop        ebx
0048af3b  call       0x46c932

// block 0048af40
0048af40  leave      
0048af41  ret        
