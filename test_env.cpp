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
  bool dir;
  uint32_t pc;
  uint32_t nextpc;
  uint32_t br_type;
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
    result.dir = (bool)dir;
    result.pc = pc;
    result.nextpc = nextpc;
    result.br_type = br_type;
    if (dir == 1) {
      return 0; // stop reading when first taken branch is found
    }
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
      printf("pushing %x\n", out->predict_next_fetch_address);

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
      printf("popping...\n");

      // set the feedback signal
      in->reset = false;
      in->back2front_valid = true;
      in->refetch = (pred.predict_next_fetch_address != actual.nextpc);
      in->predict_base_pc = actual.pc;
      in->refetch_address = actual.nextpc;
      in->predict_dir = pred.predict_dir;
      in->actual_dir = actual.dir;
      in->actual_br_type = actual.br_type;
      in->FIFO_read_enable = true;
      printf("[test_env_checker] refetch: %d,predict_npc: %x,actual_npc: %x\n",
             in->refetch, pred.predict_next_fetch_address, actual.nextpc);
      if (in->refetch) {
        // empty predict_queue
        while (!predict_queue.empty()) {
          predict_queue.pop();
          printf("popping...\n");
        }
      }
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