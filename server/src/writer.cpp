#include <queue>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/u_int32.hpp>
#include "village_interfaces/srv/sell_novel.hpp"

using std::placeholders::_1;
using std::placeholders::_2;


class Writer: public rclcpp::Node{
public:
    Writer(std::string name): Node(name){
        RCLCPP_INFO(this->get_logger(), "Hello Everyone, I'm writer caston.");
        pub_ = this->create_publisher<std_msgs::msg::UInt32>("sexy_girl_money", 10);
        callback_group_service_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
        sub_ = this->create_subscription<std_msgs::msg::String>("sexy_girl", 10, std::bind(&Writer::callback_topic, this, _1));
        server_ = this->create_service<village_interfaces::srv::SellNovel>("sell_novel",    // 服务类型,服务名称
                    std::bind(&Writer::callback_sell_novel, this, _1, _2),                  // 回调函数,_1代表request,_2代表response
                    rmw_qos_profile_services_default,                                       // 通信质量
                    callback_group_service_);                                               // 告诉ros2,调用回调函数处理请求时,请把它放到单独线程的回调组中.
    }
private:
    rclcpp::CallbackGroup::SharedPtr callback_group_service_;
    rclcpp::Service<village_interfaces::srv::SellNovel>::SharedPtr server_;
    rclcpp::Publisher<std_msgs::msg::UInt32>::SharedPtr pub_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    std::queue<std::string> novels_queue;
    

    // 服务端,需要接受请求和返回响应,所以要有两个参数,隐含参数this会在std::bind中绑定上
    // 除此之外,回调函数的类型是void,并不需要返回值,目前猜测response在server中是个全局变量.
    void callback_sell_novel(village_interfaces::srv::SellNovel::Request::SharedPtr request,
                             village_interfaces::srv::SellNovel::Response::SharedPtr response){
        unsigned int novelsNum = request->money * 1;
        RCLCPP_INFO(this->get_logger(), "收到订单,需要%d章.", novelsNum);

        if(novels_queue.size() < novelsNum){

            RCLCPP_INFO(this->get_logger(), "目前库存不足,只有%d章,请耐心等待.", (int)novels_queue.size());
            rclcpp::Rate loop_rate(1);
            // 判断ros系统运行是否正常
            if(!rclcpp::ok()){
                 RCLCPP_ERROR(this->get_logger(), "程序被中止了");
                 return ;
            }
            
            while(novels_queue.size() < novelsNum){
                RCLCPP_INFO(this->get_logger(), "目前有%d章,还差%d章.", 
                            (int)novels_queue.size(), novelsNum-(int)novels_queue.size());
                // 防止循环过快,
                loop_rate.sleep();
            }
        }

        // 这里章节数目满足需求了,通知买家准备收货,发货.
        RCLCPP_INFO(this->get_logger(), "当前章节库存量为%d,已满足需求,马上安排发货.", (int)novels_queue.size());

        // 一本本把书去出来,放进响应对象中
        for(unsigned int i = 0; i < novelsNum; i++){
            // 这里的pop和python中的pop是不一样的,这里的pop函数类型为void不会返回被弹出的值.
            response->novels.push_back(novels_queue.front());
            novels_queue.pop();
        }
    }


    void callback_topic(const std_msgs::msg::String::SharedPtr msg){
        std_msgs::msg::UInt32 money;
        money.data = 10;

        pub_->publish(money);
        RCLCPP_INFO(this->get_logger(), "王二:我收到了:'%s',并给了李四;%d的稿费", msg->data.c_str(), money.data);

        novels_queue.push(msg->data);
    }

};


int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Writer>("wang2");
    rclcpp::executors::MultiThreadedExecutor exector;
    exector.add_node(node);
    exector.spin();
    rclcpp::shutdown();
    return 0;
}
