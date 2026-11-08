// The opencv toolkit's web part: the Dashboard `image` instrument (opencv/view).
// The Paperboat web app bundles every engine/toolkit/<name>/web/index.tsx and calls register(host).

type ImageProps = { name: string; latest: unknown; idle: boolean };
type Host = { registerInstrument: (kind: string, renderer: (props: ImageProps) => unknown) => void };

/** The latest frame, a data URL, or a greyed placeholder before the first one. */
function ImageInstrument({ name, latest, idle }: ImageProps) {
  return (
    <figure className={`instrument image${idle ? " idle" : ""}`}>
      <div className="screen">
        {typeof latest === "string" && latest ? <img src={latest} alt={name} draggable={false} /> : <output>{idle ? "–" : "no image"}</output>}
      </div>
      <figcaption>
        <span>{name}</span>
        <span>image</span>
      </figcaption>
    </figure>
  );
}

export function register(host: Host) {
  host.registerInstrument("image", ImageInstrument);
}
