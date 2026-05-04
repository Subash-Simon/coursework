//
// Created by subash pun 20/04/2026.
//

// Header file for waveform analysis: defines data structures, constants, and function prototypes

#ifndef MINI_COURSEWORK_WAVEFORM_H
#define MINI_COURSEWORK_WAVEFORM_H

#include <stddef.h>

// System constants for voltage analysis
#define NOMINAL_VOLTAGE 230.0
#define VOLTAGE_TOLERANCE_FRACTION 0.10
#define CLIPPING_THRESHOLD 324.9

// Enum representing the three phases of the supply
typedef enum {
    PHASE_A = 0,
    PHASE_B = 1,
    PHASE_C = 2
} Phase;

// Enum representing additional numeric fields in the dataset
typedef enum {
    FIELD_LINE_CURRENT = 0,
    FIELD_FREQUENCY = 1,
    FIELD_POWER_FACTOR = 2,
    FIELD_THD_PERCENT = 3
} NumericField;

// Structure representing one row/sample from the CSV file
typedef struct {
    double timestamp;
    double phase_a_voltage;
    double phase_b_voltage;
    double phase_c_voltage;
    double line_current;
    double frequency;
    double power_factor;
    double thd_percent;
} WaveformSample;

// Structure storing calculated metrics for each voltage phase
typedef struct {
    const char *name;
    double minimum;
    double maximum;
    double rms;
    double peak_to_peak;
    double dc_offset;
    double standard_deviation;
    size_t clipped_count;
    int is_compliant;
} PhaseMetrics;

// Structure storing statistics for non-voltage fields (e.g., frequency, THD)
typedef struct {
    const char *name;
    double minimum;
    double maximum;
    double mean;
} FieldStats;

// Function declarations for waveform analysis
const char *phase_name(Phase phase);
double get_phase_voltage(const WaveformSample *sample, Phase phase);
PhaseMetrics analyse_phase(const WaveformSample *samples,
                           size_t count,
                           Phase phase,
                           double clipping_threshold);
FieldStats analyse_field(const WaveformSample *samples,
                         size_t count,
                         NumericField field);
int check_compliance(double rms,
                     double nominal_voltage,
                     double tolerance_fraction);

#endif //MINI_COURSEWORK_WAVEFORM_H