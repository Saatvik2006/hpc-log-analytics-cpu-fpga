#include "Vpolaris_log_parser.h"
#include "verilated.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdint>
#include <iomanip>

using namespace std;

static uint64_t total_cycles = 0;

// ------------------------------------------------------------
// One complete clock cycle
// ------------------------------------------------------------
void tick(Vpolaris_log_parser* parser)
{
    parser->clk = 0;
    parser->eval();

    parser->clk = 1;
    parser->eval();

    total_cycles++;
}

// ------------------------------------------------------------
// Reset parser
// ------------------------------------------------------------
void reset_parser(Vpolaris_log_parser* parser)
{
    parser->rst = 1;
    parser->valid_in = 0;
    parser->ascii_in = 0;
    parser->ready = 0;

    tick(parser);
    tick(parser);

    parser->rst = 0;

    tick(parser);
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main(int argc, char** argv)
{
    Verilated::commandArgs(argc, argv);

    if (argc < 2) {
        cerr << "Usage: " << argv[0]
             << " <polaris_dataset.csv>" << endl;
        return 1;
    }

    const char* filename = argv[1];

    // --------------------------------------------------------
    // Open Polaris dataset
    // --------------------------------------------------------
    ifstream file(filename, ios::binary);

    if (!file.is_open()) {
        cerr << "ERROR: Cannot open file: "
             << filename << endl;
        return 1;
    }

    // --------------------------------------------------------
    // Create Verilated parser
    // --------------------------------------------------------
    Vpolaris_log_parser* parser =
        new Vpolaris_log_parser;

    parser->clk = 0;
    parser->rst = 0;
    parser->ascii_in = 0;
    parser->valid_in = 0;
    parser->ready = 0;

    reset_parser(parser);

    // --------------------------------------------------------
    // Benchmark counters
    // --------------------------------------------------------
    uint64_t bytes_processed = 0;
    uint64_t records_detected = 0;

    auto start_time =
        chrono::high_resolution_clock::now();

    // --------------------------------------------------------
    // Stream dataset byte-by-byte
    // --------------------------------------------------------
    char c;

    while (file.get(c)) {

        parser->ascii_in =
            static_cast<uint8_t>(c);

        parser->valid_in = 1;

        tick(parser);

        bytes_processed++;

        // ----------------------------------------------------
        // Parser produced a complete record
        // ----------------------------------------------------
        if (parser->valid_record) {

            records_detected++;

            // Accept the record
            parser->ready = 1;

            tick(parser);

            parser->ready = 0;

            tick(parser);
        }
    }

    // Stop input
    parser->valid_in = 0;
    parser->ascii_in = 0;

    tick(parser);

    auto end_time =
        chrono::high_resolution_clock::now();

    file.close();

    // --------------------------------------------------------
    // Calculate benchmark metrics
    // --------------------------------------------------------
    double elapsed_seconds =
        chrono::duration<double>(
            end_time - start_time
        ).count();

    double cycles_per_byte =
        (bytes_processed > 0)
            ? static_cast<double>(total_cycles) /
              static_cast<double>(bytes_processed)
            : 0.0;

    double bytes_per_second =
        (elapsed_seconds > 0)
            ? static_cast<double>(bytes_processed) /
              elapsed_seconds
            : 0.0;

    double records_per_second =
        (elapsed_seconds > 0)
            ? static_cast<double>(records_detected) /
              elapsed_seconds
            : 0.0;

    // --------------------------------------------------------
    // Print results
    // --------------------------------------------------------
    cout << endl;
    cout << "============================================" << endl;
    cout << " POLARIS FPGA PARSER VERILATOR BENCHMARK" << endl;
    cout << "============================================" << endl;

    cout << "Dataset              : "
         << filename << endl;

    cout << "Bytes processed      : "
         << bytes_processed << endl;

    cout << "Records detected     : "
         << records_detected << endl;

    cout << "FPGA cycles          : "
         << total_cycles << endl;

    cout << fixed << setprecision(6);

    cout << "Cycles / byte        : "
         << cycles_per_byte << endl;

    cout << "Verilator time       : "
         << elapsed_seconds << " s" << endl;

    cout << "Simulation throughput: "
         << bytes_per_second / (1024.0 * 1024.0)
         << " MB/s" << endl;

    cout << "Records / second     : "
         << records_per_second << endl;

    cout << "============================================" << endl;

    delete parser;

    return 0;
}
