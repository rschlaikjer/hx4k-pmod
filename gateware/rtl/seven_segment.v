`define CLK_HZ 48_000_000

module seven_segment(
    input wire i_clk,
    input wire [7:0] i_data,
    output reg [6:0] o_segments,
    output reg o_digit
);

// Create two modules to mux from a nibble to a hex character,
// one for the ones place and one for the tens place
wire [6:0] ones_segments;
wire [6:0] tens_segments;
nibble_to_digit nibble_ones(i_clk, i_data[3:0], ones_segments);
nibble_to_digit nibble_tens(i_clk, i_data[7:4], tens_segments);

// The icebreaker 7-segment addon muxes two 7-segment displays with a digit
// select pin, so we need to alternate between which segment is being displayed.
reg [3:0] display_state;
localparam PRESCALER_1KHZ = (`CLK_HZ / 1000) - 1;
reg [$clog2(PRESCALER_1KHZ):0] prescaler = 0;
always @(posedge i_clk) begin
    if (prescaler == 0) begin
        display_state <= display_state + 1;
        case (display_state)
            0, 1, 2: begin
                o_digit <= 1;
                o_segments <= ~ones_segments;
            end
            3: o_segments <= ~0;
            4, 5, 6: begin
                o_digit <= 0;
                o_segments <= ~tens_segments;
            end
            7: o_segments <= ~0;
        endcase
    end else
        prescaler <= prescaler - 1;
end

endmodule

// To avoid duplicating this, create a little submodule with just the
// mapping between a nibble and the segments used to display it.
// Segments are considered active high here.
// Most significant bit is segment 'G', lsb is segment 'A'.
module nibble_to_digit(input wire i_clk,
                       input wire [3:0] i_nibble,
                       output reg [6:0] o_segments);
    always @(posedge i_clk) begin
        case (i_nibble)
            0: o_segments <= 7'b0111111;
            1: o_segments <= 7'b0000110;
            2: o_segments <= 7'b1011011;
            3: o_segments <= 7'b1001111;
            4: o_segments <= 7'b1100110;
            5: o_segments <= 7'b1101101;
            6: o_segments <= 7'b1111101;
            7: o_segments <= 7'b0000111;
            8: o_segments <= 7'b1111111;
            9: o_segments <= 7'b1101111;
            4'hA: o_segments <= 7'b1110111;
            4'hB: o_segments <= 7'b1111100;
            4'hC: o_segments <= 7'b0111001;
            4'hD: o_segments <= 7'b1011110;
            4'hE: o_segments <= 7'b1111001;
            4'hF: o_segments <= 7'b1110001;
        endcase
    end
endmodule
