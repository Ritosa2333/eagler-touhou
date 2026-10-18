import { loadCompiledContract } from "./load-compiled-contract.mjs";

const contract = await loadCompiledContract("runtime-protocol");
export const {
  RUNTIME_PROTOCOL_COMMANDS,
  RUNTIME_PROTOCOL_EVENTS,
  RUNTIME_PROTOCOL_OPTIONAL_COMMANDS,
  RUNTIME_PROTOCOL_OPTIONAL_EVENTS,
  isRuntimeResponseMessage,
  parseRuntimeInboundMessage,
} = contract;
