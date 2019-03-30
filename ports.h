/** @file ports.h
 *  @brief Port declaration function prototypes and macro declarations
 *
 *  Declaration of each of the port select bit macros and their related
 *  port initialization prototypes. There are a total of 7 ports with a
 *  differing number of pins on each port.
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef ADC_INTERRUPT_HEADER_H
#define ADC_INTERRUPT_HEADER_H

/**
 * @brief General function to call all of the port initializations
 *
 * Port 1 pins:
 * pin 0: RED_LED       (GPIO) (OUTPUT) (HIGH)
 * pin 1: A1_SEEED      (ADC)
 * pin 2: V_DETECT_L    (ADC)
 * pin 3: V_DETECT_R    (ADC)
 * pin 4: A4_SEEED      (ADC)
 * pin 5: V_THUMB       (ADC)
 * pin 6: UCA0RXD       (FUNCTION)
 * pin 7: UCA0TXD       (FUNCTION)
 *
 * Port 2 pins:
 * pin 0: P2_0          (GPIO)
 * pin 1: P2_1          (GPIO)
 * pin 2: P2_2          (GPIO)
 * pin 3: SW2           (GPIO) (INPUT) (PULL_UP) (INTERRUPT)
 * pin 4: P2_4          (GPIO)
 * pin 5: P2_5          (GPIO)
 * pin 6: LFXOUT        (FUNCTION)
 * pin 7: LFXIN         (FUNCTION)
 *
 * Port 3 pins:
 * pin 0: TEST_PROBE    (GPIO)
 * pin 1: OA20          (FUNCTION)
 * pin 2: OA2N          (FUNCTION)
 * pin 3: OA2P          (FUNCTION)
 * pin 4: SMCLK_OUT     (GPIO) or (FUNCTION) // Based on paramerter input
 * pin 5: OA3O          (FUNCTION)
 * pin 6: P3_6          (GPIO)
 * pin 7: P3_7          (GPIO)
 *
 * Port 4 pins:
 * pin 0: RESET_LCD     (GPIO) (OUTPUT) (LOW)
 * pin 1: SW1           (GPIO) (INPUT) (PULL_UP) (INTERRUPT)
 * pin 2: UCA1RXD       (FUNCTION)
 * pin 3: UCA1TXD       (FUNCTION)
 * pin 4: UCB1_CS_LCD   (GPIO) (OUTPUT) (HIGH)
 * pin 5: UCB1CLK       (FUNCTION)
 * pin 6: UCB1SIMO      (FUNCTION)
 * pin 7: UCB1SOMI      (FUNCTION)
 *
 * Port 5 pins:
 * pin 0: IOT_RESET     (FUNCTION)
 * pin 1: IOT_LINK      (FUNCTION)
 * pin 2: IOT_PROG_SEL  (FUNCTION)
 * pin 3: IOT_PROG_MODE (FUNCTION)
 * pin 4: IR_LED        (GPIO) (OUTPUT) (LOW)
 *
 * Port 6 pins:
 * pin 0: R_FORWARD     (FUNCTION)
 * pin 1: L_FORWARD     (FUNCTION)
 * pin 2: R_REVERSE     (FUNCTION)
 * pin 3: L_REVERSE     (FUNCTION)
 * pin 4: LCD_BACKLITE  (GPIO) (OUTPUT) (HIGH)
 * pin 5: P6_5          (GPIO)
 * pin 6: GRN_LED       (GPIO) (OUTPUT) (LOW)
 */
void init_ports(void);

/**
 * @brief Sets the SMCLK select bit mode
 *
 * @param smclk_mode is a boolean flag. 0 sets it GPIO and 1 sets it
 * to FUNCTION
 */
void set_smclk_mode(int smclk_mode);

/**
 * @brief All possible states for the smclk input
 *
 * Three channels are defined:
 * 1. GPIO: represents an initialization of 00
 * 2. FUNCTION: represents an initialization of 01
 * 3. INVALID_PIN_STATE: invalid pin select bit state
 */
typedef enum smclk_state_type { GPIO, FUNCTION, INVALID_PIN_STATE } smclk_state;

// Register initializaiton macros
#define INIT_OUTPUT (0xFF)
#define INIT_LOW (0x00)

// Port 1 Definitions
#define RED_LED (0x01)     // pin 0
#define A1_SEEED (0x02)    // pin 1
#define V_DETECT_L (0x04)  // pin 2
#define V_DETECT_R (0x08)  // pin 3
#define A4_SEEED (0x10)    // pin 4
#define V_THUMB (0x20)     // pin 5
#define UCA0RXD (0x40)     // pin 6
#define UCA0TXD (0x80)     // pin 7

// Port 2 Definitions
#define P2_0 (0x01)    // pin 0
#define P2_1 (0x02)    // pin 1
#define P2_2 (0x04)    // pin 2
#define SW2 (0x08)     // pin 3
#define P2_4 (0x10)    // pin 4
#define P2_5 (0x20)    // pin 5
#define LFXOUT (0x40)  // pin 6
#define LFXIN (0x80)   // pin 7

// Port 3 Definitions
#define TEST_PROBE (0x01)  // pin 0
#define OA20 (0x02)        // pin 1
#define OA2N (0x04)        // pin 2
#define OA2P (0x08)        // pin 3
#define SMCLK_OUT (0x10)   // pin 4
#define OA3O (0x20)        // pin 5
#define P3_6 (0x40)        // pin 6
#define P3_7 (0x80)        // pin 7

// Port 4 Definitions
#define RESET_LCD (0x01)    // pin 0
#define SW1 (0x02)          // pin 1
#define UCA1RXD (0x04)      // pin 2
#define UCA1TXD (0x08)      // pin 3
#define UCB1_CS_LCD (0x10)  // pin 4
#define UCB1CLK (0x20)      // pin 5
#define UCB1SIMO (0x40)     // pin 6
#define UCB1SOMI (0x80)     // pin 7

// Port 5 Definitions
#define IOT_RESET (0x01)      // pin 0
#define IOT_LINK (0x02)       // pin 1
#define IOT_PROG_SEL (0x04)   // pin 2
#define IOT_PROG_MODE (0x08)  // pin 3
#define IR_LED (0x10)         // pin 4

// Port 6 Definitions
#define R_FORWARD (0x01)     // pin 0
#define L_FORWARD (0x02)     // pin 1
#define R_REVERSE (0x04)     // pin 2
#define L_REVERSE (0x08)     // pin 3
#define LCD_BACKLITE (0x10)  // pin 4
#define P6_5 (0x20)          // pin 5
#define GRN_LED (0x40)       // pin 6

#endif /* ADC_INTERRUPT_HEADER_H */