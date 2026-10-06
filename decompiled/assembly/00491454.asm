
// block 00491454
00491454  mov        edi, edi
00491456  push       ebp
00491457  mov        ebp, esp
00491459  mov        ecx, dword ptr [ebp + 8]
0049145c  sub        esp, 0xc
0049145f  test       ecx, ecx
00491461  ja         0x49146e

// block 00491463
00491463  xor        ecx, ecx

// block 00491465
00491465  push       ecx
00491466  call       0x46c9ea

// block 0049146b
0049146b  pop        ecx
0049146c  leave      
0049146d  ret        

// block 0049146e
0049146e  or         eax, 0xffffffff
00491471  xor        edx, edx
00491473  div        ecx
00491475  cmp        eax, 1
00491478  jae        0x491465

// block 0049147a
0049147a  push       0
0049147c  lea        ecx, [ebp - 0xc]
0049147f  call       0x44d280

// block 00491484
00491484  push       0x4ab5d0
00491489  lea        eax, [ebp - 0xc]
0049148c  push       eax
0049148d  call       0x46ff8f

// block 00491492
00491492  int3       
