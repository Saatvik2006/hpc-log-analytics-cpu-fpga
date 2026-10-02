`timescale 1ns / 1ps

module tb_polaris_real;

    reg clk;
    reg rst;

    reg [7:0] ascii_in;
    reg       valid_in;
    reg       ready;

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


    // ============================================================
    // Parser
    // ============================================================

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


    // ============================================================
    // Clock
    // ============================================================

    always #5 clk = ~clk;


    // ============================================================
    // Runtime configuration
    // ============================================================

    string input_file;

    integer expected_records;

    integer fd;
    integer c;

    integer bytes_processed;
    integer records;
    integer header_bytes;

    integer verbose_records;

    reg [7:0] current_byte;


    // ============================================================
    // Cycle counter
    // ============================================================

    longint unsigned cycle_count;

    always @(posedge clk) begin

        if (rst)
            cycle_count <= 0;

        else
            cycle_count <= cycle_count + 1;

    end


    // ============================================================
    // Send one byte to FPGA parser
    // ============================================================

    task send_byte;

        input [7:0] b;

        begin

            @(negedge clk);

            ascii_in = b;
            valid_in = 1;

            @(negedge clk);

            valid_in = 0;

        end

    endtask


    // ============================================================
    // Wait for parser to complete a record
    // ============================================================

    task wait_for_record;

        begin

            wait (valid_record === 1'b1);

            @(negedge clk);

            ready = 1;

            @(negedge clk);

            ready = 0;

        end

    endtask


    // ============================================================
    // Capture completed records
    //
    // Detailed output is controlled using:
    // +VERBOSE=1
    //
    // For large datasets use:
    // +VERBOSE=0
    // ============================================================

    always @(posedge clk) begin

        if (!rst && valid_record) begin

            records = records + 1;

            if (verbose_records != 0) begin

                $display("");
                $display("--------------------------------------------");
                $display("REAL RECORD %0d", records);
                $display("--------------------------------------------");

                $display("JOB         = %h", job_name_out);

                $display("QUEUED TS   = %h", queued_timestamp_out);

                $display("START TS    = %h", start_timestamp_out);

                $display("END TS      = %h", end_timestamp_out);

                $display("USERNAME    = %h", username_out);

                $display("RUNTIME     = %h", runtime_out);

                $display("NODES USED  = %h", nodes_used_out);

                $display("NODES REQ   = %h", nodes_requested_out);

                $display("CORES USED  = %h", cores_used_out);

                $display("CORES REQ   = %h", cores_requested_out);

                $display("GPUS REQ    = %h", gpus_requested_out);

                $display("EXIT STATUS = %h", exit_status_out);

                $display("SCIENCE     = %h", science_field_out);

                $display("--------------------------------------------");

            end

        end

    end


    // ============================================================
    // Main simulation
    // ============================================================

    initial begin

        clk = 0;
        rst = 1;

        ascii_in = 0;
        valid_in = 0;
        ready = 0;

        bytes_processed = 0;
        records = 0;
        header_bytes = 0;

        verbose_records = 0;


        // ========================================================
        // Default configuration
        // ========================================================

        input_file =
            "datasets/polaris_real_10.csv";

        expected_records = -1;


        // ========================================================
        // Command-line configuration
        //
        // Example:
        //
        // +INPUT=datasets/polaris_real_10.csv
        // +EXPECTED_RECORDS=10
        // +VERBOSE=1
        // ========================================================

        if ($value$plusargs("INPUT=%s", input_file)) begin

            $display(
                "Using input file from command line."
            );

        end


        if ($value$plusargs(
                "EXPECTED_RECORDS=%d",
                expected_records
            )) begin

            $display(
                "Expected record count = %0d",
                expected_records
            );

        end


        if ($value$plusargs(
                "VERBOSE=%d",
                verbose_records
            )) begin

            $display(
                "Verbose record output = %0d",
                verbose_records
            );

        end


        // ========================================================
        // Open CSV file
        // ========================================================

        fd = $fopen(input_file, "r");

        if (fd == 0) begin

            $display("");
            $display("============================================");
            $display(" ERROR: COULD NOT OPEN INPUT FILE");
            $display("============================================");

            $display("File = %s", input_file);

            $display("============================================");

            $finish;

        end


        // ========================================================
        // Simulation header
        // ========================================================

        $display("");
        $display("============================================");
        $display(" POLARIS FPGA STREAMING PARSER TEST");
        $display("============================================");

        $display(
            "Input file        = %s",
            input_file
        );

        $display(
            "Verbose records   = %0d",
            verbose_records
        );

        $display("============================================");


        // ========================================================
        // Reset
        // ========================================================

        #30;

        rst = 0;


        // ========================================================
        // Skip CSV header
        // ========================================================

        c = $fgetc(fd);

        while ((c != -1) &&
               (c != 8'h0A)) begin

            header_bytes =
                header_bytes + 1;

            c = $fgetc(fd);

        end


        // Count newline as part of the header

        if (c == 8'h0A)

            header_bytes =
                header_bytes + 1;


        $display(
            "CSV header skipped = %0d bytes",
            header_bytes
        );


        // ========================================================
        // Stream CSV data
        //
        // Reads one byte directly from the input CSV.
        // No HEX conversion and no complete dataset loading.
        // ========================================================

        c = $fgetc(fd);

        while (c != -1) begin

            current_byte = c[7:0];

            send_byte(current_byte);

            bytes_processed =
                bytes_processed + 1;


            // ----------------------------------------------------
            // Newline completes one CSV record.
            //
            // Wait until the parser generates valid_record and
            // acknowledge it before continuing.
            // ----------------------------------------------------

            if (current_byte == 8'h0A) begin

                wait_for_record();

            end


            c = $fgetc(fd);

        end


        // ========================================================
        // Close input file
        // ========================================================

        $fclose(fd);


        // Allow final signals to settle

        #100;


        // ========================================================
        // Final results
        // ========================================================

        $display("");
        $display("============================================");
        $display(" POLARIS FPGA STREAMING TEST COMPLETE");
        $display("============================================");

        $display(
            "Bytes processed    = %0d",
            bytes_processed
        );

        $display(
            "Records detected   = %0d",
            records
        );

        $display(
            "Clock cycles       = %0d",
            cycle_count
        );


        // ========================================================
        // Record verification
        // ========================================================

        if (expected_records >= 0) begin

            $display(
                "Expected records   = %0d",
                expected_records
            );

            if (records == expected_records) begin

                $display(
                    "Record check      = PASS"
                );

            end

            else begin

                $display(
                    "Record check      = FAIL"
                );

            end

        end


        $display("============================================");


        $finish;

    end


endmodule
