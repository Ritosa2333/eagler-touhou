// Export auditable analysis and C-like pseudocode. This does not recover compilable C++.
// @category Reconstruction
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.*;
import ghidra.program.model.data.StringDataInstance;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;

public class ExportEvidence extends GhidraScript {
    private PrintWriter writer(Path path) throws IOException {
        return new PrintWriter(Files.newBufferedWriter(path, StandardCharsets.UTF_8));
    }
    private String csv(Object value) {
        String text=String.valueOf(value);
        return "\""+text.replace("\"","\"\"")+"\"";
    }
    private String va(Address address) { return "0x"+address.toString(); }
    @Override
    public void run() throws Exception {
        String[] args=getScriptArgs();
        if(args.length!=1) throw new IllegalArgumentException("Output directory required");
        Path out=Paths.get(args[0]); Files.createDirectories(out);
        Path perFunction=out.resolve("pseudocode"); Files.createDirectories(perFunction);
        FunctionManager manager=currentProgram.getFunctionManager();
        DecompInterface decompiler=new DecompInterface();
        decompiler.toggleCCode(true); decompiler.toggleSyntaxTree(false);
        if(!decompiler.openProgram(currentProgram)) throw new IOException(decompiler.getLastMessage());
        int total=0,ok=0,failed=0,external=0;
        long started=System.currentTimeMillis();
        try(PrintWriter functions=writer(out.resolve("functions.csv"));
            PrintWriter references=writer(out.resolve("function_xrefs.csv"));
            PrintWriter ranges=writer(out.resolve("function_ranges.csv"));
            PrintWriter combined=writer(out.resolve("th20_pseudocode.c"));
            PrintWriter failures=writer(out.resolve("decompile_failures.csv"))) {
            functions.println("entry_va,name,body_bytes,range_count,signature,external,thunk,decompiled,pseudocode_file");
            references.println("from_va,to_va,reference_type,source_function,target_function");
            ranges.println("function_entry,range_start,range_end_inclusive");
            failures.println("entry_va,name,error");
            combined.println("/* Ghidra-generated C-like pseudocode from the supplied th20.exe.\n * NOT compilable C++, NOT original source, NOT evidence of 1:1 gameplay equivalence.\n * Executable SHA256: "+currentProgram.getExecutableSHA256()+"\n */\n");
            FunctionIterator iterator=manager.getFunctions(true);
            while(iterator.hasNext() && !monitor.isCancelled()) {
                Function function=iterator.next(); total++;
                Address entry=function.getEntryPoint(); String entryText=va(entry);
                boolean success=false; String file="";
                if(function.isExternal()) external++;
                else {
                    DecompileResults result=null;
                    String error="";
                    try {
                        result=decompiler.decompileFunction(function,45,monitor);
                        success=result.decompileCompleted() && result.getDecompiledFunction()!=null;
                        error=result.getErrorMessage();
                    } catch(Exception e) { error=e.toString(); }
                    if(success) {
                        ok++; file="pseudocode/"+entry.toString()+".c";
                        String code="/* Entry: "+entryText+"; symbol: "+function.getName()+"; body bytes: "+function.getBody().getNumAddresses()+" */\n"+result.getDecompiledFunction().getC();
                        Files.writeString(out.resolve(file),code,StandardCharsets.UTF_8);
                        combined.println(code);
                    } else {
                        failed++;
                        Files.deleteIfExists(perFunction.resolve(entry.toString()+".c"));
                        failures.println(csv(entryText)+","+csv(function.getName())+","+csv(error));
                        combined.println("/* Failed to decompile "+entryText+" "+function.getName()+" */");
                    }
                }
                functions.println(csv(entryText)+","+csv(function.getName())+","+function.getBody().getNumAddresses()+","+function.getBody().getNumAddressRanges()+","+csv(function.getPrototypeString(true,true))+","+function.isExternal()+","+function.isThunk()+","+success+","+csv(file));
                AddressRangeIterator bodyRanges=function.getBody().getAddressRanges();
                while(bodyRanges.hasNext()) {
                    AddressRange range=bodyRanges.next();
                    ranges.println(csv(entryText)+","+csv(va(range.getMinAddress()))+","+csv(va(range.getMaxAddress())));
                }
                ReferenceIterator refs=currentProgram.getReferenceManager().getReferencesTo(entry);
                while(refs.hasNext()) {
                    Reference ref=refs.next();
                    Function caller=manager.getFunctionContaining(ref.getFromAddress());
                    references.println(csv(va(ref.getFromAddress()))+","+csv(entryText)+","+csv(ref.getReferenceType())+","+csv(caller==null?"":va(caller.getEntryPoint()))+","+csv(entryText));
                }
                if(total%100==0) {
                    println("EXPORTED "+total+" functions; "+ok+" pseudocode; "+failed+" failures");
                    functions.flush();failures.flush();combined.flush();references.flush();
                }
            }
        } finally { decompiler.dispose(); }
        int stringCount=0;
        try(PrintWriter strings=writer(out.resolve("strings.csv"));
            PrintWriter xrefs=writer(out.resolve("string_xrefs.csv"))) {
            strings.println("va,data_type,length,text");
            xrefs.println("from_va,to_va,function_entry,reference_type,text");
            DataIterator iterator=currentProgram.getListing().getDefinedData(true);
            while(iterator.hasNext()) {
                Data data=iterator.next();
                if(!data.hasStringValue()) continue;
                StringDataInstance instance=StringDataInstance.getStringDataInstance(data);
                String value=instance.getStringValue();stringCount++;
                strings.println(csv(va(data.getAddress()))+","+csv(data.getDataType().getName())+","+data.getLength()+","+csv(value));
                ReferenceIterator refs=currentProgram.getReferenceManager().getReferencesTo(data.getAddress());
                while(refs.hasNext()) {
                    Reference ref=refs.next();Function f=manager.getFunctionContaining(ref.getFromAddress());
                    xrefs.println(csv(va(ref.getFromAddress()))+","+csv(va(data.getAddress()))+","+csv(f==null?"":va(f.getEntryPoint()))+","+csv(ref.getReferenceType())+","+csv(value));
                }
            }
        }
        try(PrintWriter summary=writer(out.resolve("summary.properties"))) {
            summary.println("sha256="+currentProgram.getExecutableSHA256());
            summary.println("language="+currentProgram.getLanguageID());
            summary.println("compiler="+currentProgram.getCompilerSpec().getCompilerSpecID());
            summary.println("function_count="+total);
            summary.println("external_function_count="+external);
            summary.println("pseudocode_success_count="+ok);
            summary.println("pseudocode_failure_count="+failed);
            summary.println("defined_string_count="+stringCount);
            summary.println("export_elapsed_ms="+(System.currentTimeMillis()-started));
            summary.println("cancelled="+monitor.isCancelled());
            summary.println("scope=All functions discovered by Ghidra automatic analysis; unknown/missed functions may remain.");
            summary.println("status=Analysis evidence and C-like pseudocode; not compilable C++ or gameplay parity.");
        }
        println("EXPORT COMPLETE: total="+total+", success="+ok+", failure="+failed+", strings="+stringCount);
    }
}
