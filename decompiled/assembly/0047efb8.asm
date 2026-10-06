
// block 0047efb8
0047efb8  mov        edi, edi
0047efba  push       ebp
0047efbb  mov        ebp, esp
0047efbd  mov        eax, dword ptr [0x4b4318]
0047efc2  mov        al, byte ptr [eax]
0047efc4  sub        esp, 0x10
0047efc7  test       al, al
0047efc9  je         0x47f00c

// block 0047efcb
0047efcb  cmp        al, 0x5a
0047efcd  jne        0x47efea

// block 0047efcf
0047efcf  inc        dword ptr [0x4b4318]
0047efd5  mov        edx, dword ptr [ebp - 4]
0047efd8  mov        eax, dword ptr [ebp + 8]
0047efdb  xor        ecx, ecx
0047efdd  and        edx, 0xffff0000
0047efe3  mov        dword ptr [eax], ecx
0047efe5  mov        dword ptr [eax + 4], edx
0047efe8  leave      
0047efe9  ret        

// block 0047efea
0047efea  push       0x29
0047efec  push       dword ptr [ebp + 8]
0047efef  lea        eax, [ebp - 8]
0047eff2  push       eax
0047eff3  call       0x47eedc

// block 0047eff8
0047eff8  push       eax
0047eff9  lea        eax, [ebp - 0x10]
0047effc  push       0x49de84
0047f001  push       eax
0047f002  call       0x47ec4a

// block 0047f007
0047f007  add        esp, 0x10
0047f00a  jmp        0x47f02b

// block 0047f00c
0047f00c  push       0x29
0047f00e  push       dword ptr [ebp + 8]
0047f011  lea        eax, [ebp - 0x10]
0047f014  push       1
0047f016  push       eax
0047f017  push       0x49de84
0047f01c  lea        ecx, [ebp - 8]
0047f01f  call       0x47e59a

// block 0047f024
0047f024  mov        ecx, eax
0047f026  call       0x47e79b

// block 0047f02b
0047f02b  mov        ecx, eax
0047f02d  call       0x47ec6e

// block 0047f032
0047f032  mov        eax, dword ptr [ebp + 8]
0047f035  leave      
0047f036  ret        
