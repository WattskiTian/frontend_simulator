#include "demo_tage.h"
#include "front_IO.h"
#include "front_module.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <queue>

#define DELAY_CYCLE 0

// to store the front-end prediction result
struct PredictResult {
  uint32_t instructions[FETCH_WIDTH];
  bool predict_dir[FETCH_WIDTH];
  uint32_t predict_next_fetch_address;
  uint32_t predict_base_pc[FETCH_WIDTH];
};

// to store the actual execution result
// should be COMMIT_WIDTH but for simplicity, we use FETCH_WIDTH
struct ActualResult {
  bool dir[FETCH_WIDTH];
  uint32_t pc[FETCH_WIDTH];
  uint32_t nextpc;
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
    result.nextpc = nextpc;
    result.br_type[i] = br_type;
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
    printf("--------------------------------\n");
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
      ActualResult actual_result;
      for (int i = 0; i < FETCH_WIDTH; i++) {
        pred_result.instructions[i] = out->instructions[i];
        pred_result.predict_dir[i] = out->predict_dir[i];
        pred_result.predict_base_pc[i] = out->pc[i];
      }
      pred_result.predict_next_fetch_address = out->predict_next_fetch_address;
      predict_queue.push(pred_result);
      // printf("pushing %x\n", out->predict_next_fetch_address);
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
      // printf("popping...\n");

      // set the feedback signal
      in->reset = false;
      bool first_taken = false;
      for (int i = 0; i < FETCH_WIDTH; i++) {
        if (first_taken == false) {
          in->back2front_valid[i] = true;
          in->predict_base_pc[i] = actual.pc[i];
          if (actual.pc[i] != pred.predict_base_pc[i]) {
            printf("pc mismatch: %x != %x\n", actual.pc[i],
                   pred.predict_base_pc[i]);
            exit(1);
          }
          in->predict_dir[i] = pred.predict_dir[i];
          in->actual_dir[i] = actual.dir[i];
          in->actual_br_type[i] = actual.br_type[i];
        } else if (first_taken == true) {
          in->back2front_valid[i] = false;
          continue;
        }
        if (actual.dir[i] == 1) {
          first_taken = true;
        }
      }
      in->refetch_address = actual.nextpc;
      in->refetch = (pred.predict_next_fetch_address != actual.nextpc);
      in->FIFO_read_enable = true;
      printf("[test_env_checker] refetch: %d,predict_npc: %x,actual_npc: %x\n",
             in->refetch, pred.predict_next_fetch_address, actual.nextpc);
      if (in->refetch) {
        // empty predict_queue
        while (!predict_queue.empty()) {
          predict_queue.pop();
          // printf("popping...\n");
        }
      }
    } else {
      in->reset = false;
      for (int i = 0; i < FETCH_WIDTH; i++) {
        in->back2front_valid[i] = false;
        in->predict_base_pc[i] = 0;
        in->predict_dir[i] = false;
        in->actual_dir[i] = false;
        in->actual_br_type[i] = 0;
      }
      in->refetch_address = 0;
      in->refetch = false;
      in->FIFO_read_enable = true;
    }
  }
}

int main() {
  srand(time(0));
  // test_env_checker(100);
  TAGE_do_update(0x11e04, false, false);
  TAGE_do_update(0x11e08, false, false);
  TAGE_do_update(0x11e0c, true, false);
  printf("%d\n", TAGE_get_prediction(0x11e0c));
  TAGE_do_update(0x11df8, false, false);
  TAGE_do_update(0x11dfc, false, false);
  TAGE_do_update(0x11e00, false, false);
  TAGE_do_update(0x11e04, false, false);
  TAGE_do_update(0x11e08, false, false);
  TAGE_do_update(0x11e0c, true, false);
  printf("%d\n", TAGE_get_prediction(0x11e0c));
  TAGE_do_update(0x11df8, false, false);
  TAGE_do_update(0x11dfc, false, false);
  TAGE_do_update(0x11e00, false, false);
  TAGE_do_update(0x11e04, false, false);
  TAGE_do_update(0x11e08, false, false);
  TAGE_do_update(0x11e0c, true, false);
  printf("%d\n", TAGE_get_prediction(0x11e0c));
  return 0;
}