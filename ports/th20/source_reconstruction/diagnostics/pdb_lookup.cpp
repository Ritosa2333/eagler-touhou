// Resolve source-build crash RVAs with the installed Visual Studio DIA reader.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dia2.h>
#include <iostream>
#include <string>
int wmain(int argc,wchar_t** argv){
    if(argc<3){std::cerr<<"Usage: th20_pdb_lookup BUILD.pdb RVA...\n";return 2;}
    CoInitialize(nullptr);
    const auto module=LoadLibraryW(TH20_DIA_DLL);
    if(!module){std::cerr<<"DIA reader unavailable: "<<GetLastError()<<'\n';return 2;}
    auto factory_function=reinterpret_cast<HRESULT(WINAPI*)(REFCLSID,REFIID,void**)>(GetProcAddress(module,"DllGetClassObject"));
    IClassFactory* factory=nullptr;IDiaDataSource* source=nullptr;IDiaSession* session=nullptr;
    HRESULT hr=factory_function(__uuidof(DiaSource),IID_IClassFactory,reinterpret_cast<void**>(&factory));
    if(SUCCEEDED(hr))hr=factory->CreateInstance(nullptr,__uuidof(IDiaDataSource),reinterpret_cast<void**>(&source));
    if(SUCCEEDED(hr))hr=source->loadDataFromPdb(argv[1]);
    if(SUCCEEDED(hr))hr=source->openSession(&session);
    if(FAILED(hr)){std::cerr<<"DIA initialization failed: 0x"<<std::hex<<hr<<'\n';return 2;}
    for(int i=2;i<argc;++i){
        if(argv[i][0]==L'@'){
            IDiaSymbol* root=nullptr;IDiaEnumSymbols* matches=nullptr;
            if(SUCCEEDED(session->get_globalScope(&root))&&root){
                if(SUCCEEDED(root->findChildren(SymTagData,argv[i]+1,nsRegularExpression,&matches))&&matches){
                    IDiaSymbol* value=nullptr;ULONG fetched=0;
                    while(SUCCEEDED(matches->Next(1,&value,&fetched))&&fetched){
                        DWORD rva=0;BSTR name=nullptr;value->get_name(&name);
                        if(SUCCEEDED(value->get_relativeVirtualAddress(&rva))&&name)std::wcout<<L"DATA 0x"<<std::hex<<rva<<L" "<<name<<L'\n';
                        if(name)SysFreeString(name);value->Release();
                    }matches->Release();
                }root->Release();
            }continue;
        }
        const auto rva=static_cast<DWORD>(std::stoul(argv[i],nullptr,16));
        std::wcout<<L"RVA 0x"<<std::hex<<rva;IDiaSymbol* symbol=nullptr;
        if(SUCCEEDED(session->findSymbolByRVA(rva,SymTagFunction,&symbol))&&symbol){
            BSTR name=nullptr;symbol->get_name(&name);DWORD start=0;symbol->get_relativeVirtualAddress(&start);
            if(name){std::wcout<<L" "<<name<<L"+0x"<<(rva-start);SysFreeString(name);}symbol->Release();
        }
        std::wcout<<L'\n';IDiaEnumLineNumbers* lines=nullptr;
        if(SUCCEEDED(session->findLinesByRVA(rva,1,&lines))&&lines){
            IDiaLineNumber* line=nullptr;ULONG fetched=0;
            while(SUCCEEDED(lines->Next(1,&line,&fetched))&&fetched){
                DWORD number=0;line->get_lineNumber(&number);IDiaSourceFile* file=nullptr;
                if(SUCCEEDED(line->get_sourceFile(&file))&&file){BSTR name=nullptr;file->get_fileName(&name);if(name){std::wcout<<L"  "<<name<<L":"<<std::dec<<number<<L'\n';SysFreeString(name);}file->Release();}line->Release();
            }lines->Release();
        }
    }
    session->Release();source->Release();factory->Release();FreeLibrary(module);CoUninitialize();
}
