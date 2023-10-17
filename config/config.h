#include <gflags/gflags.h>

enum LossType {
    LINEAR = 0,
    EXPONENT = 1,
};

// 初始配置
DEFINE_double(ownership, 1, "ownership");
DEFINE_double(state, 0.5, "state");
DEFINE_double(gain_loss, 0.5, "gain_loss");
// static LossType FLAGS_loss = LossType::LINEAR;
LossType FLAGS_loss = LossType::EXPONENT;

// 面包分数
DEFINE_double(bread_score, 1, "score of bread");

// 娃娃分数
DEFINE_double(toy_score, 2, "score of toy");

// 吃面包的饥饿度恢复
DEFINE_double(score_by_eat_bread, 0.5, "score of eat bread");

// 饥饿状态
DEFINE_double(hungry_score, 0.1, "score of eat bread");

// 线性loss k
DEFINE_double(k_linear, -0.04, "k");  // e(t) = e(t-1) - k * t

// 指数loss k
DEFINE_double(k_exponent, -0.02, "k_exp"); // e(t) = e(t-1) * exp (k * t)

// α1
DEFINE_double(alpha_1, 2.5, "α1");

// α2
DEFINE_double(alpha_2, 8, "α2");

// loss_time
DEFINE_double(loss_time, 500, "how much time(s) that emotion score 4->1");