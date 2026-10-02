`timescale 1ns/1ps

module dual_polaris_log_parser (
    input wire clk,
    input wire rst,

    // Parser 0 input
    input wire [7:0] ascii0_in,
    input wire       valid0_in,
    input wire       ready0,

    // Parser 1 input
    input wire [7:0] ascii1_in,
    input wire       valid1_in,
    input wire       ready1,

    // Parser 0 output
    output wire       valid0_record,

    output wire [255:0] job_name0_out,
    output wire [255:0] queued_timestamp0_out,
    output wire [255:0] start_timestamp0_out,
    output wire [255:0] end_timestamp0_out,
    output wire [255:0] username0_out,

    output wire [511:0] science_field0_out,

    output wire [63:0] runtime0_out,
    output wire [63:0] nodes_used0_out,
    output wire [63:0] nodes_requested0_out,
    output wire [63:0] cores_used0_out,
    output wire [63:0] cores_requested0_out,
    output wire [63:0] gpus_requested0_out,
    output wire [31:0] exit_status0_out,

    // Parser 1 output
    output wire       valid1_record,

    output wire [255:0] job_name1_out,
    output wire [255:0] queued_timestamp1_out,
    output wire [255:0] start_timestamp1_out,
    output wire [255:0] end_timestamp1_out,
    output wire [255:0] username1_out,

    output wire [511:0] science_field1_out,

    output wire [63:0] runtime1_out,
    output wire [63:0] nodes_used1_out,
    output wire [63:0] nodes_requested1_out,
    output wire [63:0] cores_used1_out,
    output wire [63:0] cores_requested1_out,
    output wire [63:0] gpus_requested1_out,
    output wire [31:0] exit_status1_out
);

    // --------------------------------------------------------
    // Parser 0
    // --------------------------------------------------------

    polaris_log_parser parser0 (
        .clk(clk),
        .rst(rst),

        .ascii_in(ascii0_in),
        .valid_in(valid0_in),
        .ready(ready0),

        .valid_record(valid0_record),

        .job_name_out(job_name0_out),
        .queued_timestamp_out(queued_timestamp0_out),
        .start_timestamp_out(start_timestamp0_out),
        .end_timestamp_out(end_timestamp0_out),
        .username_out(username0_out),

        .science_field_out(science_field0_out),

        .runtime_out(runtime0_out),
        .nodes_used_out(nodes_used0_out),
        .nodes_requested_out(nodes_requested0_out),
        .cores_used_out(cores_used0_out),
        .cores_requested_out(cores_requested0_out),
        .gpus_requested_out(gpus_requested0_out),
        .exit_status_out(exit_status0_out)
    );

    // --------------------------------------------------------
    // Parser 1
    // --------------------------------------------------------

    polaris_log_parser parser1 (
        .clk(clk),
        .rst(rst),

        .ascii_in(ascii1_in),
        .valid_in(valid1_in),
        .ready(ready1),

        .valid_record(valid1_record),

        .job_name_out(job_name1_out),
        .queued_timestamp_out(queued_timestamp1_out),
        .start_timestamp_out(start_timestamp1_out),
        .end_timestamp_out(end_timestamp1_out),
        .username_out(username1_out),

        .science_field_out(science_field1_out),

        .runtime_out(runtime1_out),
        .nodes_used_out(nodes_used1_out),
        .nodes_requested_out(nodes_requested1_out),
        .cores_used_out(cores_used1_out),
        .cores_requested_out(cores_requested1_out),
        .gpus_requested_out(gpus_requested1_out),
        .exit_status_out(exit_status1_out)
    );

endmodule
