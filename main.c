
//
// Created by Subash on 02/04/2026.
//

#include "io.h"
#include "waveform.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    WaveformSample *samples = 0;
    size_t sample_count = 0;
    PhaseMetrics phase_metrics[3];
    FieldStats field_stats[4];
    size_t total_clipped = 0;
    int status;

    if (argc != 2) {
        printf("Usage: %s <csv-file>\n", argv[0]);
        printf("Example: %s power_quality_log.csv\n", argv[0]);
        return 1;
    }

    status = load_csv(argv[1], &samples, &sample_count);
    if (status != 0) {
        return 1;
    }

    phase_metrics[0] = analyse_phase(samples,
                                     sample_count,
                                     PHASE_A,
                                     CLIPPING_THRESHOLD);
    phase_metrics[1] = analyse_phase(samples,
                                     sample_count,
                                     PHASE_B,
                                     CLIPPING_THRESHOLD);
    phase_metrics[2] = analyse_phase(samples,
                                     sample_count,
                                     PHASE_C,
                                     CLIPPING_THRESHOLD);

    field_stats[0] = analyse_field(samples, sample_count, FIELD_LINE_CURRENT);
    field_stats[1] = analyse_field(samples, sample_count, FIELD_FREQUENCY);
    field_stats[2] = analyse_field(samples, sample_count, FIELD_POWER_FACTOR);
    field_stats[3] = analyse_field(samples, sample_count, FIELD_THD_PERCENT);

    total_clipped = phase_metrics[0].clipped_count
                    + phase_metrics[1].clipped_count
                    + phase_metrics[2].clipped_count;

    status = write_report("results.txt",
                          argv[1],
                          sample_count,
                          phase_metrics,
                          3,
                          field_stats,
                          4);

    if (status != 0) {
        free(samples);
        return 1;
    }

    printf("Loaded %zu samples.\n", sample_count);
    printf("Phase A RMS: %.3f V\n", phase_metrics[0].rms);
    printf("Phase B RMS: %.3f V\n", phase_metrics[1].rms);
    printf("Phase C RMS: %.3f V\n", phase_metrics[2].rms);
    printf("Total clipped samples: %zu\n", total_clipped);
    printf("Report written to results.txt\n");

    free(samples);

    return 0;
}