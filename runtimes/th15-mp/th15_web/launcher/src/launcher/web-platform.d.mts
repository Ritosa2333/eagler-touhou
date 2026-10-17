export {};
declare global {
  interface ScreenOrientation {
    lock(orientation: "any" | "natural" | "landscape" | "portrait" | "portrait-primary" | "portrait-secondary" | "landscape-primary" | "landscape-secondary"): Promise<void>;
  }
}
