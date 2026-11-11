export type LauncherMusicMode = "ogg-stream" | "ogg-full" | "ogg-akyu" | "midi" | "none";

export interface MusicModeResolutionInput {
  requested?: string;
  explicit?: boolean;
  audio?: boolean;
  midi?: boolean;
  localOgg?: boolean;
  remoteOgg?: boolean;
  preferOgg?: boolean;
  musicSources?: readonly string[];
}

export function resolveMusicMode({
  requested = "ogg-stream",
  explicit = false,
  audio = true,
  midi = true,
  localOgg = false,
  remoteOgg = false,
  preferOgg = true,
  musicSources = [],
}: MusicModeResolutionInput = {}): LauncherMusicMode {
  if (requested === "ogg-akyu" && !musicSources.includes("akyu")) requested = "ogg-stream";
  if (!audio || requested === "none") return "none";
  const ogg = localOgg || remoteOgg;
  if (explicit && requested === "midi" && midi) return "midi";
  if ((requested === "ogg-stream" || requested === "ogg-full" || requested === "ogg-akyu") && ogg) return requested;
  if (!explicit && ogg && preferOgg) return "ogg-stream";
  if (midi) return "midi";
  return ogg ? "ogg-stream" : "none";
}

export interface InstalledMusicAvailability {
  revision?: unknown;
  oggFileIds: readonly string[];
  files: Readonly<Record<string, { objectId?: unknown } | undefined>>;
}

export interface MusicAvailabilityInput {
  musicSources?: readonly string[];
  audio: boolean;
  /** Soundtrack ships inside the managed DATA object. */
  embeddedOgg?: boolean;
  midiAvailable: boolean;
  importServer: boolean;
  // Compatibility/diagnostic signal from the Site Manifest. Once an installed
  // generation exists, remote progressive OGG authority comes from that
  // descriptor plus the matching Release Catalog revision instead.
  publishedOggCapable?: boolean;
  remoteOggAdvertised: boolean;
  remoteRevision?: unknown;
  installed?: InstalledMusicAvailability | null;
}

export interface MusicAvailability {
  audio: boolean;
  midi: boolean;
  localOgg: boolean;
  remoteOgg: boolean;
  ogg: boolean;
}

export function resolveMusicAvailability({
  audio,
  embeddedOgg = false,
  midiAvailable,
  importServer,
  remoteOggAdvertised,
  remoteRevision = null,
  installed = null,
}: MusicAvailabilityInput): MusicAvailability {
  let localOgg = embeddedOgg;
  let remoteOgg = false;

  if (installed) {
    const ids = installed.oggFileIds;
    // Launcher startup gates OGG on up to the first two files;
    // the rest of the component is installed progressively after gameplay can
    // already begin.  Availability must use the same startup barrier instead
    // of requiring the whole optional component, otherwise closing a Runtime
    // while later tracks are still downloading makes the settings UI silently
    // fall back to MIDI/none even though the next OGG launch is already viable.
    const startupIds = ids.slice(0, 2);
    localOgg = embeddedOgg || startupIds.length > 0 && startupIds.every(fileId => !!installed.files[fileId]?.objectId);
    // Once the installed generation already declares OGG files, matching the
    // Release Catalog revision is enough to prove that the same remote OGG
    // objects remain available for progressive completion. The Site Manifest
    // must not have to duplicate that capability or it can disappear from the
    // UI when the Runtime closes and only the Package snapshot remains.
    remoteOgg = !importServer && ids.length > 0 && remoteRevision === installed.revision;
  } else if (!importServer) {
    remoteOgg = remoteOggAdvertised;
  }

  return {
    audio,
    midi: audio && midiAvailable,
    localOgg: audio && localOgg,
    remoteOgg: audio && remoteOgg,
    ogg: audio && (localOgg || remoteOgg),
  };
}

export interface EffectiveMusicModeInput extends MusicAvailabilityInput {
  requested: string;
  explicit: boolean;
}

export function resolveEffectiveMusicMode({
  musicSources,
  requested,
  explicit,
  audio,
  embeddedOgg = false,
  midiAvailable,
  importServer,
  remoteOggAdvertised,
  remoteRevision = null,
  installed = null,
}: EffectiveMusicModeInput): LauncherMusicMode {
  const availability = resolveMusicAvailability({
    audio,
    embeddedOgg,
    midiAvailable,
    importServer,
    remoteOggAdvertised,
    remoteRevision,
    installed,
  });

  return resolveMusicMode({
    musicSources,
    requested,
    explicit,
    audio: availability.audio,
    midi: availability.midi,
    localOgg: availability.localOgg,
    remoteOgg: availability.remoteOgg,
    preferOgg: availability.localOgg || (!installed && !importServer),
  });
}
