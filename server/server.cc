#include "proto/emotion.grpc.pb.h"
#include "proto/emotion.pb.h"
#include "config/config.h"

#include <grpc++/grpc++.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <mutex>
#include <limits>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class Emotion{
  public:
    void SetOwnership(double value) {
        ownership_ += value;
        delta_ownership_ = value;
    }
    void SetState(double value) {
        state_ += value;
        delta_state_ = value;
    }
    void SetScore(const ScoreRequest *request) {
        auto delta_ownership = request->ownership();
        auto delta_state = state_;
        ownership_ += delta_ownership;
        auto state_temp = state_ + request->state();
        state_ = state_temp > 1 ? 1 : state_temp;
        state_ = state_temp < 0 ? 0 : state_temp;
        delta_state = state_ - delta_state;
        emotion_score_ = curr_emotion_score_ + FLAGS_alpha_1 * delta_ownership +
                FLAGS_alpha_2 * delta_state;
        if (emotion_score_ >= max_emotion_score_) {
            emotion_score_ = max_emotion_score_;
        } else if (emotion_score_ <= 0 - max_emotion_score_) {
            emotion_score_ = 0 - max_emotion_score_;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        t_ = 0;
    }
    void SetGainLoss(double value) {
        gain_loss_ = value;
        ChangeMaxScore();
    }
    void SetType() {
        if (FLAGS_loss == LossType::EXPONENT) {
            FLAGS_loss = LossType::LINEAR;
        } else {
            FLAGS_loss = LossType::EXPONENT;
        }
    }
    void SetInfo() { 
        std::lock_guard<std::mutex> lock(mutex_);
        ownership_ = FLAGS_ownership;
        state_ = FLAGS_state;
        gain_loss_ = FLAGS_gain_loss;
        emotion_score_ = 0.0000001;
        curr_emotion_score_ = 0;
        // loss_type_ = FLAGS_loss;
        k_linear_ = -3 / FLAGS_loss_time;
        k_exponent_ = -1.38 / FLAGS_loss_time;
        t_ = 0;
        ChangeMaxScore();
    }

    void ChangeMaxScore() {
        if (gain_loss_ < 0.0000001 && gain_loss_ > -0.0000001) {
            max_emotion_score_ = std::numeric_limits<double>::max();
        }
        max_emotion_score_ = 4 / gain_loss_;
    }

    double GetOwnership() {
        return ownership_;
    }
    double GetState() {
        return state_;
    }
    double GetGainLoss() {
        return gain_loss_;
    }

    double LinearLoss() {
        if (emotion_score_ > 0) {
            auto temp = emotion_score_ + k_linear_ * t_;
            return temp < 0 ? 0 : temp;
        }
        auto temp = emotion_score_ - k_linear_ * t_;
        return temp > 0 ? 0 : temp;
    }

    double ExponentLoss1() {
        return emotion_score_ * std::exp(k_exponent_ * t_);
    }

    double ExponentLoss2() {
        return max_emotion_score_ * std::exp(k_exponent_ * t_);
    }

    double Calculate() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (FLAGS_loss == LossType::EXPONENT) {
            std::cout << "loss : exponent" << std::endl;
            curr_emotion_score_ = ExponentLoss1();
        } else {
            std::cout << "loss : linear" << std::endl;
            curr_emotion_score_ = LinearLoss();
        }
        ++t_;
        std::cout << "当前情绪分数：" << curr_emotion_score_ << std::endl;
        return curr_emotion_score_;
    }

    void CurrentState() {
        std::cout << "current ownership : " << ownership_ << std::endl;
        std::cout << "current state_ : " << state_ << std::endl;
        std::cout << "current gain_loss_ : " << gain_loss_ << std::endl;
        std::cout << "current emotion_score_ : " << emotion_score_ << std::endl;
        std::cout << "current curr_emotion_score_ : " << curr_emotion_score_ << std::endl;
        std::cout << "current time : " << t_ << std::endl;
    }

    void ShowEmotion() {
        int score = gain_loss_ * curr_emotion_score_;
        std::cout << "score: " << score << std::endl;
        std::cout << "*******************************" << std::endl;
        std::cout << "  当前情绪：";
        if (score >= 3) {
            std::cout << "😄  很开心    ";
        } else if (score == 2) {
            std::cout << "😃  开心      ";
        } else if (score == 1) {
            std::cout << "🙂  有点开心  ";
        } else if (score == 0) {
            std::cout << "😐  平静";
        } else if (score == -1) {
            std::cout << "😔  有点伤心";
        } else if (score == -2) {
            std::cout << "😞  伤心";
        } else if (score <= -3) {
            std::cout << "😢  很伤心";
        }
        std::cout << std::endl << "*******************************" << std::endl;
    }

  private:
    double ownership_;
    double state_;
    double gain_loss_;
    double delta_ownership_;
    double delta_state_;
    double emotion_score_;
    double curr_emotion_score_;
    LossType loss_type_;

    double k_linear_;
    double k_exponent_;

    double max_emotion_score_;

    double last_t_;

    int t_;

    std::mutex mutex_;
};

class EmotionServer : public EmotionService::Service {
  public:
    EmotionServer(){
        emotion_.SetInfo();
    }

    void Run() {
        while (true) {
            system("clear");
            emotion_.Calculate();
            CurrentState();
            emotion_.ShowEmotion();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void CurrentState() {
        // std::cout << "current ownership : " << emotion_.GetOwnership() << std::endl;
        // std::cout << "current state     : " << emotion_.GetState() << std::endl;
        // std::cout << "current gain_loss : " << emotion_.GetGainLoss() << std::endl;
        emotion_.CurrentState();
    }

    void StartServer() {
        ServerBuilder builder;
        builder.AddListeningPort("127.0.0.1:10087", grpc::InsecureServerCredentials());
        builder.RegisterService(this);
        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "Server listening on 127.0.0.1:10086" << std::endl;
        server->Wait();
    }

    Status SetScore(ServerContext* context, const ScoreRequest* request, Response* reply) override{
        // emotion_.SetOwnership(request->ownership());
        // emotion_.SetState(request->state());
        emotion_.SetScore(request);
        reply->set_ownership(emotion_.GetOwnership());
        reply->set_state(emotion_.GetState());
        reply->set_gain_loss(emotion_.GetGainLoss());
        return Status::OK;
    }
    Status SetValue(ServerContext* context, const ValueRequest* request, Response* reply) override{
        emotion_.SetGainLoss(request->gain_loss());
        reply->set_ownership(emotion_.GetOwnership());
        reply->set_state(emotion_.GetState());
        reply->set_gain_loss(emotion_.GetGainLoss());
        return Status::OK;
    }
    Status SetType(ServerContext* context, const SetTypeRequest* request, Response* reply) override{
        emotion_.SetType();
        reply->set_ownership(emotion_.GetOwnership());
        reply->set_state(emotion_.GetState());
        reply->set_gain_loss(emotion_.GetGainLoss());
        return Status::OK;
    }
    Status SetInfo(ServerContext* context, const SetInfoRequest* request, Response* reply) override{
        emotion_.SetInfo();
        reply->set_ownership(emotion_.GetOwnership());
        reply->set_state(emotion_.GetState());
        reply->set_gain_loss(emotion_.GetGainLoss());
        return Status::OK;
    }
  private:
    Emotion emotion_;
};

int main() {
    EmotionServer server;
    std::thread(&EmotionServer::Run, &server).detach();
    server.StartServer();
}