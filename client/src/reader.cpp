#include<rclcpp/rclcpp.hpp>
#include"village_interfaces/srv/sell_novel.hpp"

using std::placeholders::_1;

class Reader: public rclcpp::Node{
public:
    Reader(std::string name): Node(name){
        // 这里要用c_str是因为,名字是字符串,要转为char*类型的数据.
        RCLCPP_INFO(this->get_logger(), "大家好,我是一个贫穷的读者,%s", name.c_str());
        // 客户端只需要连接上服务即可
        client_ = this->create_client<village_interfaces::srv::SellNovel>("sell_novel");
    }

    void buy_novel(){
        RCLCPP_INFO(this->get_logger(), "我来买书了");

        while(!client_->wait_for_service(std::chrono::seconds(1))){
            if(!rclcpp::ok()){
                RCLCPP_ERROR(this->get_logger(), "系统出错啦");
                return;
            }

            RCLCPP_INFO(this->get_logger(), "等待服务器上线中");
        }

        auto request = std::make_shared<village_interfaces::srv::SellNovel_Request>();
        request->money = 5;
        client_->async_send_request(request, std::bind(&Reader::callback_get_novel, this, _1));
    }


private:
    rclcpp::Client<village_interfaces::srv::SellNovel>::SharedPtr client_;

    // 接收小说,回调函数.
    void callback_get_novel(rclcpp::Client<village_interfaces::srv::SellNovel>::SharedFuture response){
        auto result = response.get();
        RCLCPP_INFO(this->get_logger(), "收到了%d章节小说,开始阅读", (int)result->novels.size());

        for(std::string novel: result->novels){
            RCLCPP_INFO(this->get_logger(), "读到[%s]", novel.c_str());
        }

        RCLCPP_INFO(this->get_logger(), "读完啦~nice");
    }
};


int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Reader>("wang8");
    node->buy_novel();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}