# rigol2ltspice

Small command-line converter for transforming Rigol oscilloscope CSV waveform exports into a simple two-column text format that LTspice can import.

## What It Does

- Accepts input as:
  - `rigol <rigol.csv> <out.txt> [time_shift_ms]`
- Reads the input CSV.
- Skips the first two lines (Rigol header/metadata area).
- Uses only the first two comma-separated fields from each remaining row:
  - column 1: time
  - column 2: signal value
- Parses both fields as floating-point values (scientific notation is supported).
- Writes output as space-separated `time value`, one sample per line.

## Time Shift Behavior

- If `time_shift_ms` is provided:
  - The value is interpreted as milliseconds and converted to seconds.
  - That shift is applied to every output timestamp.
- If no shift is provided and the first parsed timestamp is negative:
  - The tool auto-shifts the whole waveform so the first output time becomes `0`.

## Build

Use a C++ compiler with C++11 support:

```bash
g++ -std=c++11 rigol.cpp -o rigol
```

## Usage

```bash
./rigol input.csv output.txt
./rigol input.csv output.txt 2.5
```

Examples:

- `./rigol scope.csv wave.txt`
- `./rigol scope.csv wave.txt 1.25`

In the second example, `1.25` means `1.25 ms` shift, which is applied as `0.00125 s`.

## Input Assumptions and Limits

- Intended for plain numeric CSV rows.
- The splitter is regex-based and intentionally simple.
- Quoted CSV fields and embedded commas are not supported.
- Rows with fewer than two parseable fields are ignored.

## PlantUML: Converter Usage Flow

The repository includes a PlantUML diagram at:

- `docs/rigol_usage.puml`

You can render it with PlantUML, for example:

```bash
plantuml docs/rigol_usage.puml
```

## License

See `LICENSE`.
