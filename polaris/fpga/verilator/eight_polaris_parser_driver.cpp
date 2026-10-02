#include "Veight_polaris_log_parser.h"
#include "verilated.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <iomanip>

using namespace std;

static uint64_t total_cycles = 0;

// ============================================================
// Shared clock
// ============================================================

void tick(Veight_polaris_log_parser* parser)
{
    parser->clk = 0;
    parser->eval();

    parser->clk = 1;
    parser->eval();

    total_cycles++;
}

// ============================================================
// Reset
// ============================================================

void reset_parser(Veight_polaris_log_parser* parser)
{
    parser->rst = 1;

    parser->ascii0_in = 0;
    parser->ascii1_in = 0;
    parser->ascii2_in = 0;
    parser->ascii3_in = 0;
    parser->ascii4_in = 0;
    parser->ascii5_in = 0;
    parser->ascii6_in = 0;
    parser->ascii7_in = 0;

    parser->valid0_in = 0;
    parser->valid1_in = 0;
    parser->valid2_in = 0;
    parser->valid3_in = 0;
    parser->valid4_in = 0;
    parser->valid5_in = 0;
    parser->valid6_in = 0;
    parser->valid7_in = 0;

    parser->ready0 = 0;
    parser->ready1 = 0;
    parser->ready2 = 0;
    parser->ready3 = 0;
    parser->ready4 = 0;
    parser->ready5 = 0;
    parser->ready6 = 0;
    parser->ready7 = 0;

    tick(parser);
    tick(parser);

    parser->rst = 0;

    tick(parser);
}

// ============================================================
// Main
// ============================================================

int main(int argc, char** argv)
{
    Verilated::commandArgs(argc, argv);

    if (argc < 2) {
        cerr << "Usage: "
             << argv[0]
             << " <polaris_dataset.csv>"
             << endl;
        return 1;
    }

    const char* filename = argv[1];

    // ========================================================
    // Load dataset
    // ========================================================

    ifstream file(filename, ios::binary);

    if (!file.is_open()) {
        cerr << "ERROR: Cannot open file: "
             << filename << endl;
        return 1;
    }

    vector<uint8_t> data(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );

    file.close();

    const size_t total_bytes = data.size();

    if (total_bytes == 0) {
        cerr << "ERROR: Dataset is empty."
             << endl;
        return 1;
    }

    // ========================================================
    // Find header/data boundary
    // ========================================================

    size_t header_end = 0;

    while (header_end < total_bytes &&
           data[header_end] != '\n') {
        header_end++;
    }

    if (header_end >= total_bytes) {
        cerr << "ERROR: Could not find header boundary."
             << endl;
        return 1;
    }

    const size_t header_bytes = header_end + 1;

    // ========================================================
    // Split DATA into 8 newline-aligned chunks
    // ========================================================

    const size_t data_start = header_bytes;
    const size_t data_bytes = total_bytes - data_start;

    size_t split1 = data_start + data_bytes / 8;
    size_t split2 = data_start + (2 * data_bytes) / 8;
    size_t split3 = data_start + (3 * data_bytes) / 8;
    size_t split4 = data_start + (4 * data_bytes) / 8;
    size_t split5 = data_start + (5 * data_bytes) / 8;
    size_t split6 = data_start + (6 * data_bytes) / 8;
    size_t split7 = data_start + (7 * data_bytes) / 8;

    while (split1 < total_bytes &&
           data[split1] != '\n')
        split1++;

    while (split2 < total_bytes &&
           data[split2] != '\n')
        split2++;

    while (split3 < total_bytes &&
           data[split3] != '\n')
        split3++;

    while (split4 < total_bytes &&
           data[split4] != '\n')
        split4++;

    while (split5 < total_bytes &&
           data[split5] != '\n')
        split5++;

    while (split6 < total_bytes &&
           data[split6] != '\n')
        split6++;

    while (split7 < total_bytes &&
           data[split7] != '\n')
        split7++;

    if (split1 >= total_bytes ||
        split2 >= total_bytes ||
        split3 >= total_bytes ||
        split4 >= total_bytes ||
        split5 >= total_bytes ||
        split6 >= total_bytes ||
        split7 >= total_bytes) {

        cerr << "ERROR: Could not find newline "
             << "for dataset split."
             << endl;

        return 1;
    }

    // ========================================================
    // Create 8 independent buffers
    // ========================================================

    vector<uint8_t> buffer0(
        data.begin() + data_start,
        data.begin() + split1 + 1
    );

    vector<uint8_t> buffer1(
        data.begin() + split1 + 1,
        data.begin() + split2 + 1
    );

    vector<uint8_t> buffer2(
        data.begin() + split2 + 1,
        data.begin() + split3 + 1
    );

    vector<uint8_t> buffer3(
        data.begin() + split3 + 1,
        data.begin() + split4 + 1
    );

    vector<uint8_t> buffer4(
        data.begin() + split4 + 1,
        data.begin() + split5 + 1
    );

    vector<uint8_t> buffer5(
        data.begin() + split5 + 1,
        data.begin() + split6 + 1
    );

    vector<uint8_t> buffer6(
        data.begin() + split6 + 1,
        data.begin() + split7 + 1
    );

    vector<uint8_t> buffer7(
        data.begin() + split7 + 1,
        data.end()
    );

    // ========================================================
    // Instantiate 8-parser wrapper
    // ========================================================

    Veight_polaris_log_parser* parser =
        new Veight_polaris_log_parser;

    reset_parser(parser);

    // ========================================================
    // Independent stream positions
    // ========================================================

    size_t i0 = 0;
    size_t i1 = 0;
    size_t i2 = 0;
    size_t i3 = 0;
    size_t i4 = 0;
    size_t i5 = 0;
    size_t i6 = 0;
    size_t i7 = 0;

    uint64_t records0 = 0;
    uint64_t records1 = 0;
    uint64_t records2 = 0;
    uint64_t records3 = 0;
    uint64_t records4 = 0;
    uint64_t records5 = 0;
    uint64_t records6 = 0;
    uint64_t records7 = 0;

    // ========================================================
    // Benchmark
    // ========================================================

    auto sim_start =
        chrono::high_resolution_clock::now();

    total_cycles = 0;

    while (
        i0 < buffer0.size() ||
        i1 < buffer1.size() ||
        i2 < buffer2.size() ||
        i3 < buffer3.size() ||
        i4 < buffer4.size() ||
        i5 < buffer5.size() ||
        i6 < buffer6.size() ||
        i7 < buffer7.size() ||

        parser->valid0_record ||
        parser->valid1_record ||
        parser->valid2_record ||
        parser->valid3_record ||
        parser->valid4_record ||
        parser->valid5_record ||
        parser->valid6_record ||
        parser->valid7_record ||

        !parser->input0_ready ||
        !parser->input1_ready ||
        !parser->input2_ready ||
        !parser->input3_ready ||
        !parser->input4_ready ||
        !parser->input5_ready ||
        !parser->input6_ready ||
        !parser->input7_ready
    ) {

        // ----------------------------------------------------
        // Default inputs
        // ----------------------------------------------------

        parser->valid0_in = 0;
        parser->valid1_in = 0;
        parser->valid2_in = 0;
        parser->valid3_in = 0;
        parser->valid4_in = 0;
        parser->valid5_in = 0;
        parser->valid6_in = 0;
        parser->valid7_in = 0;

        parser->ready0 = 0;
        parser->ready1 = 0;
        parser->ready2 = 0;
        parser->ready3 = 0;
        parser->ready4 = 0;
        parser->ready5 = 0;
        parser->ready6 = 0;
        parser->ready7 = 0;

        // ----------------------------------------------------
        // Record handshakes
        // ----------------------------------------------------

        if (parser->valid0_record)
            parser->ready0 = 1;

        if (parser->valid1_record)
            parser->ready1 = 1;

        if (parser->valid2_record)
            parser->ready2 = 1;

        if (parser->valid3_record)
            parser->ready3 = 1;

        if (parser->valid4_record)
            parser->ready4 = 1;

        if (parser->valid5_record)
            parser->ready5 = 1;

        if (parser->valid6_record)
            parser->ready6 = 1;

        if (parser->valid7_record)
            parser->ready7 = 1;

        // ----------------------------------------------------
        // Feed parser 0
        // ----------------------------------------------------

        if (!parser->valid0_record &&
            parser->input0_ready &&
            i0 < buffer0.size()) {

            parser->ascii0_in = buffer0[i0];
            parser->valid0_in = 1;
        }

        // ----------------------------------------------------
        // Feed parser 1
        // ----------------------------------------------------

        if (!parser->valid1_record &&
            parser->input1_ready &&
            i1 < buffer1.size()) {

            parser->ascii1_in = buffer1[i1];
            parser->valid1_in = 1;
        }

        // ----------------------------------------------------
        // Feed parser 2
        // ----------------------------------------------------

        if (!parser->valid2_record &&
            parser->input2_ready &&
            i2 < buffer2.size()) {

            parser->ascii2_in = buffer2[i2];
            parser->valid2_in = 1;
        }

        // ----------------------------------------------------
        // Feed parser 3
        // ----------------------------------------------------

        if (!parser->valid3_record &&
            parser->input3_ready &&
            i3 < buffer3.size()) {

            parser->ascii3_in = buffer3[i3];
            parser->valid3_in = 1;
        }

        // ----------------------------------------------------
        // Feed parser 4
        // ----------------------------------------------------

        if (!parser->valid4_record &&
            parser->input4_ready &&
            i4 < buffer4.size()) {

            parser->ascii4_in = buffer4[i4];
            parser->valid4_in = 1;
        }

        // ----------------------------------------------------
        // Feed parser 5
        // ----------------------------------------------------

        if (!parser->valid5_record &&
            parser->input5_ready &&
            i5 < buffer5.size()) {

            parser->ascii5_in = buffer5[i5];
            parser->valid5_in = 1;
        }

        // ----------------------------------------------------
        // Feed parser 6
        // ----------------------------------------------------

        if (!parser->valid6_record &&
            parser->input6_ready &&
            i6 < buffer6.size()) {

            parser->ascii6_in = buffer6[i6];
            parser->valid6_in = 1;
        }

        // ----------------------------------------------------
        // Feed parser 7
        // ----------------------------------------------------

        if (!parser->valid7_record &&
            parser->input7_ready &&
            i7 < buffer7.size()) {

            parser->ascii7_in = buffer7[i7];
            parser->valid7_in = 1;
        }

        // ----------------------------------------------------
        // Clock
        // ----------------------------------------------------

        tick(parser);

        // ----------------------------------------------------
        // Advance input positions
        // ----------------------------------------------------

        if (parser->valid0_in)
            i0++;

        if (parser->valid1_in)
            i1++;

        if (parser->valid2_in)
            i2++;

        if (parser->valid3_in)
            i3++;

        if (parser->valid4_in)
            i4++;

        if (parser->valid5_in)
            i5++;

        if (parser->valid6_in)
            i6++;

        if (parser->valid7_in)
            i7++;

        // ----------------------------------------------------
        // Count completed records
        // ----------------------------------------------------

        if (parser->ready0)
            records0++;

        if (parser->ready1)
            records1++;

        if (parser->ready2)
            records2++;

        if (parser->ready3)
            records3++;

        if (parser->ready4)
            records4++;

        if (parser->ready5)
            records5++;

        if (parser->ready6)
            records6++;

        if (parser->ready7)
            records7++;
    }

    // ========================================================
    // Stop simulation timer
    // ========================================================

    auto sim_end =
        chrono::high_resolution_clock::now();

    double simulation_time =
        chrono::duration<double>(
            sim_end - sim_start
        ).count();

    // ========================================================
    // Results
    // ========================================================

    const uint64_t bytes0 = buffer0.size();
    const uint64_t bytes1 = buffer1.size();
    const uint64_t bytes2 = buffer2.size();
    const uint64_t bytes3 = buffer3.size();
    const uint64_t bytes4 = buffer4.size();
    const uint64_t bytes5 = buffer5.size();
    const uint64_t bytes6 = buffer6.size();
    const uint64_t bytes7 = buffer7.size();

    const uint64_t total_records =
        records0 +
        records1 +
        records2 +
        records3 +
        records4 +
        records5 +
        records6 +
        records7;

    const double cycles_per_byte =
        static_cast<double>(total_cycles) /
        static_cast<double>(data_bytes);

    const double throughput =
        (data_bytes / (1024.0 * 1024.0)) /
        simulation_time;

    const double records_per_second =
        static_cast<double>(total_records) /
        simulation_time;

    // ========================================================
    // Output
    // ========================================================

    cout << fixed << setprecision(6);

    cout << "\n============================================\n";
    cout << " POLARIS FPGA 8-PARSER VERILATOR BENCHMARK\n";
    cout << "============================================\n";

    cout << "Dataset              : "
         << filename << endl;

    cout << "Total file bytes     : "
         << total_bytes << endl;

    cout << "Header bytes         : "
         << header_bytes << endl;

    cout << "Data bytes           : "
         << data_bytes << endl;

    cout << "Parser 0 bytes       : "
         << bytes0 << endl;

    cout << "Parser 1 bytes       : "
         << bytes1 << endl;

    cout << "Parser 2 bytes       : "
         << bytes2 << endl;

    cout << "Parser 3 bytes       : "
         << bytes3 << endl;

    cout << "Parser 4 bytes       : "
         << bytes4 << endl;

    cout << "Parser 5 bytes       : "
         << bytes5 << endl;

    cout << "Parser 6 bytes       : "
         << bytes6 << endl;

    cout << "Parser 7 bytes       : "
         << bytes7 << endl;

    cout << "Parser 0 records     : "
         << records0 << endl;

    cout << "Parser 1 records     : "
         << records1 << endl;

    cout << "Parser 2 records     : "
         << records2 << endl;

    cout << "Parser 3 records     : "
         << records3 << endl;

    cout << "Parser 4 records     : "
         << records4 << endl;

    cout << "Parser 5 records     : "
         << records5 << endl;

    cout << "Parser 6 records     : "
         << records6 << endl;

    cout << "Parser 7 records     : "
         << records7 << endl;

    cout << "Total records        : "
         << total_records << endl;

    cout << "FPGA cycles          : "
         << total_cycles << endl;

    cout << "Cycles / byte        : "
         << cycles_per_byte << endl;

    cout << "Simulation time      : "
         << simulation_time << " s" << endl;

    cout << "Simulation throughput: "
         << throughput << " MB/s" << endl;

    cout << "Records / second     : "
         << records_per_second << endl;

    cout << "============================================\n";

    delete parser;

    return 0;
}
