/*
 * Shipping delivery identity for the owned sound-journal window.
 *
 * The serial is issued by the module wrapper (this JS closure property), not by linear memory, so a
 * whole-HEAP capsule restore cannot rewind it. 0x7fffffff is the last issued serial; wrapping to 0
 * makes the core seal the journal instead of aliasing an earlier serial.
 */
mergeInto(LibraryManager.library, {
  th128_host_capsule_next_serial: function () {
    Module.__th128SoundSerial = ((Module.__th128SoundSerial ?? 0) + 1) & 0x7fffffff;
    return Module.__th128SoundSerial;
  },
});
