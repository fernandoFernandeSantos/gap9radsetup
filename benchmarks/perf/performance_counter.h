//
// Created by fernando on 21/09/22.
//

#ifndef GAP_SDK_BEAM_PERFORMANCE_COUNTER_H
#define GAP_SDK_BEAM_PERFORMANCE_COUNTER_H

#define SETUP_RADIATION_ITERATIONS 2048
#define ALWAYS_INLINE__ inline __attribute__((always_inline))

#define gap_ncore() __builtin_pulp_CoreCount()

typedef struct {
    uint32_t perf_cycles;
    uint32_t perf_inst;
} rad_setup_metrics_t;

//static rad_setup_metrics_t rad_metrics_diff = {0, 0};
static rad_setup_metrics_t begin_rad_metrics_iteration = {0, 0};
static rad_setup_metrics_t end_rad_metrics_iteration = {0, 0};

static ALWAYS_INLINE__ void start_counters() {
    // Set to defaults
    pi_pmu_voltage_set(PI_PMU_VOLTAGE_DOMAIN_CHIP, VOLT_SET);
    pi_pmu_voltage_set(PI_PMU_VOLTAGE_DOMAIN_CHIP, VOLT_SET);
    pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC);
    pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL);
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, FREQ_PE);

    pi_perf_conf(1 << PI_PERF_CYCLES | 1 << PI_PERF_INSTR);
    pi_perf_reset();

    pi_perf_start();
//    rad_metrics_diff.perf_cycles = pi_perf_read(PI_PERF_CYCLES);
//    rad_metrics_diff.perf_inst = pi_perf_read(PI_PERF_INSTR);
}

static ALWAYS_INLINE__ void end_counters() {
    printf("RATIT:%d CORE:%d CYCLES_OUT:%d INST_OUT:%d\n",
           SETUP_RADIATION_ITERATIONS,
           gap_ncore(),
//           rad_metrics_diff.perf_cycles,
           pi_perf_read(PI_PERF_CYCLES),
//           rad_metrics_diff.perf_inst,
           pi_perf_read(PI_PERF_INSTR)
    );
    pi_perf_stop();
}

static ALWAYS_INLINE__ void begin_perf_iteration_i() {
    pi_perf_reset();
    begin_rad_metrics_iteration.perf_cycles = pi_perf_read(PI_PERF_CYCLES);
    begin_rad_metrics_iteration.perf_inst = pi_perf_read(PI_PERF_INSTR);
}

static ALWAYS_INLINE__ void end_perf_iteration_i() {
    end_rad_metrics_iteration.perf_cycles = pi_perf_read(PI_PERF_CYCLES);
    end_rad_metrics_iteration.perf_inst = pi_perf_read(PI_PERF_INSTR);
}

static ALWAYS_INLINE__ void print_iteration_perf(int its) {
    printf("ITS:%d CORE:%d CYCLES_T1:%d CYCLES_T2:%d INST_T1:%d INST_T2:%d\n",
           its,
           gap_ncore(),
           begin_rad_metrics_iteration.perf_cycles,
           end_rad_metrics_iteration.perf_cycles,
           begin_rad_metrics_iteration.perf_inst,
           end_rad_metrics_iteration.perf_inst
    );
}


#endif //GAP_SDK_BEAM_PERFORMANCE_COUNTER_H
