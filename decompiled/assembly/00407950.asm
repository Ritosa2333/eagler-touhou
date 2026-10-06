
// block 00407950
00407950  mov        eax, dword ptr [esi + 0x40]
00407953  mov        edx, dword ptr [0x4ce8cc]
00407959  push       edi
0040795a  push       eax
0040795b  call       0x461920

// block 00407960
00407960  mov        edi, eax
00407962  test       edi, edi
00407964  jne        0x407969

// block 00407966
00407966  mov        dword ptr [esi + 0x40], eax

// block 00407969
00407969  push       0x28
0040796b  call       0x406f60

// block 00407970
00407970  test       edi, edi
00407972  jne        0x407979

// block 00407974
00407974  or         eax, 0xffffffff
00407977  pop        edi
00407978  ret        

// block 00407979
00407979  cmp        dword ptr [esi + 0x18], 0xa0
00407980  jl         0x4079a6

// block 00407982
00407982  mov        ecx, dword ptr [esi + 0x40]
00407985  push       ebx
00407986  push       ecx
00407987  mov        ebx, 1
0040798c  call       0x461970

// block 00407991
00407991  mov        edx, dword ptr [esi + 0x48]
00407994  push       edx
00407995  call       0x461970

// block 0040799a
0040799a  pop        ebx
0040799b  mov        dword ptr [esi + 0x40], 0
004079a2  xor        eax, eax
004079a4  pop        edi
004079a5  ret        

// block 004079a6
004079a6  mov        ecx, esi
004079a8  call       0x407a30

// block 004079ad
004079ad  fld        dword ptr [edi + 0x44]
004079b0  fstp       dword ptr [esi + 0x514]
004079b6  xor        eax, eax
004079b8  pop        edi
004079b9  ret        
