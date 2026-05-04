//
// Created by Subash Pun 20/04/2026.
//
#include "io.h"
#include "waveform.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Pointer to store waveform samples dynamically
    WaveformSample *samples = 0;

    // Number of samples read from CSV
    size_t sample_count = 0;

    // Store analysis results for 3 phases (A, B, C)
    PhaseMetrics phase_metrics[3];

    // Store statistics for other fields (current, frequency, etc.)
    FieldStats field_stats[4];

    // Total number of clipped samples across all phases
    size_t total_clipped = 0;

    // Status variable for error checking
    int status;

    // Check if user provided correct number of arguments
    if (argc != 2) {
        printf("Error: Invalid input.\n");
        printf("Usage: %s <csv-file>\n", argv[0]);
        printf("Example: %s power_quality_log.csv\n", argv[0]);
        return 1;
    }

    // Load CSV file into memory
    status = load_csv(argv[1], &samples, &sample_count);
    if (status != 0) {
        return 1;
    }

    // Analyse voltage data for each phase
    phase_metrics[0] = analyse_phase(samples, sample_count, PHASE_A, CLIPPING_THRESHOLD);
    phase_metrics[1] = analyse_phase(samples, sample_count, PHASE_B, CLIPPING_THRESHOLD);
    phase_metrics[2] = analyse_phase(samples, sample_count, PHASE_C, CLIPPING_THRESHOLD);

    // Analyse other numerical fields (current, frequency, etc.)
    field_stats[0] = analyse_field(samples, sample_count, FIELD_LINE_CURRENT);
    field_stats[1] = analyse_field(samples, sample_count, FIELD_FREQUENCY);
    field_stats[2] = analyse_field(samples, sample_count, FIELD_POWER_FACTOR);
    field_stats[3] = analyse_field(samples, sample_count, FIELD_THD_PERCENT);

    // Calculate total clipped samples from all phases
    total_clipped = phase_metrics[0].clipped_count
                    + phase_metrics[1].clipped_count
                    + phase_metrics[2].clipped_count;

    // Write results to output file
    status = write_report("results.txt",
                          argv[1],
                          sample_count,
                          phase_metrics,
                          3,
                          field_stats,
                          4);

    // If writing fails, free memory and exit
    if (status != 0) {
        free(samples);
        return 1;
    }

    // Print summary to terminal
    printf("\n--- Analysis Summary ---\n");
    printf("Loaded %zu samples.\n", sample_count);
    printf("Phase A RMS: %.3f V\n", phase_metrics[0].rms);
    printf("Phase B RMS: %.3f V\n", phase_metrics[1].rms);
    printf("Phase C RMS: %.3f V\n", phase_metrics[2].rms);
    printf("Total clipped samples: %zu\n", total_clipped);
    printf("Report written to results.txt\n");

    // Free allocated memory
    free(samples);

    return 0;
}