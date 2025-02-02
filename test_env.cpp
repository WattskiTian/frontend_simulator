#include "front_IO.h"
#include "front_module.h"
#include <cstdint>
#include <cstdio>
#include <queue>

#define DELAY_CYCLE 2

// to store the front-end prediction result
struct PredictResult {
  uint32_t instructions[FETCH_WIDTH];
  bool predict_dir;
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc;
};

// to store the actual execution result
struct ActualResult {
  bool dir[FETCH_WIDTH];
  uint32_t pc[FETCH_WIDTH];
  uint32_t nextpc[FETCH_WIDTH];
  uint32_t br_type[FETCH_WIDTH];
};

static FILE *log_file = nullptr;
static std::queue<PredictResult> predict_queue;
static std::queue<ActualResult> actual_queue;
static uint64_t cycle_count = 0;
static bool initialized = false;

// read the actual execution result from the log file
static int read_actual_result(ActualResult &result) {
  for (int i = 0; i < FETCH_WIDTH; i++) {
    uint32_t dir, pc, nextpc, br_type;
    if (fscanf(log_file, "%u %x %x %u\n", &dir, &pc, &nextpc, &br_type) != 4) {
      return 1;
    }
    result.dir[i] = (bool)dir;
    result.pc[i] = pc;
    result.nextpc[i] = nextpc;
    result.br_type[i] = br_type;
  }
  return 0;
}

void test_env_checker(uint64_t step_count) {

  struct front_top_in in_tmp;
  struct front_top_out out_tmp;
  struct front_top_in *in = &in_tmp;
  struct front_top_out *out = &out_tmp;

  while (step_count--) {
    // initialize
    if (!initialized) {
      log_file = fopen("/home/watts/dhrystone/gem5output_rv/fronted_log", "r");
      if (log_file == NULL) {
        printf("Error: Cannot open log file\n");
        return;
      }
      printf("[test_env_checker] log file opened\n");

      in->reset = true;
      in->FIFO_read_enable = true;
      front_top(in, out);
      printf("[test_env_checker] front_top reset done\n");

      initialized = true;
      cycle_count = 0;
    } else {
      // run the front-end
      cycle_count++;
      // input signal was set in the previous cycle
      front_top(in, out);
      printf("[test_env_checker] front_top cycle %lu done\n", cycle_count);
    }

    // save the front-end prediction result
    if (out->FIFO_valid) {
      PredictResult pred_result;
      for (int i = 0; i < FETCH_WIDTH; i++) {
        pred_result.instructions[i] = out->instructions[i];
      }
      pred_result.predict_dir = out->predict_dir;
      pred_result.predict_next_fetch_address = out->predict_next_fetch_address;
      pred_result.predict_base_pc = out->predict_base_pc;
      predict_queue.push(pred_result);

      // read the actual execution result
      ActualResult actual_result;
      if (read_actual_result(actual_result) == 0) {
        actual_queue.push(actual_result);
      }
    }

    // compare the result after DELAY_CYCLE cycles
    if (cycle_count > DELAY_CYCLE && !predict_queue.empty() &&
        !actual_queue.empty()) {
      PredictResult pred = predict_queue.front();
      ActualResult actual = actual_queue.front();
      predict_queue.pop();
      actual_queue.pop();

      // find the first taken branch
      int first_taken = -1;
      uint32_t actual_next_pc = actual.pc[0] + 4 * FETCH_WIDTH;
      for (int i = 0; i < FETCH_WIDTH; i++) {
        if (actual.dir[i]) {
          first_taken = i;
          actual_next_pc = actual.nextpc[i];
          break;
        }
      }

      // set the feedback signal
      in->reset = false;
      in->back2front_valid = true;
      in->refetch = (pred.predict_next_fetch_address != actual_next_pc);
      in->predict_base_pc = actual.pc[first_taken];
      in->refetch_address = actual_next_pc;
      in->predict_dir = pred.predict_dir;
      in->actual_dir = actual.dir[first_taken];
      in->actual_br_type = actual.br_type[first_taken];
      in->FIFO_read_enable = true;
      printf("[test_env_checker] refetch: %d\n", in->refetch);
    } else {
      in->reset = false;
      in->back2front_valid = false;
      in->refetch = false;
      in->predict_base_pc = 0;
      in->refetch_address = 0;
      in->predict_dir = false;
      in->actual_dir = false;
      in->actual_br_type = 0;
      in->FIFO_read_enable = true;
    }
  }
}

int main() {
  test_env_checker(100);
  return 0;
}