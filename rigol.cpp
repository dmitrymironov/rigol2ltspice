// Build:
//   g++ -std=c++11 rigol.cpp -o rigol
//
// Rigol CSV -> LTspice two-column text converter.
// Original tool by Novorado (c), 2015, Dmitry Mironov.
//
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>

using namespace std;

// Parses a floating-point value, including scientific notation.
// This intentionally mirrors the lightweight parsing behavior of the
// original tool and accepts inputs like "1e-3", "-2.5E+1", etc.
double me(const string& s)
{
    istringstream str(s);
    double v = 0.0;
    str >> scientific >> v;
    return v;
}

int main(int argc, char* argv[])
{
    // Time shift applied to every sample (seconds).
    // When not explicitly provided and first timestamp is negative,
    // this is auto-populated so first output time starts at 0.
    double deltaTime = 0.0;

    if (argc < 3) {
        cerr << "Usage: " << argv[0]
             << " <Rigol CSV file> <LTSpice CSV file> [time shift in MS]"
             << endl;
        return 1;
    }

    if (argc == 4) {
        // User provides shift in milliseconds; internal units are seconds.
        deltaTime = me(argv[3]) / 1000.0;
        cout << "Shifting for " << deltaTime << "s in time domain" << endl;
    }

    string inputPath(argv[1]), outputPath(argv[2]);
    cout << "Reading '" << inputPath << "', writing '" << outputPath << "'" << endl;

    ifstream input(inputPath);
    ofstream output(outputPath);
    if (!input.is_open()) {
        cerr << "Error: cannot open input file: " << inputPath << endl;
        return 2;
    }
    if (!output.is_open()) {
        cerr << "Error: cannot open output file: " << outputPath << endl;
        return 3;
    }

    // Simple comma-splitter: this handles plain numeric CSV.
    // It is not a full CSV parser (no quoted field support).
    const regex fieldRegex("[^,]+");

    string line;
    int skipLine = 2;    // Rigol CSV commonly has two header lines.
    bool firstHit = true;

    // Read until getline fails; avoids processing stale content at EOF.
    while (getline(input, line)) {
        if (skipLine > 0) {
            --skipLine;
            continue;
        }

        double vals[2] = {0.0, 0.0};
        int cnt = 0;
        for (sregex_iterator it(line.begin(), line.end(), fieldRegex), end;
             it != end && cnt < 2; ++it, ++cnt) {
            vals[cnt] = me((*it).str());
        }

        // Ignore malformed/short rows that do not provide two fields.
        if (cnt < 2) {
            continue;
        }

        if (firstHit) {
            firstHit = false;

            // Auto-shift only when user did not explicitly set a shift and
            // the first timestamp starts below zero.
            if (deltaTime == 0.0 && vals[0] < 0.0) {
                deltaTime = -vals[0];
            }
        }

        output << fixed << (vals[0] + deltaTime) << " " << vals[1] << endl;
    }

    if (!input.eof() && input.fail()) {
        cerr << "Warning: read terminated before EOF due to input error." << endl;
    }

    if (!output) {
        cerr << "Warning: write error occurred while saving output." << endl;
        return 4;
    }

    cout << "Done!" << endl;
    return 0;
}
