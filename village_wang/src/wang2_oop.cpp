#include<queue>
#include<rclcpp/rclcpp.hpp>
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/u_int32.hpp"
#include "village_interfaces/srv/sell_novel.hpp"


using std::placeholders::_1;
using std::placeholders::_2;


// constructor print a log
// subscrib a topic which name is 'sexy_girl' and this subscription has a callback function
class SingleDogNode : public rclcpp::Node{
public:
    SingleDogNode(std::string name) : Node(name){
        RCLCPP_INFO(this->get_logger(), "Hello Everyone, i'm single dog %s.", name.c_str());
        // cppze
        sub_novel = this->create_subscription<std_msgs::msg::String>("sexy_girl", 10, std::bind(&SingleDogNode::topic_callback, this, _1));
        pub_money = this->create_publisher<std_msgs::msg::UInt32>("sexy_girl_money", 10);
        callback_group_services_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
        server_ = this->create_service<village_interfaces::srv::SellNovel>("sell_novel", std::bind(&SingleDogNode::sell_book_callback, this, _1, _2),            
            rmw_qos_profile_services_default,
            callback_group_services_);
    }
private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_novel;
    rclcpp::Publisher<std_msgs::msg::UInt32>::SharedPtr pub_money;
    // multithread to solve dead block
    rclcpp::CallbackGroup::SharedPtr callback_group_services_;
    rclcpp::Service<village_interfaces::srv::SellNovel>::SharedPtr server_;
    // this is a book deque, store books wang2 got.
    std::queue<std::string> novels_queue;

    void topic_callback(const std_msgs::msg::String::SharedPtr msg){
        std_msgs::msg::UInt32 money;
        money.data = 10;
        pub_money->publish(money);
        RCLCPP_INFO(this->get_logger(), "I have read: '%s', give lisi %d yuan", msg->data.c_str(), money.data);
        novels_queue.push(msg->data);
    }

    void sell_book_callback(const village_interfaces::srv::SellNovel::Request::SharedPtr request,
                            const village_interfaces::srv::SellNovel::Response::SharedPtr response){
        RCLCPP_INFO(this->get_logger(), "get a shop_book request, get %d yuan", request->money);
        unsigned int novelsNum = request->money * 1;

        if(novels_queue.size() < novelsNum) {
            RCLCPP_INFO(this->get_logger(), "novel's store num = %d, cannot meet require, please wait !", (int)novels_queue.size());
            rclcpp::Rate loop_rate(1);
            while(novels_queue.size() < novelsNum){
                if(!rclcpp::ok()){
                    RCLCPP_ERROR(this->get_logger(), "System has broken!");
                    return;
                }
                RCLCPP_INFO(this->get_logger(), "waiting, now have %d zhang, need %d zhang", (int)novels_queue.size(), novelsNum - (int)novels_queue.size());
                
                // sleep let this cycle 1/s
                loop_rate.sleep();
            }
        }

        RCLCPP_INFO(this->get_logger(), "now i have %d zhang buauty girl: meet your require!", (int)novels_queue.size());

        for(unsigned int i = 0; i < novelsNum; i++){
            response->novels.push_back(novels_queue.front());
            novels_queue.pop();
        }
    }


};


int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SingleDogNode>("wang2");
    // multithread run node
    rclcpp::executors::MultiThreadedExecutor exector;
    exector.add_node(node);
    exector.spin();
    rclcpp::shutdown();
    return 0;
}