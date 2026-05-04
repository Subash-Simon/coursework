//
// Created by Subash Pun 02/04/2026
//

#include "io.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_BUFFER_SIZE 512

// Check if a line contains non-space characters
static int line_has_text(const char *line)
{
    while (*line != '\0') {
        if (!isspace((unsigned char)*line)) {
            return 1;
        }
        line++;
    }
    return 0;
}

// Count number of valid data rows in CSV file
static size_t count_data_rows(FILE *file)
{
    char line[LINE_BUFFER_SIZE];
    size_t row_count = 0;

    while (fgets(line, sizeof(line), file) != 0) {
        if (line_has_text(line)) {
            row_count++;
        }
    }
    return row_count;
}

// Parse one CSV line into WaveformSample struct
static int parse_sample_line(const char *line, WaveformSample *sample)
{
    int fields_read = sscanf(line,
                             " %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
                             &sample->timestamp,
                             &sample->phase_a_voltage,
                             &sample->phase_b_voltage,
                             &sample->phase_c_voltage,
                             &sample->line_current,
                             &sample->frequency,
                             &sample->power_factor,
                             &sample->thd_percent);

    return fields_read == 8;
}

// Load CSV file into dynamic memory
int load_csv(const char *filename,
             WaveformSample **samples_out,
             size_t *count_out)
{
    FILE *file;
    WaveformSample *samples;
    char line[LINE_BUFFER_SIZE];
    size_t row_count;
    size_t index = 0;

    // Check for null input
    if (filename == 0 || samples_out == 0 || count_out == 0) {
        fprintf(stderr, "Internal error: load_csv received a null pointer.\n");
        return 1;
    }

    *samples_out = 0;
    *count_out = 0;

    // Open file
    file = fopen(filename, "r");
    if (file == 0) {
        fprintf(stderr, "Error: could not open input file '%s'.\n", filename);
        return 1;
    }

    // Check if file is empty
    if (fgets(line, sizeof(line), file) == 0) {
        fprintf(stderr, "Error: input file is empty.\n");
        fclose(file);
        return 1;
    }

    // Count number of rows
    row_count = count_data_rows(file);
    if (row_count == 0) {
        fprintf(stderr, "Error: input file has a header but no data rows.\n");
        fclose(file);
        return 1;
    }

    // Prevent memory overflow
    if (row_count > SIZE_MAX / sizeof(WaveformSample)) {
        fprintf(stderr, "Error: input file is too large.\n");
        fclose(file);
        return 1;
    }

    // Allocate memory
    samples = malloc(row_count * sizeof(WaveformSample));
    if (samples == 0) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // Reset file pointer and skip header
    rewind(file);
    fgets(line, sizeof(line), file);

    // Read each line and parse
    while (fgets(line, sizeof(line), file) != 0) {

        if (!line_has_text(line)) {
            continue;
        }

        if (!parse_sample_line(line, &samples[index])) {
            fprintf(stderr, "Error: could not parse row %zu.\n", index + 1);
            free(samples);
            fclose(file);
            return 1;
        }

        index++;
    }

    fclose(file);

    *samples_out = samples;
    *count_out = index;

    return 0;
}

// Write analysis results into results.txt
int write_report(const char *output_filename,
                 const char *input_filename,
                 size_t sample_count,
                 const PhaseMetrics phase_metrics[],
                 size_t phase_count,
                 const FieldStats field_stats[],
                 size_t field_count)
{
    FILE *file;
    size_t i;

    // Open output file
    file = fopen(output_filename, "w");
    if (file == 0) {
        fprintf(stderr, "Error: could not write file '%s'.\n", output_filename);
        return 1;
    }

    // Write report header
    fprintf(file, "Power Quality Waveform Analyser Report\n");
    fprintf(file, "======================================\n\n");

    fprintf(file, "Input file: %s\n", input_filename);
    fprintf(file, "Samples analysed: %zu\n\n", sample_count);

    // Phase analysis results
    fprintf(file, "Voltage phase analysis\n");
    fprintf(file, "----------------------\n");

    for (i = 0; i < phase_count; i++) {
        const PhaseMetrics *m = &phase_metrics[i];

        fprintf(file, "%s\n", m->name);
        fprintf(file, "  RMS: %.3f V\n", m->rms);
        fprintf(file, "  Min: %.3f V\n", m->minimum);
        fprintf(file, "  Max: %.3f V\n", m->maximum);
        fprintf(file, "  Peak-to-peak: %.3f V\n", m->peak_to_peak);
        fprintf(file, "  DC offset: %.6f V\n", m->dc_offset);
        fprintf(file, "  Std dev: %.3f V\n", m->standard_deviation);
        fprintf(file, "  Clipped: %zu\n", m->clipped_count);
        fprintf(file, "  Compliance: %s\n\n",
                m->is_compliant ? "COMPLIANT" : "NON-COMPLIANT");
    }

    // Other field stats
    fprintf(file, "Other measured fields\n");
    fprintf(file, "---------------------\n");

    for (i = 0; i < field_count; i++) {
        const FieldStats *s = &field_stats[i];

        fprintf(file, "%s\n", s->name);
        fprintf(file, "  Min: %.6f\n", s->minimum);
        fprintf(file, "  Max: %.6f\n", s->maximum);
        fprintf(file, "  Mean: %.6f\n\n", s->mean);
    }

    fclose(file);
    return 0;
}