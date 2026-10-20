
// block 004778ff
004778ff  push       0x10
00477901  push       0x4aadf8
00477906  call       0x46fcec

// block 0047790b
0047790b  xor        eax, eax
0047790d  mov        ebx, dword ptr [ebp + 8]
00477910  xor        edi, edi
00477912  cmp        ebx, edi
00477914  setne      al
00477917  cmp        eax, edi
00477919  jne        0x477938

// block 0047791b
0047791b  call       0x46e96f

// block 00477920
00477920  mov        dword ptr [eax], 0x16
00477926  push       edi
00477927  push       edi
00477928  push       edi
00477929  push       edi
0047792a  push       edi
0047792b  call       0x470f2d

// block 00477930
00477930  add        esp, 0x14
00477933  or         eax, 0xffffffff
00477936  jmp        0x47798b

// block 00477938
00477938  cmp        dword ptr [0x4d6458], 3
0047793f  jne        0x477979

// block 00477941
00477941  push       4
00477943  call       0x46ec9a

// block 00477948
00477948  pop        ecx
00477949  mov        dword ptr [ebp - 4], edi
0047794c  push       ebx
0047794d  call       0x46edc8

// block 00477952
00477952  pop        ecx
00477953  mov        dword ptr [ebp - 0x20], eax
00477956  cmp        eax, edi
00477958  je         0x477965

// block 0047795a
0047795a  mov        esi, dword ptr [ebx - 4]
0047795d  sub        esi, 9
00477960  mov        dword ptr [ebp - 0x1c], esi
00477963  jmp        0x477968

// block 00477965
00477965  mov        esi, dword ptr [ebp - 0x1c]

// block 00477968
00477968  mov        dword ptr [ebp - 4], 0xfffffffe
0047796f  call       0x477999

// block 00477974
00477974  cmp        dword ptr [ebp - 0x20], edi
00477977  jne        0x477989

// block 00477979
00477979  push       ebx
0047797a  push       edi
0047797b  push       dword ptr [0x4b3c04]
00477981  call       dword ptr [0x49812c]

// block 00477987
00477987  mov        esi, eax

// block 00477989
00477989  mov        eax, esi

// block 0047798b
0047798b  call       0x46fd31

// block 00477990
00477990  ret        
