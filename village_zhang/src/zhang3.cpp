#include"rclcpp/rclcpp.hpp"
#include "village_interfaces/srv/sell_novel.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class PoorManNode : public rclcpp::Node{
public:
    PoorManNode(std::string name) : Node(name){
        RCLCPP_INFO(this->get_logger(), "I'm PoorMan %s", name.c_str());
        client_ = this->create_client<village_interfaces::srv::SellNovel>("sell_novel");
    }

    void buy_novel(){
        RCLCPP_INFO(this->get_logger(), "go to buy novel !");

        while(!client_->wait_for_service(std::chrono::seconds(1))){
            RCLCPP_INFO(this->get_logger(), "in while");
            if(!rclcpp::ok()){
                RCLCPP_ERROR(this->get_logger(), "Wiating for server but broken..");
                return;
            }
            RCLCPP_INFO(this->get_logger(), "Waiting for service !");
        }

        auto request = std::make_shared<village_interfaces::srv::SellNovel_Request>();
        request->money = 5;

        client_->async_send_request(request, std::bind(&PoorManNode::novels_callback, this, _1));
    }

    
private:
    rclcpp::Client<village_interfaces::srv::SellNovel>::SharedPtr client_;

    //TODO:: why here use SharedFeature
    void novels_callback(rclcpp::Client<village_interfaces::srv::SellNovel>::SharedFuture response){
        auto result = response.get();
        RCLCPP_INFO(this->get_logger(), "get %d novel, now reading !", (int)result->novels.size());

        for(std::string novel:result->novels){
            RCLCPP_INFO(this->get_logger(), "%s", novel.c_str());
        }

        RCLCPP_INFO(this->get_logger(), "i have read this novel, very good, looking forward to new ~");
    }
};


int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PoorManNode>("Zhang3");
    node->buy_novel();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}