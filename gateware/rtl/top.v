`default_nettype none

`define CLK_HZ 48_000_000

module top(
        // Input clock
        input wire CLK_12MHZ,

        // RGB LEDs
        output wire LED_R,
        output wire LED_G,
        output wire LED_B,

        // PMOD C1
        output wire PMOD_C1_D0,
        output wire PMOD_C1_D1,
        output wire PMOD_C1_D2,
        output wire PMOD_C1_D3,
        output wire PMOD_C1_D4,
        output wire PMOD_C1_D5,
        output wire PMOD_C1_D6,
        output wire PMOD_C1_D7
    );

    // PLL
    wire clk_48mhz;
    wire pll_lock;
    ice_pll pll(CLK_12MHZ, clk_48mhz, pll_lock);

    // 7-segment counter
    reg [7:0] seven_seg_counter = 0;
    wire [6:0] segments;
    assign {
        PMOD_C1_D6, // G
        PMOD_C1_D5, // F
        PMOD_C1_D4, // E
        PMOD_C1_D3, // D
        PMOD_C1_D2, // C
        PMOD_C1_D1, // B
        PMOD_C1_D0  // A
    } = segments;

    // Byte to 7-segment converter
    seven_segment ss1(
        .i_clk(clk_48mhz),
        .i_data(seven_seg_counter),
        .o_segments(segments),
        .o_digit(PMOD_C1_D7)
    );

    // State counter for RGB LEDs
    reg [2:0] led_state;
    assign {LED_R, LED_G, LED_B} = led_state;

    // Twice a second, count up the seven segment display and change
    // the RGB LED state
    localparam PRESCALER_2HZ = (`CLK_HZ / 2) - 1;
    reg [$clog2(PRESCALER_2HZ):0] prescale_counter = 0;
    always @(posedge clk_48mhz) begin
        if (prescale_counter == 0) begin
            prescale_counter <= PRESCALER_2HZ;
            led_state <= led_state + 1;
            seven_seg_counter <= seven_seg_counter + 1;
        end else
            prescale_counter <= prescale_counter - 1;
    end

endmodule
