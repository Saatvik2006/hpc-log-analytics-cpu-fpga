#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdint>

#include <verilated.h>
#include "Vdual_polaris_log_parser.h"

using namespace std;


// ============================================================
// FPGA clock tick
// ============================================================
uint64_t total_cycles = 0;

void tick(Vdual_polaris_log_parser &parser)
{
    parser.clk = 0;
    parser.eval();

    parser.clk = 1;
    parser.eval();

    total_cycles++;
}


// ============================================================
// Find end of CSV header
// ============================================================
size_t find_header_end(const vector<char> &buffer)
{
    for (size_t i = 0; i < buffer.size(); i++)
    {
        if (buffer[i] == '\n')
            return i + 1;
    }

    return buffer.size();
}


// ============================================================
// Find next newline after split position
// ============================================================
size_t find_next_newline(
    const vector<char> &buffer,
    size_t position)
{
    while (position < buffer.size() &&
           buffer[position] != '\n')
    {
        position++;
    }

    if (position < buffer.size())
        return position + 1;

    return buffer.size();
}


// ============================================================
// Main
// ============================================================
int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);

    if (argc < 2)
    {
        cerr << "Usage: ./Vdual_polaris_log_parser <dataset>\n";
        return 1;
    }


    // ========================================================
    // Open dataset
    // ========================================================
    ifstream logfile(argv[1], ios::binary);

    if (!logfile.is_open())
    {
        cerr << "Failed to open dataset.\n";
        return 1;
    }


    // ========================================================
    // Read entire dataset
    // ========================================================
    logfile.seekg(0, ios::end);
    uint64_t total_file_bytes = logfile.tellg();

    logfile.seekg(0, ios::beg);

    vector<char> buffer;
    buffer.reserve(total_file_bytes);

    char c;

    while (logfile.get(c))
        buffer.push_back(c);

    logfile.close();


    // ========================================================
    // Find header
    // ========================================================
    size_t header_end = find_header_end(buffer);

    size_t data_start = header_end;

    uint64_t header_bytes = data_start;

    uint64_t data_bytes =
        buffer.size() - data_start;


    // ========================================================
    // Split data approximately in half
    // ========================================================
    size_t half =
        data_start + data_bytes / 2;

    size_t split =
        find_next_newline(buffer, half);


    // ========================================================
    // Create parser buffers
    // ========================================================
    vector<char> buffer0(
        buffer.begin() + data_start,
        buffer.begin() + split
    );

    vector<char> buffer1(
        buffer.begin() + split,
        buffer.end()
    );


    uint64_t parser0_bytes = buffer0.size();
    uint64_t parser1_bytes = buffer1.size();


    // ========================================================
    // Create dual parser
    // ========================================================
    Vdual_polaris_log_parser parser;


    // ========================================================
    // Initial signals
    // ========================================================
    parser.clk = 0;

    parser.rst = 1;

    parser.valid0_in = 0;
    parser.ready0 = 0;
    parser.ascii0_in = 0;

    parser.valid1_in = 0;
    parser.ready1 = 0;
    parser.ascii1_in = 0;


    // ========================================================
    // Reset
    // ========================================================
    tick(parser);
    tick(parser);

    parser.rst = 0;

    tick(parser);


    // ========================================================
    // Benchmark counters
    // ========================================================
    total_cycles = 0;

    uint64_t records0 = 0;
    uint64_t records1 = 0;

    size_t i0 = 0;
    size_t i1 = 0;


    auto parse_start =
        chrono::high_resolution_clock::now();


    // ========================================================
    // Main dual-parser loop
    // ========================================================
    while (i0 < buffer0.size() ||
           i1 < buffer1.size())
    {

        // ----------------------------------------------------
        // Default signals
        // ----------------------------------------------------
        parser.valid0_in = 0;
        parser.valid1_in = 0;

        parser.ready0 = 0;
        parser.ready1 = 0;


        // ----------------------------------------------------
        // Feed parser 0
        // ----------------------------------------------------
        bool newline0 = false;

        if (i0 < buffer0.size())
        {
            unsigned char byte =
                static_cast<unsigned char>(buffer0[i0]);

            parser.ascii0_in = byte;
            parser.valid0_in = 1;

            i0++;

            if (byte == '\n')
                newline0 = true;
        }


        // ----------------------------------------------------
        // Feed parser 1
        // ----------------------------------------------------
        bool newline1 = false;

        if (i1 < buffer1.size())
        {
            unsigned char byte =
                static_cast<unsigned char>(buffer1[i1]);

            parser.ascii1_in = byte;
            parser.valid1_in = 1;

            i1++;

            if (byte == '\n')
                newline1 = true;
        }


        // ----------------------------------------------------
        // Consume the input bytes
        // ----------------------------------------------------
        tick(parser);


        // ====================================================
        // A newline moves the RTL from PARSE -> DONE.
        //
        // valid_record is generated in the following clock.
        // Therefore DO NOT send another byte during that clock.
        // ====================================================

        if (newline0 || newline1)
        {
            parser.valid0_in = 0;
            parser.valid1_in = 0;

            parser.ready0 = 0;
            parser.ready1 = 0;

            parser.ascii0_in = 0;
            parser.ascii1_in = 0;

            // Give the parser(s) the DONE transition cycle.
            tick(parser);


            // ------------------------------------------------
            // Parser 0 record
            // ------------------------------------------------
            if (newline0 && parser.valid0_record)
            {
                records0++;

                parser.ready0 = 1;
            }


            // ------------------------------------------------
            // Parser 1 record
            // ------------------------------------------------
            if (newline1 && parser.valid1_record)
            {
                records1++;

                parser.ready1 = 1;
            }


            // ------------------------------------------------
            // Complete ready handshake
            // ------------------------------------------------
            if (
                (newline0 && parser.valid0_record) ||
                (newline1 && parser.valid1_record)
            )
            {
                tick(parser);
            }


            // ------------------------------------------------
            // Clear handshake
            // ------------------------------------------------
            parser.ready0 = 0;
            parser.ready1 = 0;
        }
    }


    // ========================================================
    // Final safety drain
    //
    // This does not assume which parser finished last.
    // ========================================================
    parser.valid0_in = 0;
    parser.valid1_in = 0;

    parser.ready0 = 0;
    parser.ready1 = 0;

    parser.ascii0_in = 0;
    parser.ascii1_in = 0;


    // One final clock in case the last newline has just
    // transitioned the parser into STATE_DONE.
    tick(parser);


    bool final0 = parser.valid0_record;
    bool final1 = parser.valid1_record;


    if (final0)
    {
        records0++;
        parser.ready0 = 1;
    }

    if (final1)
    {
        records1++;
        parser.ready1 = 1;
    }


    if (final0 || final1)
    {
        tick(parser);
    }


    parser.ready0 = 0;
    parser.ready1 = 0;


    // ========================================================
    // Benchmark timing
    // ========================================================
    auto parse_end =
        chrono::high_resolution_clock::now();


    // ========================================================
    // Results
    // ========================================================
    uint64_t total_records =
        records0 + records1;

    double parse_seconds =
        chrono::duration<double>(
            parse_end - parse_start
        ).count();

    double cycles_per_byte =
        static_cast<double>(total_cycles) /
        static_cast<double>(data_bytes);

    double simulation_throughput =
        (data_bytes / 1024.0 / 1024.0) /
        parse_seconds;

    double records_per_second =
        static_cast<double>(total_records) /
        parse_seconds;


    // ========================================================
    // Print
    // ========================================================
    cout << "\n";
    cout << "============================================\n";
    cout << " POLARIS FPGA 2-PARSER VERILATOR BENCHMARK\n";
    cout << "============================================\n";

    cout << "Dataset              : "
         << argv[1] << "\n";

    cout << "Total file bytes     : "
         << total_file_bytes << "\n";

    cout << "Header bytes         : "
         << header_bytes << "\n";

    cout << "Data bytes           : "
         << data_bytes << "\n";

    cout << "Parser 0 bytes       : "
         << parser0_bytes << "\n";

    cout << "Parser 1 bytes       : "
         << parser1_bytes << "\n";

    cout << "Parser 0 records     : "
         << records0 << "\n";

    cout << "Parser 1 records     : "
         << records1 << "\n";

    cout << "Total records        : "
         << total_records << "\n";

    cout << "FPGA cycles          : "
         << total_cycles << "\n";

    cout << "Cycles / byte        : "
         << cycles_per_byte << "\n";

    cout << "Simulation time      : "
         << parse_seconds
         << " s\n";

    cout << "Simulation throughput: "
         << simulation_throughput
         << " MB/s\n";

    cout << "Records / second     : "
         << records_per_second
         << "\n";

    cout << "============================================\n";


    return 0;
}
