
// block 0048ac9c
0048ac9c  mov        edi, edi
0048ac9e  push       ebp
0048ac9f  mov        ebp, esp
0048aca1  sub        esp, 0x18
0048aca4  fld        qword ptr [0x49f320]
0048acaa  fstp       qword ptr [ebp - 0x10]
0048acad  fld        qword ptr [0x49f318]
0048acb3  fstp       qword ptr [ebp - 0x18]
0048acb6  fld        qword ptr [ebp - 0x18]
0048acb9  fdiv       qword ptr [ebp - 0x10]
0048acbc  fmul       qword ptr [ebp - 0x10]
0048acbf  fsubr      qword ptr [ebp - 0x18]
0048acc2  fstp       qword ptr [ebp - 8]
0048acc5  fld1       
0048acc7  fcomp      qword ptr [ebp - 8]
0048acca  fnstsw     ax
0048accc  test       ah, 5
0048accf  jp         0x48acd6

// block 0048acd1
0048acd1  xor        eax, eax
0048acd3  inc        eax
0048acd4  leave      
0048acd5  ret        

// block 0048acd6
0048acd6  xor        eax, eax
0048acd8  leave      
0048acd9  ret        

// block 0048acda
0048acda  push       0x49f344
0048acdf  call       dword ptr [0x49819c]

// block 0048ace5
0048ace5  test       eax, eax
0048ace7  je         0x48acfe

// block 0048ace9
0048ace9  push       0x49f328
0048acee  push       eax
0048acef  call       dword ptr [0x498170]

// block 0048acf5
0048acf5  test       eax, eax
0048acf7  je         0x48acfe

// block 0048acf9
0048acf9  push       0
0048acfb  call       eax

// block 0048acfd
0048acfd  ret        

// block 0048acfe
0048acfe  jmp        0x48ac9c
