# 支持超标量的前端模拟器

C++实现的前端模拟器，支持超标量，IO生成器版本仍在施工中...

使用方法：

```
make testenv # 运行测试环境
make testenv_IO # 运行测试环境（IO版本）
```

以FETCH_WIDTH=4为例，BPU基本结构图如下：

![BPU-top-arch](https://github.com/WattskiTian/frontend_simulator/blob/master/svg/BPU_arch.svg)

前端基本结构图（仅示意，实际信号与此不同）：

![front-end-top-arch](https://github.com/WattskiTian/frontend_simulator/blob/master/svg/front_end_top.svg)

test_env.cpp为测试环境，可以检查模拟器是否正常工作

（理论上后端在一个周期内提交的预测对比结果应该是最多COMMIT_WIDTH条，但此处为了代码方便统一比对FETCH_WIDTH条）

测试环境基本结构如下：

![test-env-arch](https://github.com/WattskiTian/frontend_simulator/blob/master/svg/test_env.svg)

tage_IO部分基本结构:

![tage_IO-arch](https://github.com/WattskiTian/frontend_simulator/blob/master/svg/tageIO.svg)

文件目录：

```
.
├── BPU
│   ├── BPU_top.cpp
│   ├── dir_predictor
│   │   ├── demo_tage
│   │   ├── demo_tage.cpp
│   │   ├── demo_tage.gdb
│   │   ├── demo_tage.h
│   │   ├── dir_predictor_IO
│   │   │   ├── tage_IO.cpp
│   │   │   └── tage_types.h
│   │   ├── experimential_demo
│   │   │   ├── demo_loop.cpp
│   │   │   ├── demo_ltage.cpp
│   │   │   ├── loop_pred.h
│   │   │   ├── utils.cpp
│   │   │   └── utils.h
│   │   └── tage_IO.h
│   └── target_predictor
│       ├── BTB
│       ├── btb.cpp
│       ├── btb.h
│       ├── ras.cpp
│       ├── ras.h
│       ├── target_cache.cpp
│       ├── target_cache.h
│       └── target_predictor_IO
│           ├── btb_IO.cpp
│           ├── ras_IO.cpp
│           ├── target_cache_IO.cpp
│           └── target_predictor_types.h
├── IO_train
│   ├── IO_cvt.cpp
│   ├── IO_cvt.h
│   └── tage_func.h
├── Makefile
├── README.md
├── fifo
│   ├── PTAB.cpp
│   └── instruction_FIFO.cpp
├── front_IO.h
├── front_module.h
├── front_top.cpp
├── fronted_main.cpp
├── frontend.h
├── icache
│   └── icache.cpp
├── log
│   ├── simple_front_log
│   ├── tage_log2
│   ├── test_env_IO_log
│   └── test_env_log
├── sequential_components
│   ├── seq_comp.cpp
│   └── seq_comp.h
├── svg
│   ├── BPU_arch.svg
│   ├── front.svg
│   ├── front_end_top.svg
│   ├── tageIO.svg
│   └── test_env.svg
├── test
├── test_env.cpp
└── test_env_IO
```