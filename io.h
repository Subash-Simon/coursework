//
// Created by Subash Pun on 02/04/2026.
//

#ifndef MINI_COURSEWORK_IO_H
#define MINI_COURSEWORK_IO_H

#include "waveform.h"

// Loads waveform samples from a CSV file into dynamic memory
int load_csv(const char *filename,
             WaveformSample **samples_out,
             size_t *count_out);

// Writes all calculated results into results.txt report file
int write_report(const char *output_filename,
                 const char *input_filename,
                 size_t sample_count,
                 const PhaseMetrics phase_metrics[],
                 size_t phase_count,
                 const FieldStats field_stats[],
                 size_t field_count);

#endif // MINI_COURSEWORK_IO_H