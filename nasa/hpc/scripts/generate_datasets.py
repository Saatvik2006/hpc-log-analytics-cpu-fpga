import os
import sys

if len(sys.argv) != 3:
    print("Usage:")
    print("python3 generate_datasets.py <source_log> <output_dir>")
    sys.exit(1)

SOURCE = sys.argv[1]
OUTDIR = sys.argv[2]

sizes = [
    ("500MB", 500 * 1024**2),
    ("1GB",   1 * 1024**3),
    ("2GB",   2 * 1024**3),
    ("4GB",   4 * 1024**3),
    ("8GB",   8 * 1024**3),
    ("10GB", 10 * 1024**3),
]

os.makedirs(OUTDIR, exist_ok=True)

source_size = os.path.getsize(SOURCE)

for label, target_size in sizes:

    output_file = os.path.join(OUTDIR, f"nasa_{label}.log")

    print(f"\nCreating {output_file}")

    written = 0

    with open(output_file, "wb") as out:
        while written < target_size:
            with open(SOURCE, "rb") as src:
                while True:
                    chunk = src.read(16 * 1024 * 1024)

                    if not chunk:
                        break

                    if written + len(chunk) > target_size:
                        chunk = chunk[:target_size-written]

                    out.write(chunk)
                    written += len(chunk)

                    if written >= target_size:
                        break

    print(f"Done -> {label}")
