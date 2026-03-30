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

## Build and Run (Docker + Make)

This project now builds inside a Docker container with `build-essential`.

### Available Make targets

- `make help` - show available targets (default).
- `make image` - build local build image from `Dockerfile.build`.
- `make build` - compile `rigol.cpp` into `build/rigol`.
- `make test` - run 3 fixture-based tests using provided `test/` data.
- `make doc` - render the PlantUML usage diagram into `build/docs/` as text and PNG.
- `make clean` - remove build artifacts.

### Typical workflow

```bash
make build
./build/rigol test/NewFile0.csv build/out.txt
make test
```

## Test Coverage (3 provided fixtures)

`make test` runs these checks:

1. `test/NewFile0.csv` auto-shift conversion:
   - output line count is `16384`
   - first output sample is `0.000000 1.100000`
2. `test/NewFile0.csv` with explicit `1 ms` shift:
   - first output sample is `-0.006272 1.100000`
3. `test/rigol-1.csv` (already-converted two-column text, not CSV):
   - output line count is `0` with current strict CSV parsing behavior

## Input Assumptions and Limits

- Intended for plain numeric CSV rows.
- The splitter is regex-based and intentionally simple.
- Quoted CSV fields and embedded commas are not supported.
- Rows with fewer than two parseable fields are ignored.

## PlantUML: Converter Usage Flow

Source diagram:

- `docs/rigol_usage.puml`

Rendered output via Make:

```bash
make doc
```

This generates:

- `build/docs/rigol_usage.atxt`
- `build/docs/rigol_usage.png`

## License

See `LICENSE`.
