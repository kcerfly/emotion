#include "proto/emotion.grpc.pb.h"
#include "proto/emotion.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <cstdlib>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class EmotionClient
{
public:
  bool Connect();
  void SetInfo();


  void ShowResult(Response *response);
  void Update(Response *response);

  Status SetScore(const ScoreRequest &request, Response *response);
  Status SetValue(const ValueRequest &request, Response *response);
  Status SetType(const SetTypeRequest &request, Response *response);
  Status SetInfo(const SetInfoRequest &request, Response *response);
  void loop();
  void work(char c);

  void SelfEatBread(Response *resp);
  void ChangeV(Response *resp);
  void OtherEatBread(Response *resp);
  void GetToy(Response *resp);
  void GetBread(Response *resp);
  void SwitchHungry(Response *resp);
  void SwitchLossType(Response *resp);
  void Info(Response *resp);

private:
  std::unique_ptr<EmotionService::Stub> stub_;
  double ownership_;
  double state_;
};