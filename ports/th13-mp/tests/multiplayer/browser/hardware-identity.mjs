import assert from "node:assert/strict";

const software = /SwiftShader|llvmpipe|Software|lavapipe|softpipe|swrast|\bWARP\b|Microsoft Basic Render Driver/i;
export function assertHardwareIdentity(identity) {
  const {devices, auxAttributes, featureStatus} = identity.gpu;
  const renderer = auxAttributes?.glRenderer;
  assert(renderer && !software.test(renderer), "Actual hardware renderer");
  // EGL reports zero PCI IDs in Chromium. Its device/driver strings and the
  // actual game's unmasked context must still identify the same hardware.
  const device = devices.find(row => row.deviceString === renderer &&
    row.driverVendor && row.driverVersion && !software.test(row.deviceString));
  assert(device, "Matching GPU device and driver identity");
  assert.equal(featureStatus.webgl2, "enabled", "Hardware WebGL2 status");
  assert.equal(featureStatus.gpu_compositing, "enabled", "Hardware compositing status");
  assert.deepEqual(identity.contexts.map(row => row.seat).sort((a, b) => a - b),
    [...identity.seats].sort((a, b) => a - b), "Every assigned seat has one game context");
  assert(identity.contexts.length && new Set(identity.seats).size === identity.seats.length,
    "Unique actual game WebGL contexts");
  const driverSuffix = ` ${device.driverVendor} ${device.driverVersion})`;
  const publicRenderer = renderer.endsWith(driverSuffix)
    ? renderer.slice(0, -driverSuffix.length) + ")" : renderer;
  for (const context of identity.contexts) {
    assert.match(context.version, /^WebGL 2\.0/, "Actual game WebGL2 context");
    // Chromium's WebGL string omits the driver suffix exposed through CDP.
    // Strip exactly the independently recorded suffix, preserving the complete
    // device model, bus/backend and OpenGL version.
    assert(context.renderer === renderer || context.renderer === publicRenderer,
      "Game context matches browser GPU");
    assert.equal(context.vendor, auxAttributes.glVendor, "Game context GPU vendor");
  }
  return {device, pciIdsAvailable: device.vendorId > 0 && device.deviceId > 0};
}
