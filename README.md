# 支持超标量的前端模拟器

C++实现的前端模拟器，支持超标量,IO生成器版本仍在施工中...

以FETCH_WIDTH=4为例，BPU基本结构图如下：

![BPU-top-arch](https://github.com/WattskiTian/C_tage-tage_IO/blob/master/BPU_arch.svg)

前端基本结构图（仅示意，实际信号与此不同）：

![front-end-top-arch](https://github.com/WattskiTian/C_tage-tage_IO/blob/master/front_end_top.svg)

test_env.cpp为测试环境，可以检查模拟器是否正常工作

（理论上后端在一个周期内提交的预测对比结果应该是最多COMMIT_WIDTH条，但此处为了代码方便统一比对FETCH_WIDTH条）

测试环境基本结构如下：

![test-env-arch](https://github.com/WattskiTian/C_tage-tage_IO/blob/master/test_env.svg)
