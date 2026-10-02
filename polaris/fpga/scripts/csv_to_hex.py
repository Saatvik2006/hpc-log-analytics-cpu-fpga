import csv
import sys

if len(sys.argv) != 3:
    print("Usage: python3 csv_to_hex.py input.csv output.hex")
    sys.exit(1)

input_file = sys.argv[1]
output_file = sys.argv[2]

with open(input_file, newline="", encoding="utf-8") as f:
    reader = csv.reader(f)

    # Skip header
    next(reader)

    with open(output_file, "w", encoding="utf-8") as out:

        for row in reader:

            # Reconstruct the CSV record exactly enough for
            # testing the hardware CSV parser.
            #
            # csv.writer-style quoting is required because
            # LOCATION contains commas.
            import io

            buffer = io.StringIO()

            writer = csv.writer(
                buffer,
                quoting=csv.QUOTE_MINIMAL,
                lineterminator=""
            )

            writer.writerow(row)

            line = buffer.getvalue() + "\n"

            for byte in line.encode("utf-8"):
                out.write(f"{byte:02x}\n")
