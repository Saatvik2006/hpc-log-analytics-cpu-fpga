#include <iostream>
#include <fstream>

#include <verilated.h>
#include <vector>
#include <chrono>

#include "Vnasa_log_parser.h"
#include "log_record.h"


const bool EXPORT_RECORDS = false;

using namespace std;

using Clock = std::chrono::steady_clock;

Clock::time_point total_start;
Clock::time_point total_end;

Clock::time_point open_start;
Clock::time_point open_end;

Clock::time_point read_start;
Clock::time_point read_end;

Clock::time_point parse_start;
Clock::time_point parse_end;

Clock::time_point export_start;
Clock::time_point export_end;

uint64_t total_cycles = 0;

void tick(Vnasa_log_parser &parser)
{
    parser.clk = 0;
    parser.eval();

    parser.clk = 1;
    parser.eval();

    total_cycles++;
}

void copy_wide_field(char *dest, const WData *src, int bytes)
{
    int index = 0;

    for(int word = 0; index < bytes; word++)
    {
        uint32_t value = src[word];

        for(int b = 0; b < 4 && index < bytes; b++)
        {
            dest[index++] = (value >> (8 * b)) & 0xFF;
        }
    }

    dest[bytes] = '\0';
}

void copy_small_field(char *dest, uint64_t value, int bytes)
{
    for(int i = 0; i < bytes; i++)
        dest[i] = (value >> (8 * i)) & 0xFF;

    dest[bytes] = '\0';
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);

    Vnasa_log_parser parser;
    LogRecord record;

    uint64_t record_count = 0;

    parser.clk = 0;
    parser.rst = 1;
    parser.valid_in = 0;
    parser.ready = 0;
    parser.ascii_in = 0;

    
    // Hold reset for two complete clock cycles
    tick(parser);
    tick(parser);
    
    parser.rst = 0;
    
    // Give one clean clock after reset
    tick(parser);

    if(argc < 2)
    {
        std::cerr << "Usage: ./Vnasa_log_parser <dataset>\n";
        return 1;
    }
    
    std::ifstream logfile(argv[1]);

    if(!logfile.is_open())
    {
        cout << "Failed to open dataset." << endl;
        return 1;
    }

    cout << "Dataset opened successfully." << endl;

    logfile.seekg(0, ios::end);
    uint64_t dataset_size = logfile.tellg();
    logfile.seekg(0, ios::beg);
    
    std::vector<char> buffer;
    buffer.reserve(dataset_size);
    
    // ---------------- READ ----------------
    
    auto read_start = std::chrono::high_resolution_clock::now();
    
    char c;
    while(logfile.get(c))
        buffer.push_back(c);
    
    auto read_end = std::chrono::high_resolution_clock::now();
    
    // ---------------- PARSE ----------------

    total_cycles = 0;
    
    auto parse_start = std::chrono::high_resolution_clock::now();
    
    for(char c : buffer)
    {
        parser.ascii_in = static_cast<unsigned char>(c);
        parser.valid_in = 1;
    
        tick(parser);
    
        if(parser.valid_record)
        {
//            copy_wide_field(record.ip, parser.ip_out, 64);
//            copy_wide_field(record.timestamp, parser.timestamp_out, 32);
//            copy_small_field(record.method, parser.method_out, 4);
//            copy_small_field(record.protocol, parser.protocol_out, 8);
//            copy_small_field(record.status, parser.status_out, 4);
//            copy_small_field(record.bytes, parser.bytes_out, 8);
//            copy_wide_field(record.url, parser.url_out, 64);
    
            record_count++;
    
            parser.ready = 1;
            tick(parser);
    
            parser.ready = 0;
            tick(parser);
        }
    }
    
    parser.valid_in = 0;
    tick(parser);
    
    auto parse_end = std::chrono::high_resolution_clock::now();

    double read_seconds =
            std::chrono::duration<double>(read_end - read_start).count();
        
        double parse_seconds =
            std::chrono::duration<double>(parse_end - parse_start).count();
        
        double total_seconds =
            read_seconds + parse_seconds;

    double parse_mbps =
            (dataset_size / 1024.0 / 1024.0) / parse_seconds;
        
        double total_mbps =
            (dataset_size / 1024.0 / 1024.0) / total_seconds;
        
        double records_per_sec =
                record_count / parse_seconds;

        double cycles_per_byte =
            static_cast<double>(total_cycles) / dataset_size;

    cout << "\n========== FPGA Parser Benchmark ==========\n";

    cout << "Total FPGA Cycles : "
         << total_cycles
         << endl;


    cout << "Cycles / Byte : "
         << cycles_per_byte
         << endl;
    
    cout << "Dataset Size     : "
         << dataset_size / 1024.0 / 1024.0
         << " MB\n";
    
    cout << "Parsed Records   : "
              << record_count
              << '\n';
    
    cout << "Read Time        : "
         << read_seconds
         << " s\n";
    
    cout << "Parse Time       : "
         << parse_seconds
         << " s\n";
    
    cout << "Total Time       : "
         << total_seconds
         << " s\n";
    
    cout << "Records / Second : "
         << records_per_sec
         << '\n';
    
    cout << "Parse Throughput : "
         << parse_mbps
         << " MB/s\n";
    
    cout << "Total Throughput : "
         << total_mbps
         << " MB/s\n";
    
    logfile.close();
    
    return 0;
}
