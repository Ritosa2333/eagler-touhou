
// block 0044d8b0
0044d8b0  mov        eax, dword ptr [0x4b0e48]
0044d8b5  sub        esp, 0x18
0044d8b8  push       ebx
0044d8b9  push       ebp
0044d8ba  push       esi
0044d8bb  push       edi
0044d8bc  cmp        eax, 0x15
0044d8bf  je         0x44da9c

// block 0044d8c5
0044d8c5  cmp        eax, 0x1a
0044d8c8  jne        0x44dc0c

// block 0044d8ce
0044d8ce  cmp        dword ptr [esp + 0x2c], 0
0044d8d3  mov        ebp, dword ptr [0x4b0e68]
0044d8d9  mov        dword ptr [esp + 0x10], 0
0044d8e1  jbe        0x44dc0c

// block 0044d8e7
0044d8e7  mov        esi, dword ptr [0x4b0e4c]
0044d8ed  lea        ecx, [ecx]

// block 0044d8f0
0044d8f0  xor        edx, edx
0044d8f2  mov        dword ptr [esp + 0x14], edx
0044d8f6  test       esi, esi
0044d8f8  jbe        0x44da7d

// block 0044d8fe
0044d8fe  mov        edi, edi

// block 0044d900
0044d900  mov        eax, 0xf000
0044d905  test       word ptr [ebp], ax
0044d909  jne        0x44da6d

// block 0044d90f
0044d90f  xor        edi, edi
0044d911  xor        ebx, ebx
0044d913  xor        ecx, ecx
0044d915  mov        dword ptr [esp + 0x24], edi
0044d919  test       edx, edx
0044d91b  jbe        0x44d947

// block 0044d91d
0044d91d  movzx      eax, word ptr [ebp - 2]
0044d921  test       eax, 0xf000
0044d926  je         0x44d947

// block 0044d928
0044d928  movzx      eax, ax
0044d92b  mov        ecx, eax
0044d92d  mov        ebx, eax
0044d92f  shr        ecx, 8
0044d932  shr        ebx, 4
0044d935  and        ecx, 0xf
0044d938  and        ebx, 0xf
0044d93b  and        eax, 0xf
0044d93e  mov        dword ptr [esp + 0x24], eax
0044d942  mov        edi, 1

// block 0044d947
0044d947  dec        esi
0044d948  cmp        edx, esi
0044d94a  jae        0x44d976

// block 0044d94c
0044d94c  movzx      eax, word ptr [ebp + 2]
0044d950  test       eax, 0xf000
0044d955  je         0x44d976

// block 0044d957
0044d957  movzx      esi, ax
0044d95a  mov        edx, esi
0044d95c  mov        eax, esi
0044d95e  shr        edx, 8
0044d961  shr        eax, 4
0044d964  and        edx, 0xf
0044d967  and        eax, 0xf
0044d96a  and        esi, 0xf
0044d96d  add        dword ptr [esp + 0x24], esi
0044d971  add        ecx, edx
0044d973  add        ebx, eax
0044d975  inc        edi

// block 0044d976
0044d976  cmp        dword ptr [esp + 0x10], 0
0044d97b  mov        esi, dword ptr [0x4b0e58]
0044d981  jbe        0x44d9bb

// block 0044d983
0044d983  mov        eax, esi
0044d985  cdq        
0044d986  sub        eax, edx
0044d988  sar        eax, 1
0044d98a  add        eax, eax
0044d98c  mov        edx, eax
0044d98e  mov        eax, ebp
0044d990  sub        eax, edx
0044d992  movzx      eax, word ptr [eax]
0044d995  test       eax, 0xf000
0044d99a  je         0x44d9bb

// block 0044d99c
0044d99c  movzx      eax, ax
0044d99f  mov        edx, eax
0044d9a1  shr        edx, 8
0044d9a4  and        edx, 0xf
0044d9a7  add        ecx, edx
0044d9a9  mov        edx, eax
0044d9ab  shr        edx, 4
0044d9ae  and        edx, 0xf
0044d9b1  and        eax, 0xf
0044d9b4  add        dword ptr [esp + 0x24], eax
0044d9b8  add        ebx, edx
0044d9ba  inc        edi

// block 0044d9bb
0044d9bb  mov        eax, dword ptr [0x4b0e50]
0044d9c0  dec        eax
0044d9c1  cmp        dword ptr [esp + 0x10], eax
0044d9c5  jae        0x44d9fb

// block 0044d9c7
0044d9c7  mov        eax, esi
0044d9c9  cdq        
0044d9ca  sub        eax, edx
0044d9cc  sar        eax, 1
0044d9ce  lea        eax, [ebp + eax*2]
0044d9d2  movzx      eax, word ptr [eax]
0044d9d5  test       eax, 0xf000
0044d9da  je         0x44d9fb

// block 0044d9dc
0044d9dc  movzx      eax, ax
0044d9df  mov        edx, eax
0044d9e1  shr        edx, 8
0044d9e4  and        edx, 0xf
0044d9e7  add        ecx, edx
0044d9e9  mov        edx, eax
0044d9eb  shr        edx, 4
0044d9ee  and        edx, 0xf
0044d9f1  and        eax, 0xf
0044d9f4  add        dword ptr [esp + 0x24], eax
0044d9f8  add        ebx, edx
0044d9fa  inc        edi

// block 0044d9fb
0044d9fb  cmp        edi, 1
0044d9fe  jbe        0x44da1c

// block 0044da00
0044da00  xor        edx, edx
0044da02  mov        eax, ecx
0044da04  div        edi
0044da06  xor        edx, edx
0044da08  mov        ecx, eax
0044da0a  mov        eax, ebx
0044da0c  div        edi
0044da0e  xor        edx, edx
0044da10  mov        ebx, eax
0044da12  mov        eax, dword ptr [esp + 0x24]
0044da16  div        edi
0044da18  mov        dword ptr [esp + 0x24], eax

// block 0044da1c
0044da1c  mov        eax, dword ptr [esp + 0x24]
0044da20  shr        ecx, 1
0044da22  and        cl, 0xf
0044da25  movzx      cx, cl
0044da29  shr        ebx, 1
0044da2b  and        bl, 0xf
0044da2e  shl        cx, 4
0044da32  movzx      dx, bl
0044da36  or         cx, dx
0044da39  mov        dx, word ptr [ebp]
0044da3d  shr        eax, 1
0044da3f  mov        esi, 0xf00f
0044da44  and        dx, si
0044da47  shl        cx, 4
0044da4b  or         cx, dx
0044da4e  and        al, 0xf
0044da50  mov        edx, 0xfff0
0044da55  movzx      ax, al
0044da59  and        cx, dx
0044da5c  mov        edx, dword ptr [esp + 0x14]
0044da60  or         cx, ax
0044da63  mov        word ptr [ebp], cx
0044da67  mov        esi, dword ptr [0x4b0e4c]

// block 0044da6d
0044da6d  inc        edx
0044da6e  add        ebp, 2
0044da71  mov        dword ptr [esp + 0x14], edx
0044da75  cmp        edx, esi
0044da77  jb         0x44d900

// block 0044da7d
0044da7d  mov        eax, dword ptr [esp + 0x10]
0044da81  inc        eax
0044da82  mov        dword ptr [esp + 0x10], eax
0044da86  cmp        eax, dword ptr [esp + 0x2c]
0044da8a  jb         0x44d8f0

// block 0044da90
0044da90  pop        edi
0044da91  pop        esi
0044da92  pop        ebp
0044da93  mov        al, 1
0044da95  pop        ebx
0044da96  add        esp, 0x18
0044da99  ret        4

// block 0044da9c
0044da9c  mov        ebp, dword ptr [0x4b0e68]
0044daa2  xor        ebx, ebx
0044daa4  xor        edi, edi
0044daa6  mov        dword ptr [esp + 0x14], ebp
0044daaa  mov        dword ptr [esp + 0x10], edi
0044daae  cmp        dword ptr [esp + 0x2c], ebx
0044dab2  jbe        0x44dc0c

// block 0044dab8
0044dab8  mov        eax, dword ptr [0x4b0e4c]
0044dabd  lea        ecx, [ecx]

// block 0044dac0
0044dac0  xor        edx, edx
0044dac2  mov        dword ptr [esp + 0x18], edx
0044dac6  cmp        eax, ebx
0044dac8  jbe        0x44dbfd

// block 0044dace
0044dace  lea        ecx, [ebp - 2]

// block 0044dad1
0044dad1  cmp        byte ptr [ecx + 5], 0
0044dad5  jne        0x44dbe6

// block 0044dadb
0044dadb  xor        esi, esi
0044dadd  mov        dword ptr [esp + 0x24], ebx
0044dae1  mov        dword ptr [esp + 0x20], ebx
0044dae5  test       edx, edx
0044dae7  jbe        0x44db07

// block 0044dae9
0044dae9  cmp        byte ptr [ecx + 1], 0
0044daed  je         0x44db07

// block 0044daef
0044daef  movzx      esi, byte ptr [ecx - 1]
0044daf3  movzx      ebx, byte ptr [ecx]
0044daf6  mov        dword ptr [esp + 0x20], esi
0044dafa  movzx      esi, byte ptr [ecx - 2]
0044dafe  mov        dword ptr [esp + 0x24], esi
0044db02  mov        esi, 1

// block 0044db07
0044db07  dec        eax
0044db08  cmp        edx, eax
0044db0a  jae        0x44db29

// block 0044db0c
0044db0c  cmp        byte ptr [ecx + 9], 0
0044db10  je         0x44db29

// block 0044db12
0044db12  movzx      edx, byte ptr [ecx + 8]
0044db16  movzx      eax, byte ptr [ecx + 7]
0044db1a  add        dword ptr [esp + 0x20], eax
0044db1e  add        ebx, edx
0044db20  movzx      edx, byte ptr [ecx + 6]
0044db24  add        dword ptr [esp + 0x24], edx
0044db28  inc        esi

// block 0044db29
0044db29  test       edi, edi
0044db2b  mov        edi, dword ptr [0x4b0e58]
0044db31  jbe        0x44db68

// block 0044db33
0044db33  mov        eax, edi
0044db35  cdq        
0044db36  and        edx, 3
0044db39  add        eax, edx
0044db3b  sar        eax, 2
0044db3e  add        eax, eax
0044db40  add        eax, eax
0044db42  mov        edx, eax
0044db44  mov        eax, ebp
0044db46  sub        eax, edx
0044db48  cmp        byte ptr [eax + 3], 0
0044db4c  je         0x44db68

// block 0044db4e
0044db4e  movzx      edx, byte ptr [eax + 2]
0044db52  mov        ebp, dword ptr [esp + 0x14]
0044db56  add        ebx, edx
0044db58  movzx      edx, byte ptr [eax + 1]
0044db5c  movzx      eax, byte ptr [eax]
0044db5f  add        dword ptr [esp + 0x20], edx
0044db63  add        dword ptr [esp + 0x24], eax
0044db67  inc        esi

// block 0044db68
0044db68  mov        edx, dword ptr [0x4b0e50]
0044db6e  dec        edx
0044db6f  cmp        dword ptr [esp + 0x10], edx
0044db73  jae        0x44dba1

// block 0044db75
0044db75  mov        eax, edi
0044db77  cdq        
0044db78  and        edx, 3
0044db7b  add        eax, edx
0044db7d  sar        eax, 2
0044db80  cmp        byte ptr [ebp + eax*4 + 3], 0
0044db85  lea        eax, [ebp + eax*4]
0044db89  je         0x44dba1

// block 0044db8b
0044db8b  movzx      edx, byte ptr [eax + 2]
0044db8f  add        ebx, edx
0044db91  movzx      edx, byte ptr [eax + 1]
0044db95  movzx      eax, byte ptr [eax]
0044db98  add        dword ptr [esp + 0x20], edx
0044db9c  add        dword ptr [esp + 0x24], eax
0044dba0  inc        esi

// block 0044dba1
0044dba1  cmp        esi, 1
0044dba4  jbe        0x44dbc6

// block 0044dba6
0044dba6  xor        edx, edx
0044dba8  mov        eax, ebx
0044dbaa  div        esi
0044dbac  xor        edx, edx
0044dbae  mov        ebx, eax
0044dbb0  mov        eax, dword ptr [esp + 0x20]
0044dbb4  div        esi
0044dbb6  xor        edx, edx
0044dbb8  mov        dword ptr [esp + 0x20], eax
0044dbbc  mov        eax, dword ptr [esp + 0x24]
0044dbc0  div        esi
0044dbc2  mov        dword ptr [esp + 0x24], eax

// block 0044dbc6
0044dbc6  mov        dl, byte ptr [esp + 0x20]
0044dbca  mov        al, byte ptr [esp + 0x24]
0044dbce  mov        edi, dword ptr [esp + 0x10]
0044dbd2  mov        byte ptr [ecx + 4], bl
0044dbd5  mov        byte ptr [ecx + 3], dl
0044dbd8  mov        edx, dword ptr [esp + 0x18]
0044dbdc  mov        byte ptr [ebp], al
0044dbdf  mov        eax, dword ptr [0x4b0e4c]
0044dbe4  xor        ebx, ebx

// block 0044dbe6
0044dbe6  inc        edx
0044dbe7  add        ebp, 4
0044dbea  add        ecx, 4
0044dbed  mov        dword ptr [esp + 0x14], ebp
0044dbf1  mov        dword ptr [esp + 0x18], edx
0044dbf5  cmp        edx, eax
0044dbf7  jb         0x44dad1

// block 0044dbfd
0044dbfd  inc        edi
0044dbfe  mov        dword ptr [esp + 0x10], edi
0044dc02  cmp        edi, dword ptr [esp + 0x2c]
0044dc06  jb         0x44dac0

// block 0044dc0c
0044dc0c  pop        edi
0044dc0d  pop        esi
0044dc0e  pop        ebp
0044dc0f  mov        al, 1
0044dc11  pop        ebx
0044dc12  add        esp, 0x18
0044dc15  ret        4
