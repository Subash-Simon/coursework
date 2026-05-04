//
// Created by Subash on 02/04/2026.
//

#include "waveform.h"
#include <math.h>

// Helper function to square a value
static double square(double value)
{
    return value * value;
}

// Convert phase enum to readable name
const char *phase_name(Phase phase)
{
    if (phase == PHASE_A) return "Phase A";
    if (phase == PHASE_B) return "Phase B";
    if (phase == PHASE_C) return "Phase C";
    return "Unknown phase";
}

// Convert field enum to readable name
static const char *field_name(NumericField field)
{
    if (field == FIELD_LINE_CURRENT) return "Line current";
    if (field == FIELD_FREQUENCY) return "Frequency";
    if (field == FIELD_POWER_FACTOR) return "Power factor";
    if (field == FIELD_THD_PERCENT) return "THD percent";
    return "Unknown field";
}

// Get voltage value for selected phase from sample
double get_phase_voltage(const WaveformSample *sample, Phase phase)
{
    if (phase == PHASE_A) return sample->phase_a_voltage;
    if (phase == PHASE_B) return sample->phase_b_voltage;
    return sample->phase_c_voltage;
}

// Get numeric field value (current, frequency, etc.)
static double get_field_value(const WaveformSample *sample, NumericField field)
{
    if (field == FIELD_LINE_CURRENT) return sample->line_current;
    if (field == FIELD_FREQUENCY) return sample->frequency;
    if (field == FIELD_POWER_FACTOR) return sample->power_factor;
    return sample->thd_percent;
}

// Check if RMS voltage is within allowed tolerance range
int check_compliance(double rms,
                     double nominal_voltage,
                     double tolerance_fraction)
{
    double lower_limit = nominal_voltage * (1.0 - tolerance_fraction);
    double upper_limit = nominal_voltage * (1.0 + tolerance_fraction);

    return rms >= lower_limit && rms <= upper_limit;
}

// Analyse voltage waveform for a single phase
PhaseMetrics analyse_phase(const WaveformSample *samples,
                           size_t count,
                           Phase phase,
                           double clipping_threshold)
{
    PhaseMetrics metrics;

    // Pointers for iteration
    const WaveformSample *current;
    const WaveformSample *end;

    // Variables for calculations
    double sum = 0.0;
    double sum_squares = 0.0;
    double variance_sum = 0.0;

    // Initialize structure values
    metrics.name = phase_name(phase);
    metrics.minimum = 0.0;
    metrics.maximum = 0.0;
    metrics.rms = 0.0;
    metrics.peak_to_peak = 0.0;
    metrics.dc_offset = 0.0;
    metrics.standard_deviation = 0.0;
    metrics.clipped_count = 0;
    metrics.is_compliant = 0;

    // Safety check
    if (samples == 0 || count == 0) {
        return metrics;
    }

    // Set initial min/max
    metrics.minimum = get_phase_voltage(samples, phase);
    metrics.maximum = metrics.minimum;

    end = samples + count;

    // Loop through all samples
    for (current = samples; current < end; current++) {

        double voltage = get_phase_voltage(current, phase);

        // Update min/max
        if (voltage < metrics.minimum) metrics.minimum = voltage;
        if (voltage > metrics.maximum) metrics.maximum = voltage;

        // Count clipping events
        if (fabs(voltage) >= clipping_threshold) {
            metrics.clipped_count++;
        }

        // Sum for RMS and DC offset
        sum += voltage;
        sum_squares += square(voltage);
    }

    // Final calculations
    metrics.dc_offset = sum / (double)count;
    metrics.rms = sqrt(sum_squares / (double)count);
    metrics.peak_to_peak = metrics.maximum - metrics.minimum;

    // Check compliance
    metrics.is_compliant = check_compliance(metrics.rms,
                                            NOMINAL_VOLTAGE,
                                            VOLTAGE_TOLERANCE_FRACTION);

    // Calculate standard deviation
    for (current = samples; current < end; current++) {
        double voltage = get_phase_voltage(current, phase);
        variance_sum += square(voltage - metrics.dc_offset);
    }

    metrics.standard_deviation = sqrt(variance_sum / (double)count);

    return metrics;
}

// Analyse non-voltage fields (current, frequency, etc.)
FieldStats analyse_field(const WaveformSample *samples,
                         size_t count,
                         NumericField field)
{
    FieldStats stats;

    const WaveformSample *current;
    const WaveformSample *end;

    double sum = 0.0;

    // Set field name
    stats.name = field_name(field);
    stats.minimum = 0.0;
    stats.maximum = 0.0;
    stats.mean = 0.0;

    // Safety check
    if (samples == 0 || count == 0) {
        return stats;
    }

    // Initialize min/max
    stats.minimum = get_field_value(samples, field);
    stats.maximum = stats.minimum;

    end = samples + count;

    // Loop through samples
    for (current = samples; current < end; current++) {

        double value = get_field_value(current, field);

        if (value < stats.minimum) stats.minimum = value;
        if (value > stats.maximum) stats.maximum = value;

        sum += value;
    }

    // Calculate mean
    stats.mean = sum / (double)count;

    return stats;
}