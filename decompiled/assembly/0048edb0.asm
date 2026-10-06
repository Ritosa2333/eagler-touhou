
// block 0048edb0
0048edb0  push       ebp
0048edb1  mov        ebp, esp
0048edb3  push       edi
0048edb4  push       esi
0048edb5  push       ebx
0048edb6  mov        ecx, dword ptr [ebp + 0x10]
0048edb9  or         ecx, ecx
0048edbb  je         0x48ee0a

// block 0048edbd
0048edbd  mov        esi, dword ptr [ebp + 8]
0048edc0  mov        edi, dword ptr [ebp + 0xc]
0048edc3  mov        bh, 0x41
0048edc5  mov        bl, 0x5a
0048edc7  mov        dh, 0x20
0048edc9  lea        ecx, [ecx]

// block 0048edcc
0048edcc  mov        ah, byte ptr [esi]
0048edce  or         ah, ah
0048edd0  mov        al, byte ptr [edi]
0048edd2  je         0x48edfb

// block 0048edd4
0048edd4  or         al, al
0048edd6  je         0x48edfb

// block 0048edd8
0048edd8  add        esi, 1
0048eddb  add        edi, 1
0048edde  cmp        ah, bh
0048ede0  jb         0x48ede8

// block 0048ede2
0048ede2  cmp        ah, bl
0048ede4  ja         0x48ede8

// block 0048ede6
0048ede6  add        ah, dh

// block 0048ede8
0048ede8  cmp        al, bh
0048edea  jb         0x48edf2

// block 0048edec
0048edec  cmp        al, bl
0048edee  ja         0x48edf2

// block 0048edf0
0048edf0  add        al, dh

// block 0048edf2
0048edf2  cmp        ah, al
0048edf4  jne        0x48ee01

// block 0048edf6
0048edf6  sub        ecx, 1
0048edf9  jne        0x48edcc

// block 0048edfb
0048edfb  xor        ecx, ecx
0048edfd  cmp        ah, al
0048edff  je         0x48ee0a

// block 0048ee01
0048ee01  mov        ecx, 0xffffffff
0048ee06  jb         0x48ee0a

// block 0048ee08
0048ee08  neg        ecx

// block 0048ee0a
0048ee0a  mov        eax, ecx
0048ee0c  pop        ebx
0048ee0d  pop        esi
0048ee0e  pop        edi
0048ee0f  leave      
0048ee10  ret        
