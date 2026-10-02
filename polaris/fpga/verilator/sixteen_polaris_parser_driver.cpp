#include <verilated.h>
#include "Vsixteen_polaris_log_parser.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using Clock = chrono::steady_clock;


// ============================================================
// Get input_ready
// ============================================================

static bool get_input_ready(
    Vsixteen_polaris_log_parser* top,
    int p)
{
    switch (p) {
        case 0:  return top->input0_ready;
        case 1:  return top->input1_ready;
        case 2:  return top->input2_ready;
        case 3:  return top->input3_ready;
        case 4:  return top->input4_ready;
        case 5:  return top->input5_ready;
        case 6:  return top->input6_ready;
        case 7:  return top->input7_ready;
        case 8:  return top->input8_ready;
        case 9:  return top->input9_ready;
        case 10: return top->input10_ready;
        case 11: return top->input11_ready;
        case 12: return top->input12_ready;
        case 13: return top->input13_ready;
        case 14: return top->input14_ready;
        case 15: return top->input15_ready;
    }

    return false;
}


// ============================================================
// Get valid_record
// ============================================================

static bool get_valid_record(
    Vsixteen_polaris_log_parser* top,
    int p)
{
    switch (p) {
        case 0:  return top->valid0_record;
        case 1:  return top->valid1_record;
        case 2:  return top->valid2_record;
        case 3:  return top->valid3_record;
        case 4:  return top->valid4_record;
        case 5:  return top->valid5_record;
        case 6:  return top->valid6_record;
        case 7:  return top->valid7_record;
        case 8:  return top->valid8_record;
        case 9:  return top->valid9_record;
        case 10: return top->valid10_record;
        case 11: return top->valid11_record;
        case 12: return top->valid12_record;
        case 13: return top->valid13_record;
        case 14: return top->valid14_record;
        case 15: return top->valid15_record;
    }

    return false;
}


// ============================================================
// Set ASCII input
// ============================================================

static void set_ascii(
    Vsixteen_polaris_log_parser* top,
    int p,
    uint8_t value)
{
    switch (p) {
        case 0:  top->ascii0_in = value; break;
        case 1:  top->ascii1_in = value; break;
        case 2:  top->ascii2_in = value; break;
        case 3:  top->ascii3_in = value; break;
        case 4:  top->ascii4_in = value; break;
        case 5:  top->ascii5_in = value; break;
        case 6:  top->ascii6_in = value; break;
        case 7:  top->ascii7_in = value; break;
        case 8:  top->ascii8_in = value; break;
        case 9:  top->ascii9_in = value; break;
        case 10: top->ascii10_in = value; break;
        case 11: top->ascii11_in = value; break;
        case 12: top->ascii12_in = value; break;
        case 13: top->ascii13_in = value; break;
        case 14: top->ascii14_in = value; break;
        case 15: top->ascii15_in = value; break;
    }
}


// ============================================================
// Set valid_in
// ============================================================

static void set_valid(
    Vsixteen_polaris_log_parser* top,
    int p,
    uint8_t value)
{
    switch (p) {
        case 0:  top->valid0_in = value; break;
        case 1:  top->valid1_in = value; break;
        case 2:  top->valid2_in = value; break;
        case 3:  top->valid3_in = value; break;
        case 4:  top->valid4_in = value; break;
        case 5:  top->valid5_in = value; break;
        case 6:  top->valid6_in = value; break;
        case 7:  top->valid7_in = value; break;
        case 8:  top->valid8_in = value; break;
        case 9:  top->valid9_in = value; break;
        case 10: top->valid10_in = value; break;
        case 11: top->valid11_in = value; break;
        case 12: top->valid12_in = value; break;
        case 13: top->valid13_in = value; break;
        case 14: top->valid14_in = value; break;
        case 15: top->valid15_in = value; break;
    }
}


// ============================================================
// Set ready
// ============================================================

static void set_ready(
    Vsixteen_polaris_log_parser* top,
    int p,
    uint8_t value)
{
    switch (p) {
        case 0:  top->ready0 = value; break;
        case 1:  top->ready1 = value; break;
        case 2:  top->ready2 = value; break;
        case 3:  top->ready3 = value; break;
        case 4:  top->ready4 = value; break;
        case 5:  top->ready5 = value; break;
        case 6:  top->ready6 = value; break;
        case 7:  top->ready7 = value; break;
        case 8:  top->ready8 = value; break;
        case 9:  top->ready9 = value; break;
        case 10: top->ready10 = value; break;
        case 11: top->ready11 = value; break;
        case 12: top->ready12 = value; break;
        case 13: top->ready13 = value; break;
        case 14: top->ready14 = value; break;
        case 15: top->ready15 = value; break;
    }
}


// ============================================================
// Clock
// ============================================================

static void tick(
    Vsixteen_polaris_log_parser* top)
{
    top->clk = 0;
    top->eval();

    top->clk = 1;
    top->eval();
}


// ============================================================
// Find header end
// ============================================================

static size_t find_header_end(
    const vector<uint8_t>& data)
{
    for (size_t i = 0; i < data.size(); i++) {

        if (data[i] == '\n')
            return i + 1;
    }

    return data.size();
}


// ============================================================
// Move split forward to next newline
// ============================================================

static size_t find_next_newline(
    const vector<uint8_t>& data,
    size_t position)
{
    while (
        position < data.size() &&
        data[position] != '\n')
    {
        position++;
    }

    if (position < data.size())
        return position + 1;

    return data.size();
}


// ============================================================
// Main
// ============================================================

int main(int argc, char** argv)
{
    Verilated::commandArgs(argc, argv);

    if (argc < 2) {

        cerr
            << "Usage: "
            << argv[0]
            << " <polaris_dataset.csv>\n";

        return 1;
    }

    const string filename = argv[1];


    // ========================================================
    // Read dataset
    // ========================================================

    ifstream file(
        filename,
        ios::binary
    );

    if (!file.is_open()) {

        cerr
            << "ERROR: Cannot open file: "
            << filename
            << "\n";

        return 1;
    }

    vector<uint8_t> data(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );

    file.close();


    if (data.empty()) {

        cerr
            << "ERROR: Dataset is empty.\n";

        return 1;
    }


    const uint64_t total_file_bytes =
        data.size();


    // ========================================================
    // Find header
    // ========================================================

    const size_t header_end =
        find_header_end(data);

    const size_t data_start =
        header_end;

    const uint64_t header_bytes =
        header_end;

    const uint64_t data_bytes =
        data.size() - data_start;


    // ========================================================
    // Split data into 16 newline-aligned chunks
    // ========================================================

    array<size_t, 16> chunk_start{};
    array<size_t, 16> chunk_end{};

    const size_t target =
        (data_bytes + 15) / 16;

    chunk_start[0] =
        data_start;


    for (int p = 0; p < 15; p++) {

        size_t boundary =
            data_start +
            target * (p + 1);

        if (boundary > data.size())
            boundary = data.size();

        boundary =
            find_next_newline(
                data,
                boundary
            );

        chunk_end[p] =
            boundary;

        chunk_start[p + 1] =
            boundary;
    }


    chunk_end[15] =
        data.size();


    // ========================================================
    // Create parser
    // ========================================================

    auto* parser =
        new Vsixteen_polaris_log_parser;


    // ========================================================
    // Initial signals
    // ========================================================

    parser->clk = 0;
    parser->rst = 1;

    for (int p = 0; p < 16; p++) {

        set_ascii(parser, p, 0);
        set_valid(parser, p, 0);
        set_ready(parser, p, 0);
    }

    parser->eval();


    // ========================================================
    // Reset
    // ========================================================

    tick(parser);
    tick(parser);

    parser->rst = 0;

    tick(parser);


    // ========================================================
    // Parser positions
    // ========================================================

    array<size_t, 16> position{};

    for (int p = 0; p < 16; p++)
        position[p] = chunk_start[p];


    array<uint64_t, 16> byte_count{};
    array<uint64_t, 16> record_count{};


    uint64_t total_cycles = 0;


    // ========================================================
    // Benchmark start
    // ========================================================

    const auto start_time =
        Clock::now();


    // ========================================================
    // Main streaming loop
    // ========================================================

    while (true) {

        bool all_finished = true;

        /*
         * Record whether valid_record was already asserted
         * BEFORE this clock.
         *
         * This is important because valid_record is held high
         * by the parser until ready is asserted.
         */
        array<bool, 16> valid_before{};

        /*
         * Record which parsers actually receive a byte on
         * this clock.
         */
        array<bool, 16> byte_sent{};


        // ----------------------------------------------------
        // Prepare all 16 parsers
        // ----------------------------------------------------

        for (int p = 0; p < 16; p++) {

            valid_before[p] =
                get_valid_record(parser, p);


            // Defaults
            set_valid(parser, p, 0);
            set_ready(parser, p, 0);
            set_ascii(parser, p, 0);


            // ------------------------------------------------
            // Completed record
            // ------------------------------------------------

            if (valid_before[p]) {

                /*
                 * Parser is in STATE_DONE.
                 *
                 * Do NOT send another byte.
                 *
                 * ready releases the completed record.
                 */
                set_ready(parser, p, 1);
            }


            // ------------------------------------------------
            // Normal input
            // ------------------------------------------------

            else if (
                position[p] < chunk_end[p] &&
                get_input_ready(parser, p)
            ) {

                set_ascii(
                    parser,
                    p,
                    data[position[p]]
                );

                set_valid(
                    parser,
                    p,
                    1
                );

                byte_sent[p] = true;
            }
        }


        // ----------------------------------------------------
        // One FPGA clock
        // ----------------------------------------------------

        tick(parser);

        total_cycles++;


        // ----------------------------------------------------
        // Account for bytes consumed
        // ----------------------------------------------------

        for (int p = 0; p < 16; p++) {

            if (byte_sent[p]) {

                position[p]++;
                byte_count[p]++;
            }
        }


        // ----------------------------------------------------
        // Detect newly completed records
        // ----------------------------------------------------

        for (int p = 0; p < 16; p++) {

            const bool valid_after =
                get_valid_record(parser, p);


            /*
             * valid_record changed:
             *
             * 0 -> 1
             *
             * Therefore a new record has just completed.
             */
            if (
                !valid_before[p] &&
                valid_after
            ) {

                record_count[p]++;
            }
        }


        // ----------------------------------------------------
        // Determine whether everything is finished
        // ----------------------------------------------------

        for (int p = 0; p < 16; p++) {

            if (
                position[p] < chunk_end[p]
            ) {

                all_finished = false;
                break;
            }


            /*
             * All bytes have been sent, but the parser may
             * still be processing the final newline and/or
             * holding the completed record.
             */
            if (get_valid_record(parser, p)) {

                all_finished = false;
                break;
            }


            if (!get_input_ready(parser, p)) {

                all_finished = false;
                break;
            }
        }


        if (all_finished)
            break;
    }


    // ========================================================
    // Timing
    // ========================================================

    const auto end_time =
        Clock::now();


    const double simulation_time =
        chrono::duration<double>(
            end_time - start_time
        ).count();


    // ========================================================
    // Statistics
    // ========================================================

    uint64_t total_records = 0;
    uint64_t total_bytes = 0;

    for (int p = 0; p < 16; p++) {

        total_records +=
            record_count[p];

        total_bytes +=
            byte_count[p];
    }


    const double cycles_per_byte =
        static_cast<double>(total_cycles) /
        static_cast<double>(data_bytes);


    const double throughput =
        (
            static_cast<double>(data_bytes) /
            (1024.0 * 1024.0)
        ) /
        simulation_time;


    const double records_per_second =
        static_cast<double>(total_records) /
        simulation_time;


    // ========================================================
    // Output
    // ========================================================

    cout << "\n";
    cout
        << "============================================\n";
    cout
        << " POLARIS FPGA 16-PARSER VERILATOR BENCHMARK\n";
    cout
        << "============================================\n";

    cout
        << "Dataset              : "
        << filename
        << "\n";

    cout
        << "Total file bytes     : "
        << total_file_bytes
        << "\n";

    cout
        << "Header bytes         : "
        << header_bytes
        << "\n";

    cout
        << "Data bytes           : "
        << data_bytes
        << "\n";


    for (int p = 0; p < 16; p++) {

        cout
            << "Parser "
            << p
            << " bytes       : "
            << byte_count[p]
            << "\n";
    }


    for (int p = 0; p < 16; p++) {

        cout
            << "Parser "
            << p
            << " records     : "
            << record_count[p]
            << "\n";
    }


    cout
        << "Total records        : "
        << total_records
        << "\n";

    cout
        << "FPGA cycles          : "
        << total_cycles
        << "\n";

    cout
        << "Cycles / byte        : "
        << cycles_per_byte
        << "\n";

    cout
        << "Simulation time      : "
        << simulation_time
        << " s\n";

    cout
        << "Simulation throughput: "
        << throughput
        << " MB/s\n";

    cout
        << "Records / second     : "
        << records_per_second
        << "\n";

    cout
        << "============================================\n";


    delete parser;

    return 0;
}
