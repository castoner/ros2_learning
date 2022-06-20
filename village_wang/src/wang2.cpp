#include "rclcpp/rclcpp.hpp"


int main(int argc, char **argv){
    rclcpp::init(argc,argv);
    // create a wang2 node 
    auto node = std::make_shared<rclcpp::Node>("wang2");
    // print self introduction
    RCLCPP_INFO(node->get_logger(), "Hello every, i'm single dog wang2.");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}