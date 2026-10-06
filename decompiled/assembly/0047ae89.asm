
// block 0047ae89
0047ae89  mov        edi, edi
0047ae8b  push       ebp
0047ae8c  mov        ebp, esp
0047ae8e  sub        esp, 0xc
0047ae91  push       ebx
0047ae92  xor        ebx, ebx
0047ae94  push       esi
0047ae95  push       edi
0047ae96  cmp        dword ptr [0x4d6434], ebx
0047ae9c  jne        0x47aea3

// block 0047ae9e
0047ae9e  call       0x473e82

// block 0047aea3
0047aea3  push       0x104
0047aea8  mov        esi, 0x4b41e0
0047aead  push       esi
0047aeae  push       ebx
0047aeaf  mov        byte ptr [0x4b42e4], bl
0047aeb5  call       dword ptr [0x4980b4]

// block 0047aebb
0047aebb  mov        eax, dword ptr [0x4d645c]
0047aec0  mov        dword ptr [0x4b3d90], esi
0047aec6  cmp        eax, ebx
0047aec8  je         0x47aed1

// block 0047aeca
0047aeca  mov        dword ptr [ebp - 4], eax
0047aecd  cmp        byte ptr [eax], bl
0047aecf  jne        0x47aed4

// block 0047aed1
0047aed1  mov        dword ptr [ebp - 4], esi

// block 0047aed4
0047aed4  mov        edx, dword ptr [ebp - 4]
0047aed7  lea        eax, [ebp - 8]
0047aeda  push       eax
0047aedb  push       ebx
0047aedc  push       ebx
0047aedd  lea        edi, [ebp - 0xc]
0047aee0  call       0x47acef

// block 0047aee5
0047aee5  mov        eax, dword ptr [ebp - 8]
0047aee8  add        esp, 0xc
0047aeeb  cmp        eax, 0x3fffffff
0047aef0  jae        0x47af3c

// block 0047aef2
0047aef2  mov        ecx, dword ptr [ebp - 0xc]
0047aef5  cmp        ecx, -1
0047aef8  jae        0x47af3c

// block 0047aefa
0047aefa  mov        edi, eax
0047aefc  shl        edi, 2
0047aeff  lea        eax, [edi + ecx]
0047af02  cmp        eax, ecx
0047af04  jb         0x47af3c

// block 0047af06
0047af06  push       eax
0047af07  call       0x4777ce

// block 0047af0c
0047af0c  mov        esi, eax
0047af0e  pop        ecx
0047af0f  cmp        esi, ebx
0047af11  je         0x47af3c

// block 0047af13
0047af13  mov        edx, dword ptr [ebp - 4]
0047af16  lea        eax, [ebp - 8]
0047af19  push       eax
0047af1a  add        edi, esi
0047af1c  push       edi
0047af1d  push       esi
0047af1e  lea        edi, [ebp - 0xc]
0047af21  call       0x47acef

// block 0047af26
0047af26  mov        eax, dword ptr [ebp - 8]
0047af29  add        esp, 0xc
0047af2c  dec        eax
0047af2d  mov        dword ptr [0x4b3d74], eax
0047af32  mov        dword ptr [0x4b3d78], esi
0047af38  xor        eax, eax
0047af3a  jmp        0x47af3f

// block 0047af3c
0047af3c  or         eax, 0xffffffff

// block 0047af3f
0047af3f  pop        edi
0047af40  pop        esi
0047af41  pop        ebx
0047af42  leave      
0047af43  ret        
