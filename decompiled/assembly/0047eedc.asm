
// block 0047eedc
0047eedc  mov        edi, edi
0047eede  push       ebp
0047eedf  mov        ebp, esp
0047eee1  mov        eax, dword ptr [0x4b4318]
0047eee6  movsx      eax, byte ptr [eax]
0047eee9  sub        esp, 0x10
0047eeec  sub        eax, 0x58
0047eeef  je         0x47efa0

// block 0047eef5
0047eef5  dec        eax
0047eef6  dec        eax
0047eef7  je         0x47ef7f

// block 0047eefd
0047eefd  lea        eax, [ebp - 8]
0047ef00  push       eax
0047ef01  call       0x47eaac

// block 0047ef06
0047ef06  pop        ecx
0047ef07  mov        ecx, dword ptr [ebp - 4]
0047ef0a  test       cl, cl
0047ef0c  jne        0x47ef72

// block 0047ef0e
0047ef0e  mov        eax, dword ptr [0x4b4318]
0047ef13  mov        al, byte ptr [eax]
0047ef15  test       al, al
0047ef17  je         0x47ef72

// block 0047ef19
0047ef19  cmp        al, 0x40
0047ef1b  je         0x47ef6c

// block 0047ef1d
0047ef1d  cmp        al, 0x5a
0047ef1f  je         0x47ef30

// block 0047ef21
0047ef21  mov        ecx, dword ptr [ebp + 8]
0047ef24  push       2
0047ef26  call       0x47e1ce

// block 0047ef2b
0047ef2b  jmp        0x47efb3

// block 0047ef30
0047ef30  mov        eax, dword ptr [0x4b4328]
0047ef35  inc        dword ptr [0x4b4318]
0047ef3b  shr        eax, 0x12
0047ef3e  not        eax
0047ef40  test       al, 1
0047ef42  mov        eax, 0x49de7c
0047ef47  jne        0x47ef4e

// block 0047ef49
0047ef49  mov        eax, 0x49de70

// block 0047ef4e
0047ef4e  push       eax
0047ef4f  lea        eax, [ebp - 0x10]
0047ef52  push       eax
0047ef53  lea        ecx, [ebp - 8]
0047ef56  call       0x47ec92

// block 0047ef5b
0047ef5b  mov        edx, dword ptr [eax]
0047ef5d  mov        ecx, dword ptr [ebp + 8]
0047ef60  mov        dword ptr [ecx], edx
0047ef62  mov        eax, dword ptr [eax + 4]
0047ef65  mov        dword ptr [ecx + 4], eax
0047ef68  mov        eax, ecx
0047ef6a  leave      
0047ef6b  ret        

// block 0047ef6c
0047ef6c  inc        dword ptr [0x4b4318]

// block 0047ef72
0047ef72  mov        eax, dword ptr [ebp + 8]
0047ef75  mov        edx, dword ptr [ebp - 8]
0047ef78  mov        dword ptr [eax], edx
0047ef7a  mov        dword ptr [eax + 4], ecx
0047ef7d  leave      
0047ef7e  ret        

// block 0047ef7f
0047ef7f  mov        eax, dword ptr [0x4b4328]
0047ef84  inc        dword ptr [0x4b4318]
0047ef8a  shr        eax, 0x12
0047ef8d  not        eax
0047ef8f  test       al, 1
0047ef91  mov        eax, 0x49d3b4
0047ef96  jne        0x47ef9d

// block 0047ef98
0047ef98  mov        eax, 0x49de64

// block 0047ef9d
0047ef9d  push       eax
0047ef9e  jmp        0x47efab

// block 0047efa0
0047efa0  inc        dword ptr [0x4b4318]
0047efa6  push       0x49de5c

// block 0047efab
0047efab  mov        ecx, dword ptr [ebp + 8]
0047efae  call       0x47e59a

// block 0047efb3
0047efb3  mov        eax, dword ptr [ebp + 8]
0047efb6  leave      
0047efb7  ret        
