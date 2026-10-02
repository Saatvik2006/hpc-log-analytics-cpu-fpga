`timescale 1ns/1ps

module eight_polaris_log_parser (

    input wire clk,
    input wire rst,

    // ========================================================
    // Parser 0
    // ========================================================

    input wire [7:0] ascii0_in,
    input wire valid0_in,
    output wire input0_ready,
    input wire ready0,
    output wire valid0_record,

    output wire [255:0] job_name0_out,
    output wire [255:0] queued_timestamp0_out,
    output wire [255:0] start_timestamp0_out,
    output wire [255:0] end_timestamp0_out,
    output wire [255:0] username0_out,

    output wire [63:0] runtime0_out,
    output wire [63:0] nodes_used0_out,
    output wire [63:0] nodes_requested0_out,
    output wire [63:0] cores_used0_out,
    output wire [63:0] cores_requested0_out,
    output wire [63:0] gpus_requested0_out,

    output wire [31:0] exit_status0_out,
    output wire [511:0] science_field0_out,


    // ========================================================
    // Parser 1
    // ========================================================

    input wire [7:0] ascii1_in,
    input wire valid1_in,
    output wire input1_ready,
    input wire ready1,
    output wire valid1_record,

    output wire [255:0] job_name1_out,
    output wire [255:0] queued_timestamp1_out,
    output wire [255:0] start_timestamp1_out,
    output wire [255:0] end_timestamp1_out,
    output wire [255:0] username1_out,

    output wire [63:0] runtime1_out,
    output wire [63:0] nodes_used1_out,
    output wire [63:0] nodes_requested1_out,
    output wire [63:0] cores_used1_out,
    output wire [63:0] cores_requested1_out,
    output wire [63:0] gpus_requested1_out,

    output wire [31:0] exit_status1_out,
    output wire [511:0] science_field1_out,


    // ========================================================
    // Parser 2
    // ========================================================

    input wire [7:0] ascii2_in,
    input wire valid2_in,
    output wire input2_ready,
    input wire ready2,
    output wire valid2_record,

    output wire [255:0] job_name2_out,
    output wire [255:0] queued_timestamp2_out,
    output wire [255:0] start_timestamp2_out,
    output wire [255:0] end_timestamp2_out,
    output wire [255:0] username2_out,

    output wire [63:0] runtime2_out,
    output wire [63:0] nodes_used2_out,
    output wire [63:0] nodes_requested2_out,
    output wire [63:0] cores_used2_out,
    output wire [63:0] cores_requested2_out,
    output wire [63:0] gpus_requested2_out,

    output wire [31:0] exit_status2_out,
    output wire [511:0] science_field2_out,


    // ========================================================
    // Parser 3
    // ========================================================

    input wire [7:0] ascii3_in,
    input wire valid3_in,
    output wire input3_ready,
    input wire ready3,
    output wire valid3_record,

    output wire [255:0] job_name3_out,
    output wire [255:0] queued_timestamp3_out,
    output wire [255:0] start_timestamp3_out,
    output wire [255:0] end_timestamp3_out,
    output wire [255:0] username3_out,

    output wire [63:0] runtime3_out,
    output wire [63:0] nodes_used3_out,
    output wire [63:0] nodes_requested3_out,
    output wire [63:0] cores_used3_out,
    output wire [63:0] cores_requested3_out,
    output wire [63:0] gpus_requested3_out,

    output wire [31:0] exit_status3_out,
    output wire [511:0] science_field3_out,


    // ========================================================
    // Parser 4
    // ========================================================

    input wire [7:0] ascii4_in,
    input wire valid4_in,
    output wire input4_ready,
    input wire ready4,
    output wire valid4_record,

    output wire [255:0] job_name4_out,
    output wire [255:0] queued_timestamp4_out,
    output wire [255:0] start_timestamp4_out,
    output wire [255:0] end_timestamp4_out,
    output wire [255:0] username4_out,

    output wire [63:0] runtime4_out,
    output wire [63:0] nodes_used4_out,
    output wire [63:0] nodes_requested4_out,
    output wire [63:0] cores_used4_out,
    output wire [63:0] cores_requested4_out,
    output wire [63:0] gpus_requested4_out,

    output wire [31:0] exit_status4_out,
    output wire [511:0] science_field4_out,


    // ========================================================
    // Parser 5
    // ========================================================

    input wire [7:0] ascii5_in,
    input wire valid5_in,
    output wire input5_ready,
    input wire ready5,
    output wire valid5_record,

    output wire [255:0] job_name5_out,
    output wire [255:0] queued_timestamp5_out,
    output wire [255:0] start_timestamp5_out,
    output wire [255:0] end_timestamp5_out,
    output wire [255:0] username5_out,

    output wire [63:0] runtime5_out,
    output wire [63:0] nodes_used5_out,
    output wire [63:0] nodes_requested5_out,
    output wire [63:0] cores_used5_out,
    output wire [63:0] cores_requested5_out,
    output wire [63:0] gpus_requested5_out,

    output wire [31:0] exit_status5_out,
    output wire [511:0] science_field5_out,


    // ========================================================
    // Parser 6
    // ========================================================

    input wire [7:0] ascii6_in,
    input wire valid6_in,
    output wire input6_ready,
    input wire ready6,
    output wire valid6_record,

    output wire [255:0] job_name6_out,
    output wire [255:0] queued_timestamp6_out,
    output wire [255:0] start_timestamp6_out,
    output wire [255:0] end_timestamp6_out,
    output wire [255:0] username6_out,

    output wire [63:0] runtime6_out,
    output wire [63:0] nodes_used6_out,
    output wire [63:0] nodes_requested6_out,
    output wire [63:0] cores_used6_out,
    output wire [63:0] cores_requested6_out,
    output wire [63:0] gpus_requested6_out,

    output wire [31:0] exit_status6_out,
    output wire [511:0] science_field6_out,


    // ========================================================
    // Parser 7
    // ========================================================

    input wire [7:0] ascii7_in,
    input wire valid7_in,
    output wire input7_ready,
    input wire ready7,
    output wire valid7_record,

    output wire [255:0] job_name7_out,
    output wire [255:0] queued_timestamp7_out,
    output wire [255:0] start_timestamp7_out,
    output wire [255:0] end_timestamp7_out,
    output wire [255:0] username7_out,

    output wire [63:0] runtime7_out,
    output wire [63:0] nodes_used7_out,
    output wire [63:0] nodes_requested7_out,
    output wire [63:0] cores_used7_out,
    output wire [63:0] cores_requested7_out,
    output wire [63:0] gpus_requested7_out,

    output wire [31:0] exit_status7_out,
    output wire [511:0] science_field7_out
);


    // ============================================================
    // Parser 0
    // ============================================================

    polaris_log_parser parser0 (

        .clk(clk),
        .rst(rst),

        .ascii_in(ascii0_in),
        .valid_in(valid0_in),
        .input_ready(input0_ready),
        .ready(ready0),

        .valid_record(valid0_record),

        .job_name_out(job_name0_out),
        .queued_timestamp_out(queued_timestamp0_out),
        .start_timestamp_out(start_timestamp0_out),
        .end_timestamp_out(end_timestamp0_out),
        .username_out(username0_out),

        .runtime_out(runtime0_out),
        .nodes_used_out(nodes_used0_out),
        .nodes_requested_out(nodes_requested0_out),
        .cores_used_out(cores_used0_out),
        .cores_requested_out(cores_requested0_out),
        .gpus_requested_out(gpus_requested0_out),

        .exit_status_out(exit_status0_out),
        .science_field_out(science_field0_out)
    );


    // ============================================================
    // Parser 1
    // ============================================================

    polaris_log_parser parser1 (

        .clk(clk),
        .rst(rst),

        .ascii_in(ascii1_in),
        .valid_in(valid1_in),
        .input_ready(input1_ready),
        .ready(ready1),

        .valid_record(valid1_record),

        .job_name_out(job_name1_out),
        .queued_timestamp_out(queued_timestamp1_out),
        .start_timestamp_out(start_timestamp1_out),
        .end_timestamp_out(end_timestamp1_out),
        .username_out(username1_out),

        .runtime_out(runtime1_out),
        .nodes_used_out(nodes_used1_out),
        .nodes_requested_out(nodes_requested1_out),
        .cores_used_out(cores_used1_out),
        .cores_requested_out(cores_requested1_out),
        .gpus_requested_out(gpus_requested1_out),

        .exit_status_out(exit_status1_out),
        .science_field_out(science_field1_out)
    );


    // ============================================================
    // Parser 2
    // ============================================================

    polaris_log_parser parser2 (

        .clk(clk),
        .rst(rst),

        .ascii_in(ascii2_in),
        .valid_in(valid2_in),
        .input_ready(input2_ready),
        .ready(ready2),

        .valid_record(valid2_record),

        .job_name_out(job_name2_out),
        .queued_timestamp_out(queued_timestamp2_out),
        .start_timestamp_out(start_timestamp2_out),
        .end_timestamp_out(end_timestamp2_out),
        .username_out(username2_out),

        .runtime_out(runtime2_out),
        .nodes_used_out(nodes_used2_out),
        .nodes_requested_out(nodes_requested2_out),
        .cores_used_out(cores_used2_out),
        .cores_requested_out(cores_requested2_out),
        .gpus_requested_out(gpus_requested2_out),

        .exit_status_out(exit_status2_out),
        .science_field_out(science_field2_out)
    );


    // ============================================================
    // Parser 3
    // ============================================================

    polaris_log_parser parser3 (

        .clk(clk),
        .rst(rst),

        .ascii_in(ascii3_in),
        .valid_in(valid3_in),
        .input_ready(input3_ready),
        .ready(ready3),

        .valid_record(valid3_record),

        .job_name_out(job_name3_out),
        .queued_timestamp_out(queued_timestamp3_out),
        .start_timestamp_out(start_timestamp3_out),
        .end_timestamp_out(end_timestamp3_out),
        .username_out(username3_out),

        .runtime_out(runtime3_out),
        .nodes_used_out(nodes_used3_out),
        .nodes_requested_out(nodes_requested3_out),
        .cores_used_out(cores_used3_out),
        .cores_requested_out(cores_requested3_out),
        .gpus_requested_out(gpus_requested3_out),

        .exit_status_out(exit_status3_out),
        .science_field_out(science_field3_out)
    );


    // ============================================================
    // Parser 4
    // ============================================================

    polaris_log_parser parser4 (

        .clk(clk),
        .rst(rst),

        .ascii_in(ascii4_in),
        .valid_in(valid4_in),
        .input_ready(input4_ready),
        .ready(ready4),

        .valid_record(valid4_record),

        .job_name_out(job_name4_out),
        .queued_timestamp_out(queued_timestamp4_out),
        .start_timestamp_out(start_timestamp4_out),
        .end_timestamp_out(end_timestamp4_out),
        .username_out(username4_out),

        .runtime_out(runtime4_out),
        .nodes_used_out(nodes_used4_out),
        .nodes_requested_out(nodes_requested4_out),
        .cores_used_out(cores_used4_out),
        .cores_requested_out(cores_requested4_out),
        .gpus_requested_out(gpus_requested4_out),

        .exit_status_out(exit_status4_out),
        .science_field_out(science_field4_out)
    );


    // ============================================================
    // Parser 5
    // ============================================================

    polaris_log_parser parser5 (

        .clk(clk),
        .rst(rst),

        .ascii_in(ascii5_in),
        .valid_in(valid5_in),
        .input_ready(input5_ready),
        .ready(ready5),

        .valid_record(valid5_record),

        .job_name_out(job_name5_out),
        .queued_timestamp_out(queued_timestamp5_out),
        .start_timestamp_out(start_timestamp5_out),
        .end_timestamp_out(end_timestamp5_out),
        .username_out(username5_out),

        .runtime_out(runtime5_out),
        .nodes_used_out(nodes_used5_out),
        .nodes_requested_out(nodes_requested5_out),
        .cores_used_out(cores_used5_out),
        .cores_requested_out(cores_requested5_out),
        .gpus_requested_out(gpus_requested5_out),

        .exit_status_out(exit_status5_out),
        .science_field_out(science_field5_out)
    );


    // ============================================================
    // Parser 6
    // ============================================================

    polaris_log_parser parser6 (

        .clk(clk),
        .rst(rst),

        .ascii_in(ascii6_in),
        .valid_in(valid6_in),
        .input_ready(input6_ready),
        .ready(ready6),

        .valid_record(valid6_record),

        .job_name_out(job_name6_out),
        .queued_timestamp_out(queued_timestamp6_out),
        .start_timestamp_out(start_timestamp6_out),
        .end_timestamp_out(end_timestamp6_out),
        .username_out(username6_out),

        .runtime_out(runtime6_out),
        .nodes_used_out(nodes_used6_out),
        .nodes_requested_out(nodes_requested6_out),
        .cores_used_out(cores_used6_out),
        .cores_requested_out(cores_requested6_out),
        .gpus_requested_out(gpus_requested6_out),

        .exit_status_out(exit_status6_out),
        .science_field_out(science_field6_out)
    );


    // ============================================================
    // Parser 7
    // ============================================================

    polaris_log_parser parser7 (

        .clk(clk),
        .rst(rst),

        .ascii_in(ascii7_in),
        .valid_in(valid7_in),
        .input_ready(input7_ready),
        .ready(ready7),

        .valid_record(valid7_record),

        .job_name_out(job_name7_out),
        .queued_timestamp_out(queued_timestamp7_out),
        .start_timestamp_out(start_timestamp7_out),
        .end_timestamp_out(end_timestamp7_out),
        .username_out(username7_out),

        .runtime_out(runtime7_out),
        .nodes_used_out(nodes_used7_out),
        .nodes_requested_out(nodes_requested7_out),
        .cores_used_out(cores_used7_out),
        .cores_requested_out(cores_requested7_out),
        .gpus_requested_out(gpus_requested7_out),

        .exit_status_out(exit_status7_out),
        .science_field_out(science_field7_out)
    );

endmodule