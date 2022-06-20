import rclpy
from rclpy.node import Node
from village_interfaces.srv import BorrowMoney

class BaiPiao(Node):
    def __init__(self, name):
        super().__init__(name)
        self.get_logger().info("I'm li3, i like baipiao")
        self.borrow_money_client_ = self.create_client(BorrowMoney, "borrow_money")

    def borrow_response_callback(self, response):
        result = response.result()
        if result.success == True:
            self.get_logger().info("he is my brolther, i get %d yuan, go to have a lunch" % result.money)
        else:
            self.get_logger().info("i never love he , he didn't let me a little money!!!!")

    def borrow_money_eat(self):
        self.get_logger().info("find my brother to get some money")
        while not self.borrow_money_client_.wait_for_service(1.0):
            self.get_logger().warn("my brother is not at home, waiting")

        request = BorrowMoney.Request()
        request.name = self.get_name()
        request.money = 10
        self.borrow_money_client_.call_async(request).add_done_callback(self.borrow_response_callback)


def main():
    rclpy.init()
    node = BaiPiao("li3")
    node.borrow_money_eat()
    rclpy.spin(node)
    rclpy.shutdown()
    