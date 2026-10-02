`timescale 1ns / 1ps

module tb_polaris_log_parser;

    reg clk;
    reg rst;

    reg [7:0] ascii_in;
    reg valid_in;
    reg ready;

    wire valid_record;

    wire [255:0] job_name_out;
    wire [255:0] queued_timestamp_out;
    wire [255:0] start_timestamp_out;
    wire [255:0] end_timestamp_out;
    wire [255:0] username_out;

    wire [63:0] runtime_out;
    wire [63:0] nodes_used_out;
    wire [63:0] nodes_requested_out;
    wire [63:0] cores_used_out;
    wire [63:0] cores_requested_out;
    wire [63:0] gpus_requested_out;

    wire [31:0] exit_status_out;

    wire [511:0] science_field_out;


    polaris_log_parser uut (
        .clk(clk),
        .rst(rst),
        .ascii_in(ascii_in),
        .valid_in(valid_in),
        .ready(ready),
        .valid_record(valid_record),

        .job_name_out(job_name_out),
        .queued_timestamp_out(queued_timestamp_out),
        .start_timestamp_out(start_timestamp_out),
        .end_timestamp_out(end_timestamp_out),
        .username_out(username_out),

        .runtime_out(runtime_out),
        .nodes_used_out(nodes_used_out),
        .nodes_requested_out(nodes_requested_out),
        .cores_used_out(cores_used_out),
        .cores_requested_out(cores_requested_out),
        .gpus_requested_out(gpus_requested_out),

        .exit_status_out(exit_status_out),
        .science_field_out(science_field_out)
    );


    always #5 clk = ~clk;


    task send_char;
        input [7:0] c;
        begin
            @(negedge clk);
            ascii_in = c;
            valid_in = 1;

            @(negedge clk);
            valid_in = 0;
        end
    endtask


    task send_string;
        input [1023:0] str;
        integer i;

        begin
            for (i = 0; i < 128; i = i + 1) begin
                if (str[i*8 +: 8] != 0)
                    send_char(str[i*8 +: 8]);
            end
        end
    endtask


    task comma;
        begin
            send_char(",");
        end
    endtask


    initial begin

        clk = 0;
        rst = 1;

        ascii_in = 0;
        valid_in = 0;

        // Allow parser to consume completed record
        ready = 0;

        $dumpfile("polaris_parser.vcd");
        $dumpvars(0, tb_polaris_log_parser);

        #30;
        rst = 0;


        // =========================================================
        // FIELD 1
        // JOB_NAME
        // =========================================================

        send_string("3123554.polaris");
        comma();


        // FIELD 2
        send_string("0");
        comma();


        // FIELD 3
        send_string("polaris");
        comma();


        // FIELD 4
        // QUEUED_TIMESTAMP
        send_string("\"2024-12-31 23:12:02\"");
        comma();


        // FIELD 5
        send_string("20241231");
        comma();


        // FIELD 6
        // START_TIMESTAMP
        send_string("\"2024-12-31 23:12:08\"");
        comma();


        // FIELD 7
        send_string("20241231");
        comma();


        // FIELD 8
        // END_TIMESTAMP
        send_string("\"2025-01-01 00:02:41\"");
        comma();


        // FIELD 9
        send_string("20250101");
        comma();


        // FIELD 10
        // USERNAME_GENID
        send_string("45432069360014");
        comma();


        // FIELD 11
        send_string("55492081984641");
        comma();


        // FIELD 12
        send_string("Discretionary");
        comma();


        // FIELD 13
        send_string("debug");
        comma();


        // FIELD 14
        send_string("3600.0");
        comma();


        // FIELD 15
        // RUNTIME_SECONDS
        send_string("3020.0");
        comma();


        // FIELD 16
        // NODES_USED
        send_string("2.0");
        comma();


        // FIELD 17
        // NODES_REQUESTED
        send_string("2.0");
        comma();


        // FIELD 18
        // CORES_USED
        send_string("128.0");
        comma();


        // FIELD 19
        // CORES_REQUESTED
        send_string("128.0");
        comma();


        // FIELD 20
        // LOCATION
        // IMPORTANT: quoted comma
        send_string("\"x3005c0s19b1n0,x3005c0s7b0n0\"");
        comma();


        // FIELD 21
        // EXIT_STATUS
        send_string("0");
        comma();


        // FIELD 22
        send_string("12");
        comma();


        // FIELD 23
        send_string("0");
        comma();


        // FIELD 24
        send_string("6");
        comma();


        // FIELD 25
        send_string("0");
        comma();


        // FIELD 26
        send_string("107.84");
        comma();


        // FIELD 27
        send_string("107.84");
        comma();


        // FIELD 28
        send_string("0.0");
        comma();


        // FIELD 29
        send_string("107.84");
        comma();


        // FIELD 30
        send_string("0.0");
        comma();


        // FIELD 31
        send_string("0.0");
        comma();


        // FIELD 32
        send_string("107.84");
        comma();


        // FIELD 33
        send_string("");
        comma();


        // FIELD 34
        send_string("0");
        comma();


        // FIELD 35
        send_string("UNKNOWN");
        comma();


        // FIELD 36
        send_string("0");
        comma();


        // FIELD 37
        send_string("UNKNOWN");
        comma();


        // FIELD 38
        send_string("Not capability");
        comma();


        // FIELD 39
        send_string("\"0% <= x < 20%\"");
        comma();


        // FIELD 40
        send_string("\"0% <= x < 10%\"");
        comma();


        // FIELD 41
        send_string("-1");
        comma();


        // FIELD 42
        send_string("-1");
        comma();


        // FIELD 43
        send_string("-1");
        comma();


        // FIELD 44
        send_string("-1");
        comma();


        // FIELD 45
        send_string("0");
        comma();


        // FIELD 46
        send_string("0");
        comma();


        // FIELD 47
        send_string("0");
        comma();


        // FIELD 48
        send_string("0");
        comma();


        // FIELD 49
        send_string("0");
        comma();


        // FIELD 50
        send_string("0");
        comma();


        // FIELD 51
        send_string("0");
        comma();


        // FIELD 52
        send_string("0");
        comma();


        // FIELD 53
        send_string("0");
        comma();


        // FIELD 54
        send_string("0");
        comma();


        // FIELD 55
        send_string("0.0");
        comma();


        // FIELD 56
        send_string("0");
        comma();


        // FIELD 57
        send_string("8");
        comma();


        // FIELD 58
        send_string("0");
        comma();


        // FIELD 59
        // GPUS_REQUESTED
        send_string("8");
        comma();


        // FIELD 60
        send_string("0");
        comma();


        // FIELD 61
        send_string("");
        comma();


        // FIELD 62
        send_string("");
        comma();


        // FIELD 63
        send_string("");
        comma();


        // FIELD 64
        send_string("-1");
        comma();


        // FIELD 65
        // SCIENCE_FIELD
        send_string("\"Chemistry:Quantum Chemistry\"");
        comma();


        // FIELD 66
        send_string("Chemistry");


        // End of record
        send_char(8'h0A);


        // Wait until parser reports a completed record
        wait(valid_record == 1);
        
        $display("");
        $display("============================================");
        $display(" POLARIS FPGA PARSER TEST");
        $display("============================================");
        
        $display("VALID RECORD = %d", valid_record);
        
        $display("JOB NAME:");
        $display("%h", job_name_out);
        
        $display("QUEUED TIMESTAMP:");
        $display("%h", queued_timestamp_out);
        
        $display("START TIMESTAMP:");
        $display("%h", start_timestamp_out);
        
        $display("END TIMESTAMP:");
        $display("%h", end_timestamp_out);
        
        $display("USERNAME:");
        $display("%h", username_out);
        
        $display("RUNTIME:");
        $display("%h", runtime_out);
        
        $display("NODES USED:");
        $display("%h", nodes_used_out);
        
        $display("NODES REQUESTED:");
        $display("%h", nodes_requested_out);
        
        $display("CORES USED:");
        $display("%h", cores_used_out);
        
        $display("CORES REQUESTED:");
        $display("%h", cores_requested_out);
        
        $display("GPUS REQUESTED:");
        $display("%h", gpus_requested_out);
        
        $display("EXIT STATUS:");
        $display("%h", exit_status_out);
        
        $display("SCIENCE FIELD:");
        $display("%h", science_field_out);
        
        $display("============================================");
        
        // Acknowledge the completed record
        @(negedge clk);
        ready = 1;
        
        #20;
        
        $finish;

    end

endmodule
