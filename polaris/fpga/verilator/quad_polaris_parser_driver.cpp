#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdint>

#include <verilated.h>
#include "Vquad_polaris_log_parser.h"

using namespace std;


// ============================================================
// FPGA clock tick
// ============================================================
uint64_t total_cycles = 0;

void tick(Vquad_polaris_log_parser &parser)
{
    parser.clk = 0;
    parser.eval();

    parser.clk = 1;
    parser.eval();

    total_cycles++;
}


// ============================================================
// Find CSV header
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
// Find next newline
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
        cerr << "Usage: ./Vquad_polaris_log_parser <dataset>\n";
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

    uint64_t total_file_bytes =
        logfile.tellg();

    logfile.seekg(0, ios::beg);

    vector<char> buffer;
    buffer.reserve(total_file_bytes);

    char c;

    while (logfile.get(c))
        buffer.push_back(c);

    logfile.close();


    // ========================================================
    // Header
    // ========================================================
    size_t header_end =
        find_header_end(buffer);

    size_t data_start =
        header_end;

    uint64_t header_bytes =
        data_start;

    uint64_t data_bytes =
        buffer.size() - data_start;


    // ========================================================
    // Split data into FOUR newline-aligned chunks
    // ========================================================
    size_t split0 =
        find_next_newline(
            buffer,
            data_start + data_bytes / 4
        );

    size_t split1 =
        find_next_newline(
            buffer,
            data_start + data_bytes / 2
        );

    size_t split2 =
        find_next_newline(
            buffer,
            data_start + (3 * data_bytes) / 4
        );


    // ========================================================
    // Create four parser buffers
    // ========================================================
    vector<char> buffer0(
        buffer.begin() + data_start,
        buffer.begin() + split0
    );

    vector<char> buffer1(
        buffer.begin() + split0,
        buffer.begin() + split1
    );

    vector<char> buffer2(
        buffer.begin() + split1,
        buffer.begin() + split2
    );

    vector<char> buffer3(
        buffer.begin() + split2,
        buffer.end()
    );


    uint64_t parser0_bytes = buffer0.size();
    uint64_t parser1_bytes = buffer1.size();
    uint64_t parser2_bytes = buffer2.size();
    uint64_t parser3_bytes = buffer3.size();


    // ========================================================
    // Create quad parser
    // ========================================================
    Vquad_polaris_log_parser parser;


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

    parser.valid2_in = 0;
    parser.ready2 = 0;
    parser.ascii2_in = 0;

    parser.valid3_in = 0;
    parser.ready3 = 0;
    parser.ascii3_in = 0;


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
    uint64_t records2 = 0;
    uint64_t records3 = 0;

    size_t i0 = 0;
    size_t i1 = 0;
    size_t i2 = 0;
    size_t i3 = 0;


    auto parse_start =
        chrono::high_resolution_clock::now();


    // ========================================================
    // Main streaming loop
    //
    // input_ready comes from the RTL.
    //
    // The driver never guesses whether a newline completed
    // a record.
    // ========================================================
   while (
        i0 < buffer0.size() ||
        i1 < buffer1.size() ||
        i2 < buffer2.size() ||
        i3 < buffer3.size() ||
    
        parser.valid0_record ||
        parser.valid1_record ||
        parser.valid2_record ||
        parser.valid3_record ||
    
        !parser.input0_ready ||
        !parser.input1_ready ||
        !parser.input2_ready ||
        !parser.input3_ready
    )
    {

        // ====================================================
        // Default inputs
        // ====================================================
        parser.valid0_in = 0;
        parser.valid1_in = 0;
        parser.valid2_in = 0;
        parser.valid3_in = 0;

        parser.ready0 = 0;
        parser.ready1 = 0;
        parser.ready2 = 0;
        parser.ready3 = 0;


        // ====================================================
        // Complete record handshakes first
        // ====================================================

        if (parser.valid0_record)
            parser.ready0 = 1;

        if (parser.valid1_record)
            parser.ready1 = 1;

        if (parser.valid2_record)
            parser.ready2 = 1;

        if (parser.valid3_record)
            parser.ready3 = 1;


        // ====================================================
        // Feed parser 0 only when RTL says it can accept data
        // ====================================================
        if (!parser.valid0_record &&
            parser.input0_ready &&
            i0 < buffer0.size())
        {
            parser.ascii0_in =
                static_cast<unsigned char>(
                    buffer0[i0]
                );

            parser.valid0_in = 1;
        }


        // ====================================================
        // Feed parser 1
        // ====================================================
        if (!parser.valid1_record &&
            parser.input1_ready &&
            i1 < buffer1.size())
        {
            parser.ascii1_in =
                static_cast<unsigned char>(
                    buffer1[i1]
                );

            parser.valid1_in = 1;
        }


        // ====================================================
        // Feed parser 2
        // ====================================================
        if (!parser.valid2_record &&
            parser.input2_ready &&
            i2 < buffer2.size())
        {
            parser.ascii2_in =
                static_cast<unsigned char>(
                    buffer2[i2]
                );

            parser.valid2_in = 1;
        }


        // ====================================================
        // Feed parser 3
        // ====================================================
        if (!parser.valid3_record &&
            parser.input3_ready &&
            i3 < buffer3.size())
        {
            parser.ascii3_in =
                static_cast<unsigned char>(
                    buffer3[i3]
                );

            parser.valid3_in = 1;
        }


        // ====================================================
        // Clock
        // ====================================================
        tick(parser);


        // ====================================================
        // Advance input indices only for bytes actually sent
        // ====================================================

        if (parser.valid0_in)
            i0++;

        if (parser.valid1_in)
            i1++;

        if (parser.valid2_in)
            i2++;

        if (parser.valid3_in)
            i3++;


        // ====================================================
        // Count completed records
        //
        // valid_record was asserted by the parser before the
        // handshake clock. The record count therefore occurs
        // when the handshake is being completed.
        // ====================================================

        if (parser.ready0)
            records0++;

        if (parser.ready1)
            records1++;

        if (parser.ready2)
            records2++;

        if (parser.ready3)
            records3++;
    }


    // ========================================================
    // Benchmark timing
    // ========================================================
    auto parse_end =
        chrono::high_resolution_clock::now();


    // ========================================================
    // Results
    // ========================================================
    uint64_t total_records =
        records0 +
        records1 +
        records2 +
        records3;

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
    // Print benchmark
    // ========================================================
    cout << "\n";
    cout << "============================================\n";
    cout << " POLARIS FPGA 4-PARSER VERILATOR BENCHMARK\n";
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

    cout << "Parser 2 bytes       : "
         << parser2_bytes << "\n";

    cout << "Parser 3 bytes       : "
         << parser3_bytes << "\n";

    cout << "Parser 0 records     : "
         << records0 << "\n";

    cout << "Parser 1 records     : "
         << records1 << "\n";

    cout << "Parser 2 records     : "
         << records2 << "\n";

    cout << "Parser 3 records     : "
         << records3 << "\n";

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
