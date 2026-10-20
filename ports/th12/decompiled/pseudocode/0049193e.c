// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49193e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct EXCEPTION_RECORD {
    int ExceptionCode;
    unsigned int ExceptionFlags;
    struct EXCEPTION_RECORD *ExceptionRecord;
    void* ExceptionAddress;
    unsigned int NumberParameters;
    UIntPtr ExceptionInformation[15];
} EXCEPTION_RECORD;

// attributes: PLT stub
Void RtlUnwind(void* TargetFrame, void* TargetIp, struct EXCEPTION_RECORD *ExceptionRecord, void* ReturnValue)
{
    void* v0;  // [bp+0x0]

    ::kernel32.dll::RtlUnwind(v0, TargetFrame, TargetIp, ExceptionRecord);
    return;
}
