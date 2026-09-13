import { copyFile, mkdir } from "node:fs/promises";

await mkdir(new URL("../public/vendor/", import.meta.url), { recursive: true });
await copyFile(
  new URL("../node_modules/webaudio-tinysynth/webaudio-tinysynth.min.js", import.meta.url),
  new URL("../public/vendor/webaudio-tinysynth.min.js", import.meta.url),
);
await copyFile(
  new URL("../node_modules/webaudio-tinysynth/LICENSE", import.meta.url),
  new URL("../public/vendor/webaudio-tinysynth.LICENSE", import.meta.url),
);
await copyFile(
  new URL("../node_modules/fflate/umd/index.js", import.meta.url),
  new URL("../public/vendor/fflate.min.js", import.meta.url),
);
await copyFile(
  new URL("../node_modules/fflate/LICENSE", import.meta.url),
  new URL("../public/vendor/fflate.LICENSE", import.meta.url),
);
await copyFile(
  new URL("../node_modules/marked/lib/marked.umd.js", import.meta.url),
  new URL("../public/vendor/marked.umd.js", import.meta.url),
);
await copyFile(
  new URL("../node_modules/marked/LICENSE", import.meta.url),
  new URL("../public/vendor/marked.LICENSE", import.meta.url),
);
await copyFile(
  new URL("../node_modules/dompurify/dist/purify.min.js", import.meta.url),
  new URL("../public/vendor/purify.min.js", import.meta.url),
);
await copyFile(
  new URL("../node_modules/dompurify/LICENSE", import.meta.url),
  new URL("../public/vendor/dompurify.LICENSE", import.meta.url),
);
