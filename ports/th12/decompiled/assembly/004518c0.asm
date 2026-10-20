
// block 004518c0
004518c0  mov        eax, dword ptr [esp + 8]
004518c4  sub        eax, 0x10
004518c7  push       ebx
004518c8  mov        ebx, dword ptr [0x49825c]
004518ce  push       esi
004518cf  mov        esi, dword ptr [esp + 0xc]
004518d3  push       edi
004518d4  je         0x451989

// block 004518da
004518da  sub        eax, 0x100
004518df  je         0x4519b3

// block 004518e5
004518e5  sub        eax, 1
004518e8  jne        0x451a48

// block 004518ee
004518ee  mov        eax, 0xd0
004518f3  cmp        word ptr [esp + 0x18], ax
004518f8  jne        0x451a48

// block 004518fe
004518fe  mov        edi, dword ptr [0x49826c]
00451904  push       0xca
00451909  push       esi
0045190a  call       edi

// block 0045190c
0045190c  cmp        eax, 1
0045190f  jne        0x45191d

// block 00451911
00451911  or         dword ptr [0x4ceae8], 0x100
0045191b  jmp        0x451927

// block 0045191d
0045191d  and        dword ptr [0x4ceae8], 0xfffffeff

// block 00451927
00451927  push       0xcc
0045192c  push       esi
0045192d  call       edi

// block 0045192f
0045192f  cmp        eax, 1
00451932  jne        0x45193d

// block 00451934
00451934  mov        byte ptr [0x4ceacd], 0
0045193b  jmp        0x45197d

// block 0045193d
0045193d  push       0xcd
00451942  push       esi
00451943  call       edi

// block 00451945
00451945  cmp        eax, 1
00451948  jne        0x451951

// block 0045194a
0045194a  mov        byte ptr [0x4ceacd], al
0045194f  jmp        0x45197d

// block 00451951
00451951  push       0xce
00451956  push       esi
00451957  call       edi

// block 00451959
00451959  cmp        eax, 1
0045195c  jne        0x451967

// block 0045195e
0045195e  mov        byte ptr [0x4ceacd], 2
00451965  jmp        0x45197d

// block 00451967
00451967  push       0xcf
0045196c  push       esi
0045196d  call       edi

// block 0045196f
0045196f  dec        eax
00451970  neg        eax
00451972  sbb        al, al
00451974  and        al, 0xfd
00451976  add        al, 3
00451978  mov        byte ptr [0x4ceacd], al

// block 0045197d
0045197d  and        dword ptr [0x4cf428], 0xffffff9f
00451984  push       6
00451986  push       esi
00451987  call       ebx

// block 00451989
00451989  mov        eax, dword ptr [0x4cf428]
0045198e  mov        edx, eax
00451990  and        dl, 0x60
00451993  cmp        dl, 0x40
00451996  jne        0x4519a3

// block 00451998
00451998  and        eax, 0xffffffbf
0045199b  or         eax, 0x20
0045199e  mov        dword ptr [0x4cf428], eax

// block 004519a3
004519a3  push       6
004519a5  push       esi
004519a6  call       ebx

// block 004519a8
004519a8  pop        edi
004519a9  pop        esi
004519aa  mov        eax, 1
004519af  pop        ebx
004519b0  ret        0x10

// block 004519b3
004519b3  test       dword ptr [0x4ceae8], 0x100
004519bd  mov        edi, dword ptr [0x498238]
004519c3  mov        ebx, dword ptr [0x498258]
004519c9  je         0x4519df

// block 004519cb
004519cb  push       0
004519cd  push       1
004519cf  push       0xf1
004519d4  push       0xca
004519d9  push       esi
004519da  call       ebx

// block 004519dc
004519dc  push       eax
004519dd  call       edi

// block 004519df
004519df  movzx      eax, byte ptr [0x4ceacd]
004519e6  cmp        eax, 3
004519e9  ja         0x451a36

// block 004519eb
004519eb  jmp        dword ptr [eax*4 + 0x451a50]

// block 004519f2
004519f2  push       0
004519f4  push       1
004519f6  push       0xf1
004519fb  push       0xcc
00451a00  jmp        0x451a30

// block 00451a02
00451a02  push       0
00451a04  push       1
00451a06  push       0xf1
00451a0b  push       0xcd
00451a10  jmp        0x451a30

// block 00451a12
00451a12  push       0
00451a14  push       1
00451a16  push       0xf1
00451a1b  push       0xce
00451a20  jmp        0x451a30

// block 00451a22
00451a22  push       0
00451a24  push       1
00451a26  push       0xf1
00451a2b  push       0xcf

// block 00451a30
00451a30  push       esi
00451a31  call       ebx

// block 00451a33
00451a33  push       eax
00451a34  call       edi

// block 00451a36
00451a36  mov        ecx, dword ptr [0x4cf428]
00451a3c  and        ecx, 0xffffffdf
00451a3f  or         ecx, 0x40
00451a42  mov        dword ptr [0x4cf428], ecx

// block 00451a48
00451a48  pop        edi
00451a49  pop        esi
00451a4a  xor        eax, eax
00451a4c  pop        ebx
00451a4d  ret        0x10
