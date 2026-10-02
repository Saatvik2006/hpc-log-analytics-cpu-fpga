module quad_polaris_log_parser (

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
    output wire [511:0] science_field3_out
);


    // ========================================================
    // Parser 0
    // ========================================================
    polaris_log_parser parser0 (
        .clk(clk),
        .rst(rst),

        .ascii_in(ascii0_in),
        .valid_in(valid0_in),
        .ready(ready0),
        .input_ready(input0_ready),

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


    // ========================================================
    // Parser 1
    // ========================================================
    polaris_log_parser parser1 (
        .clk(clk),
        .rst(rst),

        .ascii_in(ascii1_in),
        .valid_in(valid1_in),
        .ready(ready1),
        .input_ready(input1_ready),

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


    // ========================================================
    // Parser 2
    // ========================================================
    polaris_log_parser parser2 (
        .clk(clk),
        .rst(rst),

        .ascii_in(ascii2_in),
        .valid_in(valid2_in),
        .ready(ready2),
        .input_ready(input2_ready),

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


    // ========================================================
    // Parser 3
    // ========================================================
    polaris_log_parser parser3 (
        .clk(clk),
        .rst(rst),

        .ascii_in(ascii3_in),
        .valid_in(valid3_in),
        .ready(ready3),
        .input_ready(input3_ready),

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

endmodule
