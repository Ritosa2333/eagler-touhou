
// block 00493708
00493708  push       edx
00493709  wait       
0049370a  fnstcw     word ptr [esp]
0049370d  je         0x493763

// block 0049370f
0049370f  cmp        word ptr [esp], 0x27f
00493715  je         0x49371d

// block 00493717
00493717  fldcw      word ptr [0x4a5dd8]

// block 0049371d
0049371d  fptan      
0049371f  wait       
00493720  fnstsw     ax
00493722  sahf       
00493723  jp         0x493744

// block 00493725
00493725  fstp       st(0)

// block 00493727
00493727  cmp        dword ptr [0x4b4110], 0
0049372e  jne        0x4956fe

// block 00493734
00493734  mov        edx, 0x20
00493739  lea        ecx, [0x4b3580]
0049373f  jmp        0x49570b

// block 00493744
00493744  fld        xword ptr [0x4a5dda]
0049374a  fxch       st(1)

// block 0049374c
0049374c  fprem1     
0049374e  wait       
0049374f  fnstsw     ax
00493751  sahf       
00493752  jp         0x49374c

// block 00493754
00493754  fstp       st(1)
00493756  fptan      
00493758  fstp       st(0)
0049375a  jmp        0x493727

// block 0049375c
0049375c  call       0x49568c

// block 00493761
00493761  jmp        0x49377e

// block 00493763
00493763  test       eax, 0xfffff
00493768  jne        0x49375c

// block 0049376a
0049376a  cmp        dword ptr [esp + 8], 0
0049376f  jne        0x49375c

// block 00493771
00493771  fstp       st(0)
00493773  fld        xword ptr [0x4b35c0]
00493779  mov        eax, 1

// block 0049377e
0049377e  cmp        dword ptr [0x4b4110], 0
00493785  jne        0x4956fe

// block 0049378b
0049378b  mov        edx, 0x20
00493790  lea        ecx, [0x4b3580]
00493796  call       0x495617

// block 0049379b
0049379b  pop        edx
0049379c  ret        
