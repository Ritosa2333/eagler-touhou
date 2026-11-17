import {test} from "node:test";
import assert from "node:assert/strict";
import {assertHardwareIdentity} from "./hardware-identity.mjs";

const renderer = "ANGLE (NVIDIA Corporation, NVIDIA GeForce RTX 5090 Laptop GPU/PCIe/SSE2, OpenGL ES 3.2 NVIDIA 620.70.06)";
function fixture() {
  return {seats: [0], gpu: {devices: [{vendorId: 0, deviceId: 0, deviceString: renderer,
    driverVendor: "NVIDIA", driverVersion: "620.70.06"}],
    auxAttributes: {glRenderer: renderer, glVendor: "Google Inc. (NVIDIA Corporation)"},
    featureStatus: {webgl2: "enabled", gpu_compositing: "enabled"}},
    contexts: [{seat: 0, version: "WebGL 2.0 (OpenGL ES 3.0 Chromium)", renderer,
      vendor: "Google Inc. (NVIDIA Corporation)"}]};
}
test("EGL zero PCI IDs require matching real context and driver", () => {
  assert.equal(assertHardwareIdentity(fixture()).pciIdsAvailable, false);
});
test("WebGL sanitizes exactly the CDP-recorded driver suffix", () => {
  const identity = fixture();
  identity.contexts[0].renderer = renderer.replace(" NVIDIA 620.70.06)", ")");
  assertHardwareIdentity(identity);
  identity.gpu.devices[0].driverVersion = "620.70.07";
  assert.throws(() => assertHardwareIdentity(identity));
});
for (const [name, mutate] of [
  ["software browser renderer", x => x.gpu.auxAttributes.glRenderer = "SwiftShader"],
  ["software game context", x => x.contexts[0].renderer = "llvmpipe"],
  ["different game GPU", x => x.contexts[0].renderer = "ANGLE (other GPU)"],
  ["missing driver", x => x.gpu.devices[0].driverVersion = ""],
  ["missing context", x => x.contexts = []],
  ["wrong seat context", x => x.contexts[0].seat = 1],
  ["duplicate seat context", x => x.contexts.push({...x.contexts[0]})],
  ["WebGL1 context", x => x.contexts[0].version = "WebGL 1.0"],
  ["software compositing", x => x.gpu.featureStatus.gpu_compositing = "disabled_software"],
]) test("reject " + name, () => {
  const identity = fixture(); mutate(identity);
  assert.throws(() => assertHardwareIdentity(identity));
});
for (const name of ["softpipe", "swrast", "WARP", "Microsoft Basic Render Driver",
    "SwiftShader", "llvmpipe", "lavapipe"]) test("reject consistent software " + name, () => {
  const identity = fixture();
  identity.gpu.auxAttributes.glRenderer = name;
  identity.gpu.devices[0].deviceString = name;
  identity.contexts[0].renderer = name;
  assert.throws(() => assertHardwareIdentity(identity));
});
