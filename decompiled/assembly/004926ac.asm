
// block 004926ac
004926ac  push       0x2c
004926ae  push       0x4ab4f0
004926b3  call       0x46fcec

// block 004926b8
004926b8  mov        ebx, ecx
004926ba  mov        edi, dword ptr [ebp + 0xc]
004926bd  mov        esi, dword ptr [ebp + 8]
004926c0  mov        dword ptr [ebp - 0x1c], ebx
004926c3  and        dword ptr [ebp - 0x34], 0
004926c7  mov        eax, dword ptr [edi - 4]
004926ca  mov        dword ptr [ebp - 0x24], eax
004926cd  push       dword ptr [esi + 0x18]
004926d0  lea        eax, [ebp - 0x3c]
004926d3  push       eax
004926d4  call       0x491d54

// block 004926d9
004926d9  pop        ecx
004926da  pop        ecx
004926db  mov        dword ptr [ebp - 0x28], eax
004926de  call       0x475467

// block 004926e3
004926e3  mov        eax, dword ptr [eax + 0x88]
004926e9  mov        dword ptr [ebp - 0x2c], eax
004926ec  call       0x475467

// block 004926f1
004926f1  mov        eax, dword ptr [eax + 0x8c]
004926f7  mov        dword ptr [ebp - 0x30], eax
004926fa  call       0x475467

// block 004926ff
004926ff  mov        dword ptr [eax + 0x88], esi
00492705  call       0x475467

// block 0049270a
0049270a  mov        ecx, dword ptr [ebp + 0x10]
0049270d  mov        dword ptr [eax + 0x8c], ecx
00492713  and        dword ptr [ebp - 4], 0
00492717  xor        eax, eax
00492719  inc        eax
0049271a  mov        dword ptr [ebp + 0x10], eax
0049271d  mov        dword ptr [ebp - 4], eax
00492720  push       dword ptr [ebp + 0x1c]
00492723  push       dword ptr [ebp + 0x18]
00492726  push       ebx
00492727  push       dword ptr [ebp + 0x14]
0049272a  push       edi
0049272b  call       0x491df9

// block 00492730
00492730  add        esp, 0x14
00492733  mov        dword ptr [ebp - 0x1c], eax
00492736  and        dword ptr [ebp - 4], 0
0049273a  jmp        0x4927ab

// block 004927ab
004927ab  mov        dword ptr [ebp - 4], 0xfffffffe
004927b2  mov        dword ptr [ebp + 0x10], 0
004927b9  call       0x4927d2

// block 004927be
004927be  mov        eax, dword ptr [ebp - 0x1c]
004927c1  call       0x46fd31

// block 004927c6
004927c6  ret        
