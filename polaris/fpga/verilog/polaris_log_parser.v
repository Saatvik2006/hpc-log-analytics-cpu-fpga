`timescale 1ns / 1ps

module polaris_log_parser (

    input  wire        clk,
    input  wire        rst,

    input  wire [7:0]  ascii_in,
    input  wire        valid_in,
    input  wire        ready,

    output reg         valid_record,
    output wire        input_ready,

    output reg [255:0] job_name_out,
    output reg [255:0] queued_timestamp_out,
    output reg [255:0] start_timestamp_out,
    output reg [255:0] end_timestamp_out,
    output reg [255:0] username_out,

    output reg [63:0]  runtime_out,
    output reg [63:0]  nodes_used_out,
    output reg [63:0]  nodes_requested_out,
    output reg [63:0]  cores_used_out,
    output reg [63:0]  cores_requested_out,
    output reg [63:0]  gpus_requested_out,

    output reg [31:0]  exit_status_out,

    output reg [511:0] science_field_out
);

    // ============================================================
    // Polaris CSV column numbers
    // ============================================================

    localparam FIELD_JOB_NAME        = 1;
    localparam FIELD_QUEUED_TS       = 4;
    localparam FIELD_START_TS        = 6;
    localparam FIELD_END_TS          = 8;
    localparam FIELD_USERNAME        = 10;
    localparam FIELD_RUNTIME         = 15;
    localparam FIELD_NODES_USED      = 16;
    localparam FIELD_NODES_REQUESTED = 17;
    localparam FIELD_CORES_USED      = 18;
    localparam FIELD_CORES_REQUESTED = 19;
    localparam FIELD_EXIT_STATUS     = 21;
    localparam FIELD_GPUS_REQUESTED  = 59;
    localparam FIELD_SCIENCE_FIELD   = 65;


    // ============================================================
    // Parser states
    // ============================================================

    localparam STATE_IDLE  = 2'd0;
    localparam STATE_PARSE = 2'd1;
    localparam STATE_DONE  = 2'd2;

    reg [1:0] state;

    // ============================================================
    // Input ready
    //
    // The parser can accept a new input byte in IDLE or PARSE.
    // While in DONE, it is holding a completed record and must
    // not consume another input byte.
    // ============================================================
    
    assign input_ready = (state != STATE_DONE);


    // ============================================================
    // Current CSV field number
    // ============================================================

    reg [6:0] field_number;


    // ============================================================
    // CSV quote tracking
    // ============================================================

    reg in_quotes;


    // ============================================================
    // Character indices
    // ============================================================

    reg [5:0] job_index;
    reg [5:0] queued_index;
    reg [5:0] start_index;
    reg [5:0] end_index;
    reg [5:0] username_index;

    reg [5:0] runtime_index;
    reg [5:0] nodes_used_index;
    reg [5:0] nodes_requested_index;
    reg [5:0] cores_used_index;
    reg [5:0] cores_requested_index;
    reg [5:0] gpus_index;

    reg [5:0] exit_index;

    reg [6:0] science_index;


    // ============================================================
    // Internal field storage
    // ============================================================

    reg [255:0] job_name;
    reg [255:0] queued_timestamp;
    reg [255:0] start_timestamp;
    reg [255:0] end_timestamp;
    reg [255:0] username;

    reg [63:0] runtime;
    reg [63:0] nodes_used;
    reg [63:0] nodes_requested;
    reg [63:0] cores_used;
    reg [63:0] cores_requested;
    reg [63:0] gpus_requested;

    reg [31:0] exit_status;

    reg [511:0] science_field;


    // ============================================================
    // Main parser
    // ============================================================

    always @(posedge clk) begin

        // ========================================================
        // RESET
        // ========================================================

        if (rst) begin

            state <= STATE_IDLE;

            field_number <= 0;
            in_quotes <= 0;

            job_index <= 0;
            queued_index <= 0;
            start_index <= 0;
            end_index <= 0;
            username_index <= 0;

            runtime_index <= 0;
            nodes_used_index <= 0;
            nodes_requested_index <= 0;
            cores_used_index <= 0;
            cores_requested_index <= 0;
            gpus_index <= 0;

            exit_index <= 0;
            science_index <= 0;


            // Internal storage

            job_name <= 0;
            queued_timestamp <= 0;
            start_timestamp <= 0;
            end_timestamp <= 0;
            username <= 0;

            runtime <= 0;
            nodes_used <= 0;
            nodes_requested <= 0;
            cores_used <= 0;
            cores_requested <= 0;
            gpus_requested <= 0;

            exit_status <= 0;

            science_field <= 0;


            // Outputs

            valid_record <= 0;

            job_name_out <= 0;
            queued_timestamp_out <= 0;
            start_timestamp_out <= 0;
            end_timestamp_out <= 0;
            username_out <= 0;

            runtime_out <= 0;
            nodes_used_out <= 0;
            nodes_requested_out <= 0;
            cores_used_out <= 0;
            cores_requested_out <= 0;
            gpus_requested_out <= 0;

            exit_status_out <= 0;

            science_field_out <= 0;

        end

        else begin

            case (state)


                // =================================================
                // STATE_IDLE
                //
                // Wait for first character of next record
                // =================================================

                STATE_IDLE: begin

                    valid_record <= 0;

                    if (valid_in) begin

                        // -----------------------------------------
                        // Initialize new record
                        // -----------------------------------------

                        field_number <= 1;
                        in_quotes <= 0;

                        job_index <= 0;
                        queued_index <= 0;
                        start_index <= 0;
                        end_index <= 0;
                        username_index <= 0;

                        runtime_index <= 0;
                        nodes_used_index <= 0;
                        nodes_requested_index <= 0;
                        cores_used_index <= 0;
                        cores_requested_index <= 0;
                        gpus_index <= 0;

                        exit_index <= 0;
                        science_index <= 0;


                        job_name <= 0;
                        queued_timestamp <= 0;
                        start_timestamp <= 0;
                        end_timestamp <= 0;
                        username <= 0;

                        runtime <= 0;
                        nodes_used <= 0;
                        nodes_requested <= 0;
                        cores_used <= 0;
                        cores_requested <= 0;
                        gpus_requested <= 0;

                        exit_status <= 0;
                        science_field <= 0;


                        state <= STATE_PARSE;


                        // -----------------------------------------
                        // Process first character
                        // -----------------------------------------

                        if (ascii_in == 8'h22) begin

                            // Opening quote

                            in_quotes <= 1'b1;

                        end

                        else if (ascii_in == 8'h2C) begin

                            // Empty first field

                            field_number <= 2;

                        end

                        else if (ascii_in == 8'h0A) begin

                            // Empty record

                            state <= STATE_DONE;

                        end

                        else if (ascii_in != 8'h0D) begin

                            // Store first character of JOB_NAME
                            // First character goes into MSB side

                            job_name[255 -: 8] <= ascii_in;

                            job_index <= 1;

                        end

                    end

                end


                // =================================================
                // STATE_PARSE
                //
                // Parse incoming CSV characters
                // =================================================

                STATE_PARSE: begin

                    if (valid_in) begin


                        // -----------------------------------------
                        // Quote handling
                        // -----------------------------------------

                        if (ascii_in == 8'h22) begin

                            in_quotes <= ~in_quotes;

                        end


                        // -----------------------------------------
                        // CSV field delimiter
                        // -----------------------------------------

                        else if (
                            (ascii_in == 8'h2C) &&
                            !in_quotes
                        ) begin

                            field_number <= field_number + 1;


                            // Reset character counters

                            job_index <= 0;
                            queued_index <= 0;
                            start_index <= 0;
                            end_index <= 0;
                            username_index <= 0;

                            runtime_index <= 0;
                            nodes_used_index <= 0;
                            nodes_requested_index <= 0;
                            cores_used_index <= 0;
                            cores_requested_index <= 0;
                            gpus_index <= 0;

                            exit_index <= 0;
                            science_index <= 0;

                        end


                        // -----------------------------------------
                        // End of CSV record
                        // -----------------------------------------

                        else if (
                            (ascii_in == 8'h0A) &&
                            !in_quotes
                        ) begin

                            state <= STATE_DONE;

                        end


                        // -----------------------------------------
                        // Ignore carriage return
                        // -----------------------------------------

                        else if (ascii_in == 8'h0D) begin

                        end


                        // -----------------------------------------
                        // Store selected field
                        // -----------------------------------------

                        else begin

                            case (field_number)


                                // =============================
                                // JOB_NAME
                                // =============================

                                FIELD_JOB_NAME: begin

                                    if (job_index < 32) begin

                                        job_name[
                                            255 - job_index*8 -: 8
                                        ] <= ascii_in;

                                        job_index <=
                                            job_index + 1;

                                    end

                                end


                                // =============================
                                // QUEUED_TIMESTAMP
                                // =============================

                                FIELD_QUEUED_TS: begin

                                    if (queued_index < 32) begin

                                        queued_timestamp[
                                            255 - queued_index*8 -: 8
                                        ] <= ascii_in;

                                        queued_index <=
                                            queued_index + 1;

                                    end

                                end


                                // =============================
                                // START_TIMESTAMP
                                // =============================

                                FIELD_START_TS: begin

                                    if (start_index < 32) begin

                                        start_timestamp[
                                            255 - start_index*8 -: 8
                                        ] <= ascii_in;

                                        start_index <=
                                            start_index + 1;

                                    end

                                end


                                // =============================
                                // END_TIMESTAMP
                                // =============================

                                FIELD_END_TS: begin

                                    if (end_index < 32) begin

                                        end_timestamp[
                                            255 - end_index*8 -: 8
                                        ] <= ascii_in;

                                        end_index <=
                                            end_index + 1;

                                    end

                                end


                                // =============================
                                // USERNAME_GENID
                                // =============================

                                FIELD_USERNAME: begin

                                    if (username_index < 32) begin

                                        username[
                                            255 - username_index*8 -: 8
                                        ] <= ascii_in;

                                        username_index <=
                                            username_index + 1;

                                    end

                                end


                                // =============================
                                // RUNTIME_SECONDS
                                // =============================

                                FIELD_RUNTIME: begin

                                    if (runtime_index < 8) begin

                                        runtime[
                                            63 - runtime_index*8 -: 8
                                        ] <= ascii_in;

                                        runtime_index <=
                                            runtime_index + 1;

                                    end

                                end


                                // =============================
                                // NODES_USED
                                // =============================

                                FIELD_NODES_USED: begin

                                    if (nodes_used_index < 8) begin

                                        nodes_used[
                                            63 - nodes_used_index*8 -: 8
                                        ] <= ascii_in;

                                        nodes_used_index <=
                                            nodes_used_index + 1;

                                    end

                                end


                                // =============================
                                // NODES_REQUESTED
                                // =============================

                                FIELD_NODES_REQUESTED: begin

                                    if (nodes_requested_index < 8) begin

                                        nodes_requested[
                                            63 - nodes_requested_index*8 -: 8
                                        ] <= ascii_in;

                                        nodes_requested_index <=
                                            nodes_requested_index + 1;

                                    end

                                end


                                // =============================
                                // CORES_USED
                                // =============================

                                FIELD_CORES_USED: begin

                                    if (cores_used_index < 8) begin

                                        cores_used[
                                            63 - cores_used_index*8 -: 8
                                        ] <= ascii_in;

                                        cores_used_index <=
                                            cores_used_index + 1;

                                    end

                                end


                                // =============================
                                // CORES_REQUESTED
                                // =============================

                                FIELD_CORES_REQUESTED: begin

                                    if (cores_requested_index < 8) begin

                                        cores_requested[
                                            63 - cores_requested_index*8 -: 8
                                        ] <= ascii_in;

                                        cores_requested_index <=
                                            cores_requested_index + 1;

                                    end

                                end


                                // =============================
                                // EXIT_STATUS
                                // =============================

                                FIELD_EXIT_STATUS: begin

                                    if (exit_index < 4) begin

                                        exit_status[
                                            31 - exit_index*8 -: 8
                                        ] <= ascii_in;

                                        exit_index <=
                                            exit_index + 1;

                                    end

                                end


                                // =============================
                                // GPUS_REQUESTED
                                // =============================

                                FIELD_GPUS_REQUESTED: begin

                                    if (gpus_index < 8) begin

                                        gpus_requested[
                                            63 - gpus_index*8 -: 8
                                        ] <= ascii_in;

                                        gpus_index <=
                                            gpus_index + 1;

                                    end

                                end


                                // =============================
                                // SCIENCE_FIELD
                                // =============================

                                FIELD_SCIENCE_FIELD: begin

                                    if (science_index < 64) begin

                                        science_field[
                                            511 - science_index*8 -: 8
                                        ] <= ascii_in;

                                        science_index <=
                                            science_index + 1;

                                    end

                                end


                                default: begin

                                end

                            endcase

                        end

                    end

                end


                // =================================================
                // STATE_DONE
                //
                // Hold completed record until READY
                // =================================================

                STATE_DONE: begin

                    valid_record <= 1'b1;


                    // ---------------------------------------------
                    // Copy parsed fields to outputs
                    // ---------------------------------------------

                    job_name_out <= job_name;

                    queued_timestamp_out <=
                        queued_timestamp;

                    start_timestamp_out <=
                        start_timestamp;

                    end_timestamp_out <=
                        end_timestamp;

                    username_out <= username;


                    runtime_out <= runtime;

                    nodes_used_out <= nodes_used;

                    nodes_requested_out <=
                        nodes_requested;

                    cores_used_out <= cores_used;

                    cores_requested_out <=
                        cores_requested;

                    gpus_requested_out <=
                        gpus_requested;


                    exit_status_out <=
                        exit_status;


                    science_field_out <=
                        science_field;


                    // ---------------------------------------------
                    // Consumer acknowledges record
                    // ---------------------------------------------

                    if (ready) begin

                        valid_record <= 1'b0;

                        state <= STATE_IDLE;

                    end

                end


                // =================================================
                // Default recovery
                // =================================================

                default: begin

                    state <= STATE_IDLE;

                end

            endcase

        end

    end

endmodule
