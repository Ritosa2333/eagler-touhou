declare module '@ffmpeg/core' {
  export default function createFFmpegCore(options: {wasmBinary: Uint8Array}): Promise<{
    FS: {
      writeFile(name: string, bytes: Uint8Array): void;
      readFile(name: string): Uint8Array<ArrayBuffer>;
      unlink(name: string): void;
    };
    exec(...args: string[]): number;
    reset(): void;
  }>;
}
