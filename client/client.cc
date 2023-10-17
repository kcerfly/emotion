#include "client.h"
#include "config/config.h"

bool EmotionClient::Connect()
{
  std::shared_ptr<Channel> channel = grpc::CreateChannel(
      "127.0.0.1:10087", grpc::InsecureChannelCredentials());
  gpr_timespec tm_out{3, 0, GPR_TIMESPAN};
  bool connected = channel->WaitForConnected<gpr_timespec>(tm_out);
  if (connected)
  {
    std::cout << "connect success!" << std::endl;
    stub_ = EmotionService::NewStub(channel);
    return true;
  }
  return false;
}

void EmotionClient::SetInfo() { 
    ownership_ = FLAGS_ownership;
    state_ = FLAGS_state;
}

void EmotionClient::ShowResult(Response *response)
{
  
  std::cout << "***********************************************" << std::endl;
  std::cout << "current ownership : " << response->ownership() << std::endl;
  std::cout << "current state : " << response->state() << std::endl;
  std::cout << "current gain_loss : " << response->gain_loss() << std::endl;
}

void EmotionClient::Update(Response *response)
{
  ownership_ = response->ownership();
  state_ = response->state();
}

Status EmotionClient::SetScore(const ScoreRequest &request, Response *response)
{
  ClientContext context;
  Status status = stub_->SetScore(&context, request, response);
  return status;
}
Status EmotionClient::SetValue(const ValueRequest &request, Response *response)
{
  ClientContext context;
  Status status = stub_->SetValue(&context, request, response);
  return status;
}
Status EmotionClient::SetType(const SetTypeRequest &request, Response *response)
{
  ClientContext context;
  Status status = stub_->SetType(&context, request, response);
  return status;
}
Status EmotionClient::SetInfo(const SetInfoRequest &request, Response *response)
{
  ClientContext context;
  Status status = stub_->SetInfo(&context, request, response);
  return status;
}
void EmotionClient::loop()
{
  char c;
  std::cout << "***********************************************" << std::endl;
  std::cout << "*  direction  :  描述                         " << std::endl;
  std::cout << "***********************************************" << std::endl;
  std::cout << "*       1     :  面包被别人吃掉，资产减少   " << std::endl;
  std::cout << "*       2     :  获得面包，资产增加                " << std::endl;
  std::cout << "*       3     :  吃掉面包，饱腹感增加，资产减少  " << std::endl;
  std::cout << "*       4     :  获得娃娃玩具，资产增加          " << std::endl;
  std::cout << "*       5     :  调整\"得失V\"值     " << std::endl;
  std::cout << "*       6     :  变得饥饿     " << std::endl;
  std::cout << "*       7     :  改变loss type     " << std::endl;
  std::cout << "*       8     :  初始化     " << std::endl;
  std::cout << "***********************************************" << std::endl;
  std::cout << "*  输入指令：  ";
  std::cin >> c;
  work(c);
}
void EmotionClient::work(char c)
{
  system("clear");
  Response resp;
  switch (c)
  {
  case '3':
    SelfEatBread(&resp);
    break;
  case '5':
    ChangeV(&resp);
    break;
  case '1':
    OtherEatBread(&resp);
    break;
  case '4':
    GetToy(&resp);
    break;
  case '2':
    GetBread(&resp);
    break;
  case '6':
    SwitchHungry(&resp);
    break;
  case '7':
    SwitchLossType(&resp);
    break;
  case '8':
    Info(&resp);
    break;
  default:
    break;
  }
  
}

void EmotionClient::SelfEatBread(Response *resp) {
  ScoreRequest req;
  req.set_ownership(0 - FLAGS_bread_score);
  req.set_state(FLAGS_score_by_eat_bread);
  auto r = SetScore(req, resp);
  if (r.ok()) {
    std::cout << "rpc success!" << std::endl;
    // ShowResult(resp);
    Update(resp);
  } else {
    std::cout << "rpc fail!" << std::endl;
  }
}
void EmotionClient::ChangeV(Response *resp) {
  ValueRequest req;
  system("clear");
  std::cout << "范围[0,1]，请输入V值：";
  double v;
  std::cin >> v;
  req.set_gain_loss(v);
  auto r = SetValue(req, resp);
  if (r.ok()) {
    std::cout << "rpc success!" << std::endl;
    // ShowResult(resp);
    Update(resp);
  } else {
    std::cout << "rpc fail!" << std::endl;
  }
}
void EmotionClient::OtherEatBread(Response *resp) {
  ScoreRequest req;
  req.set_ownership(0 - FLAGS_bread_score);
  auto r = SetScore(req, resp);
  if (r.ok()) {
    std::cout << "rpc success!" << std::endl;
    // ShowResult(resp);
    Update(resp);
  } else {
    std::cout << "rpc fail!" << std::endl;
  }
}
void EmotionClient::GetToy(Response *resp) {
  ScoreRequest req;
  req.set_ownership(FLAGS_toy_score);
  auto r = SetScore(req, resp);
  if (r.ok()) {
    std::cout << "rpc success!" << std::endl;
    // ShowResult(resp);
    Update(resp);
  } else {
    std::cout << "rpc fail!" << std::endl;
  }
}
void EmotionClient::GetBread(Response *resp) {
  ScoreRequest req;
  req.set_ownership(FLAGS_bread_score);
  auto r = SetScore(req, resp);
  if (r.ok()) {
    std::cout << "rpc success!" << std::endl;
    // ShowResult(resp);
    Update(resp);
  } else {
    std::cout << "rpc fail!" << std::endl;
  }
}
void EmotionClient::SwitchHungry(Response *resp) {
  ScoreRequest req;
  req.set_state(FLAGS_hungry_score - state_);
  auto r = SetScore(req, resp);
  if (r.ok()) {
    std::cout << "rpc success!" << std::endl;
    // ShowResult(resp);
    Update(resp);
  } else {
    std::cout << "rpc fail!" << std::endl;
  }
}
void EmotionClient::SwitchLossType(Response *resp) {
  SetTypeRequest req;
  auto r = SetType(req, resp);
  if (r.ok()) {
    std::cout << "rpc success!" << std::endl;
    // ShowResult(resp);
    Update(resp);
  } else {
    std::cout << "rpc fail!" << std::endl;
  }
}
void EmotionClient::Info(Response *resp) {
  SetInfoRequest req;
  auto r = SetInfo(req, resp);
  if (r.ok()) {
    std::cout << "rpc success!" << std::endl;
    // ShowResult(resp);
    Update(resp);
  } else {
    std::cout << "rpc fail!" << std::endl;
  }
}
