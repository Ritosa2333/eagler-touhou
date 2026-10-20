
// block 00485797
00485797  mov        edi, edi
00485799  push       ebp
0048579a  mov        ebp, esp
0048579c  sub        esp, 0x20
0048579f  mov        eax, dword ptr [ebp + 8]
004857a2  push       ebx
004857a3  push       dword ptr [ebp + 0x1c]
004857a6  xor        ebx, ebx
004857a8  lea        ecx, [ebp - 0x20]
004857ab  mov        dword ptr [ebp - 8], ebx
004857ae  mov        dword ptr [ebp - 0x10], eax
004857b1  call       0x46d3b4

// block 004857b6
004857b6  mov        eax, dword ptr [ebp + 8]
004857b9  cmp        eax, ebx
004857bb  jne        0x4857e8

// block 004857bd
004857bd  call       0x46e96f

// block 004857c2
004857c2  push       ebx
004857c3  push       ebx
004857c4  push       ebx
004857c5  push       ebx
004857c6  push       ebx
004857c7  mov        dword ptr [eax], 0x16
004857cd  call       0x470f2d

// block 004857d2
004857d2  add        esp, 0x14
004857d5  cmp        byte ptr [ebp - 0x14], bl
004857d8  je         0x4857e1

// block 004857da
004857da  mov        eax, dword ptr [ebp - 0x18]
004857dd  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004857e1
004857e1  xor        eax, eax
004857e3  jmp        0x485938

// block 004857e8
004857e8  push       edi
004857e9  mov        edi, dword ptr [ebp + 0xc]
004857ec  cmp        edi, ebx
004857ee  jne        0x48581b

// block 004857f0
004857f0  call       0x46e96f

// block 004857f5
004857f5  push       ebx
004857f6  push       ebx
004857f7  push       ebx
004857f8  push       ebx
004857f9  push       ebx
004857fa  mov        dword ptr [eax], 0x16
00485800  call       0x470f2d

// block 00485805
00485805  add        esp, 0x14
00485808  cmp        byte ptr [ebp - 0x14], bl
0048580b  je         0x485814

// block 0048580d
0048580d  mov        eax, dword ptr [ebp - 0x18]
00485810  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00485814
00485814  xor        eax, eax
00485816  jmp        0x485937

// block 0048581b
0048581b  push       esi
0048581c  mov        esi, dword ptr [ebp + 0x10]
0048581f  mov        byte ptr [eax], bl
00485821  cmp        esi, ebx
00485823  je         0x485910

// block 00485829
00485829  mov        eax, dword ptr [ebp + 0x18]
0048582c  cmp        eax, ebx
0048582e  jne        0x485839

// block 00485830
00485830  mov        eax, dword ptr [ebp - 0x20]
00485833  mov        eax, dword ptr [eax + 0xd4]

// block 00485839
00485839  mov        dword ptr [ebp - 0xc], eax
0048583c  mov        dword ptr [ebp - 4], edi
0048583f  cmp        edi, ebx
00485841  jbe        0x4858e6

// block 00485847
00485847  mov        al, byte ptr [esi]
00485849  cmp        al, bl
0048584b  je         0x4858c6

// block 0048584d
0048584d  cmp        al, 0x25
0048584f  je         0x485892

// block 00485851
00485851  lea        ecx, [ebp - 0x20]
00485854  movsx      eax, al
00485857  push       ecx
00485858  push       eax
00485859  call       0x47c5a3

// block 0048585e
0048585e  pop        ecx
0048585f  pop        ecx
00485860  test       eax, eax
00485862  je         0x485882

// block 00485864
00485864  xor        ecx, ecx
00485866  inc        ecx
00485867  cmp        dword ptr [ebp - 4], ecx
0048586a  jbe        0x485882

// block 0048586c
0048586c  lea        eax, [esi + 1]
0048586f  cmp        byte ptr [eax], bl
00485871  je         0x4858e3

// block 00485873
00485873  mov        cl, byte ptr [esi]
00485875  mov        edx, dword ptr [ebp + 8]
00485878  mov        byte ptr [edx], cl
0048587a  inc        dword ptr [ebp + 8]
0048587d  dec        dword ptr [ebp - 4]
00485880  mov        esi, eax

// block 00485882
00485882  mov        al, byte ptr [esi]
00485884  mov        ecx, dword ptr [ebp + 8]
00485887  mov        byte ptr [ecx], al
00485889  inc        dword ptr [ebp + 8]
0048588c  inc        esi
0048588d  dec        dword ptr [ebp - 4]
00485890  jmp        0x4858c1

// block 00485892
00485892  mov        edx, dword ptr [ebp + 0x14]
00485895  cmp        edx, ebx
00485897  je         0x485910

// block 00485899
00485899  inc        esi
0048589a  xor        eax, eax
0048589c  cmp        byte ptr [esi], 0x23
0048589f  jne        0x4858a3

// block 004858a1
004858a1  inc        eax
004858a2  inc        esi

// block 004858a3
004858a3  push       eax
004858a4  push       dword ptr [ebp - 0xc]
004858a7  lea        eax, [ebp - 4]
004858aa  push       eax
004858ab  lea        eax, [ebp - 0x20]
004858ae  push       eax
004858af  mov        al, byte ptr [esi]
004858b1  lea        ecx, [ebp + 8]
004858b4  call       0x484f4e

// block 004858b9
004858b9  add        esp, 0x10
004858bc  test       eax, eax
004858be  je         0x485902

// block 004858c0
004858c0  inc        esi

// block 004858c1
004858c1  cmp        dword ptr [ebp - 4], ebx
004858c4  ja         0x485847

// block 004858c6
004858c6  cmp        dword ptr [ebp - 4], ebx
004858c9  jbe        0x4858e6

// block 004858cb
004858cb  mov        eax, dword ptr [ebp + 8]
004858ce  mov        byte ptr [eax], bl
004858d0  sub        edi, dword ptr [ebp - 4]
004858d3  mov        eax, edi
004858d5  cmp        byte ptr [ebp - 0x14], bl
004858d8  je         0x485936

// block 004858da
004858da  mov        ecx, dword ptr [ebp - 0x18]
004858dd  and        dword ptr [ecx + 0x70], 0xfffffffd
004858e1  jmp        0x485936

// block 004858e3
004858e3  mov        dword ptr [ebp - 8], ecx

// block 004858e6
004858e6  mov        eax, dword ptr [ebp - 0x10]
004858e9  mov        byte ptr [eax], bl
004858eb  cmp        dword ptr [ebp - 8], ebx
004858ee  jne        0x485910

// block 004858f0
004858f0  cmp        dword ptr [ebp - 4], ebx
004858f3  ja         0x485910

// block 004858f5
004858f5  call       0x46e96f

// block 004858fa
004858fa  mov        dword ptr [eax], 0x22
00485900  jmp        0x485928

// block 00485902
00485902  cmp        dword ptr [ebp - 4], ebx
00485905  jbe        0x4858e6

// block 00485907
00485907  mov        dword ptr [ebp - 8], 1
0048590e  jmp        0x4858e6

// block 00485910
00485910  call       0x46e96f

// block 00485915
00485915  push       ebx
00485916  push       ebx
00485917  push       ebx
00485918  push       ebx
00485919  push       ebx
0048591a  mov        dword ptr [eax], 0x16
00485920  call       0x470f2d

// block 00485925
00485925  add        esp, 0x14

// block 00485928
00485928  cmp        byte ptr [ebp - 0x14], bl
0048592b  je         0x485934

// block 0048592d
0048592d  mov        eax, dword ptr [ebp - 0x18]
00485930  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00485934
00485934  xor        eax, eax

// block 00485936
00485936  pop        esi

// block 00485937
00485937  pop        edi

// block 00485938
00485938  pop        ebx
00485939  leave      
0048593a  ret        
