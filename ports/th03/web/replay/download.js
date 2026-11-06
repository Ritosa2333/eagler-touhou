// Export a completed Replay directly, without adding it to game storage.
export function downloadReplay(blob, name, environment = globalThis) {
  if (!(blob instanceof Blob) || blob.size === 0 || !/^th03-[a-zA-Z0-9_-]+\.th03r$/.test(name)) {
    throw new Error('Replay 文件无效');
  }
  const url = environment.URL.createObjectURL(blob);
  try {
    const link = environment.document.createElement('a');
    link.href = url;
    link.download = name;
    link.click();
  } finally {
    environment.setTimeout(() => environment.URL.revokeObjectURL(url), 30000);
  }
}
