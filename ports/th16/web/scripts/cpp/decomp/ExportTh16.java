// 导出 th16.exe（v1.00a）的分析证据：全部函数的反编译候选、反汇编、调用图、导入表与字符串。
// 反编译文本是候选，不是已验证实现；任何子系统在 C++ 落地前都需 oracle 对照。
// @category TH16
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.address.*;
import ghidra.program.model.data.*;
import ghidra.program.model.mem.*;
import com.google.gson.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class ExportTh16 extends GhidraScript {
    public void run() throws Exception {
        Path output = Paths.get(getScriptArgs()[0]);
        Files.createDirectories(output.resolve("functions"));
        Set<String> selected = new HashSet<>();
        for (int i = 1; i < getScriptArgs().length; i++) {
            var address = toAddr(getScriptArgs()[i]);
            if (getFunctionAt(address) == null) {
                disassemble(address);
                createFunction(address, null);
            }
            selected.add(address.toString());
        }
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        DecompInterface decompiler = new DecompInterface();
        DecompileOptions options = new DecompileOptions();
        options.grabFromProgram(currentProgram);
        decompiler.setOptions(options);
        decompiler.toggleCCode(true);
        decompiler.toggleSyntaxTree(true);
        decompiler.openProgram(currentProgram);
        JsonArray functions = new JsonArray();
        int count = 0, failed = 0;
        try {
            FunctionIterator iterator = currentProgram.getFunctionManager().getFunctions(true);
            while (iterator.hasNext() && !monitor.isCancelled()) {
                Function function = iterator.next();
                if (function.isExternal()) continue;
                String address = function.getEntryPoint().toString();
                if (!selected.isEmpty() && !selected.contains(address)) continue;
                JsonObject entry = new JsonObject();
                entry.addProperty("address", "0x" + address);
                entry.addProperty("name", function.getName(true));
                entry.addProperty("signature", function.getPrototypeString(true, true));
                entry.addProperty("callingConvention", function.getCallingConventionName());
                entry.addProperty("bodyBytes", function.getBody().getNumAddresses());
                entry.addProperty("thunk", function.isThunk());
                JsonArray calls = new JsonArray();
                for (Function called : function.getCalledFunctions(monitor)) {
                    JsonObject call = new JsonObject();
                    call.addProperty("address", "0x" + called.getEntryPoint().toString());
                    call.addProperty("name", called.getName(true));
                    calls.add(call);
                }
                entry.add("calls", calls);
                JsonArray callers = new JsonArray();
                for (Function caller : function.getCallingFunctions(monitor)) callers.add("0x" + caller.getEntryPoint().toString());
                entry.add("callers", callers);
                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);
                entry.addProperty("decompiled", result.decompileCompleted());
                entry.addProperty("message", result.getErrorMessage());
                if (result.decompileCompleted()) {
                    String code = "// Analysis candidate: " + address + " " + function.getName(true) + "\n" + result.getDecompiledFunction().getC();
                    Files.writeString(output.resolve("functions").resolve(address + ".c"), code, StandardCharsets.UTF_8);
                } else failed++;
                StringBuilder assembly = new StringBuilder();
                InstructionIterator instructions = currentProgram.getListing().getInstructions(function.getBody(), true);
                while (instructions.hasNext()) {
                    Instruction ins = instructions.next();
                    assembly.append(ins.getAddress()).append(" ").append(ins.toString()).append("\n");
                }
                Files.writeString(output.resolve("functions").resolve(address + ".asm"), assembly.toString(), StandardCharsets.UTF_8);
                functions.add(entry);
                if (++count % 100 == 0) System.out.println("TH16 functions exported: " + count + ", failures: " + failed);
            }
        } finally { decompiler.dispose(); }

        // 导入表：Win32/D3D 边界，决定 oracle 桩与 SDL3/GLES3 宿主的替换面。
        JsonArray imports = new JsonArray();
        SymbolIterator symbols = currentProgram.getSymbolTable().getExternalSymbols();
        while (symbols.hasNext()) {
            Symbol symbol = symbols.next();
            JsonObject item = new JsonObject();
            item.addProperty("name", symbol.getName());
            item.addProperty("library", symbol.getParentNamespace().getName());
            JsonArray refs = new JsonArray();
            for (Reference ref : symbol.getReferences()) refs.add("0x" + ref.getFromAddress().toString());
            item.add("references", refs);
            imports.add(item);
        }
        Files.writeString(output.resolve("imports.json"), gson.toJson(imports), StandardCharsets.UTF_8);

        // 字符串：资源文件名、格式串、版本号，用于锚定装载器与存档/录像路径。
        JsonArray strings = new JsonArray();
        DataIterator data = currentProgram.getListing().getDefinedData(true);
        while (data.hasNext()) {
            Data item = data.next();
            if (!(item.getDataType() instanceof StringDataType) && !(item.getDataType() instanceof TerminatedStringDataType) && !(item.getDataType() instanceof UnicodeDataType)) continue;
            Object value = item.getValue();
            if (value == null) continue;
            JsonObject s = new JsonObject();
            s.addProperty("address", "0x" + item.getAddress().toString());
            s.addProperty("value", value.toString());
            JsonArray refs = new JsonArray();
            for (Reference ref : getReferencesTo(item.getAddress())) refs.add("0x" + ref.getFromAddress().toString());
            s.add("references", refs);
            strings.add(s);
        }
        Files.writeString(output.resolve("strings.json"), gson.toJson(strings), StandardCharsets.UTF_8);

        // 内存映射：段落边界供 .data 全局变量定位与 Unicorn oracle 装载。
        JsonArray blocks = new JsonArray();
        for (MemoryBlock block : currentProgram.getMemory().getBlocks()) {
            JsonObject b = new JsonObject();
            b.addProperty("name", block.getName());
            b.addProperty("start", "0x" + block.getStart().toString());
            b.addProperty("end", "0x" + block.getEnd().toString());
            b.addProperty("size", block.getSize());
            b.addProperty("execute", block.isExecute());
            b.addProperty("write", block.isWrite());
            blocks.add(b);
        }
        Files.writeString(output.resolve("blocks.json"), gson.toJson(blocks), StandardCharsets.UTF_8);

        JsonObject report = new JsonObject();
        report.addProperty("program", currentProgram.getName());
        report.addProperty("sha256", currentProgram.getExecutableSHA256());
        report.addProperty("language", currentProgram.getLanguageID().toString());
        report.addProperty("imageBase", "0x" + currentProgram.getImageBase().toString());
        report.addProperty("functionCount", count);
        report.addProperty("failedCount", failed);
        report.add("functions", functions);
        Files.writeString(output.resolve(selected.isEmpty() ? "functions.json" : "selected-functions.json"), gson.toJson(report), StandardCharsets.UTF_8);
        System.out.println("TH16 export complete: " + count + " functions, " + failed + " decompiler failures");
    }
}
