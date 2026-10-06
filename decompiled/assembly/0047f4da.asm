
// block 0047f4da
0047f4da  mov        edi, edi
0047f4dc  push       ebp
0047f4dd  mov        ebp, esp
0047f4df  sub        esp, 0x10
0047f4e2  push       dword ptr [ebp + 0xc]
0047f4e5  lea        ecx, [ebp - 8]
0047f4e8  call       0x47e59a

// block 0047f4ed
0047f4ed  mov        eax, dword ptr [0x4b4318]
0047f4f2  mov        al, byte ptr [eax]
0047f4f4  inc        dword ptr [0x4b4318]
0047f4fa  cmp        al, 0x40
0047f4fc  jne        0x47f56f

// block 0047f4fe
0047f4fe  mov        eax, dword ptr [0x4b4318]
0047f503  mov        al, byte ptr [eax]
0047f505  inc        dword ptr [0x4b4318]
0047f50b  cmp        al, 0x5f
0047f50d  jne        0x47f56f

// block 0047f50f
0047f50f  inc        dword ptr [0x4b4318]
0047f515  lea        eax, [ebp - 0x10]
0047f518  push       0
0047f51a  push       eax
0047f51b  call       0x47ecb6

// block 0047f520
0047f520  lea        eax, [ebp - 0x10]
0047f523  push       0
0047f525  push       eax
0047f526  call       0x47ecb6

// block 0047f52b
0047f52b  mov        eax, dword ptr [0x4b4318]
0047f530  mov        cl, byte ptr [eax]
0047f532  add        esp, 0x10
0047f535  test       cl, cl
0047f537  je         0x47f54f

// block 0047f539
0047f539  cmp        cl, 0x40
0047f53c  je         0x47f54a

// block 0047f53e
0047f53e  inc        eax
0047f53f  mov        dword ptr [0x4b4318], eax
0047f544  mov        cl, byte ptr [eax]
0047f546  test       cl, cl
0047f548  jne        0x47f539

// block 0047f54a
0047f54a  cmp        byte ptr [eax], 0
0047f54d  jne        0x47f559

// block 0047f54f
0047f54f  dec        eax
0047f550  mov        dword ptr [0x4b4318], eax
0047f555  push       1
0047f557  jmp        0x47f571

// block 0047f559
0047f559  mov        ecx, dword ptr [ebp - 8]
0047f55c  inc        eax
0047f55d  mov        dword ptr [0x4b4318], eax
0047f562  mov        eax, dword ptr [ebp + 8]
0047f565  mov        dword ptr [eax], ecx
0047f567  mov        ecx, dword ptr [ebp - 4]
0047f56a  mov        dword ptr [eax + 4], ecx
0047f56d  leave      
0047f56e  ret        

// block 0047f56f
0047f56f  push       2

// block 0047f571
0047f571  mov        ecx, dword ptr [ebp + 8]
0047f574  call       0x47e1ce

// block 0047f579
0047f579  mov        eax, dword ptr [ebp + 8]
0047f57c  leave      
0047f57d  ret        
