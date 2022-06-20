from numpy import uint32
from sympy import true
import rclpy
from rclpy.node import Node
from std_msgs.msg import String, UInt32
from sensor_msgs.msg import Image
from village_interfaces.msg import Novel
from village_interfaces.srv import BorrowMoney


class WriterNode(Node):
    def __init__(self, name):
        super().__init__(name)
        self.get_logger().info("Hello Everyone, i'm %s , i'm a writer! " %name)
        # self.pub_novel = self.create_publisher(String, "sexy_girl", 10) # 10 is the lenth of deque
        self.writer = self.create_publisher(Novel, "sexy_girl", 10)
        self.i = 0
        timer_period = 5
        self.timer = self.create_timer(timer_period, self.timer_callback) # start timer, call time_callback every 1s

        self.account = 80
        self.submoney = self.create_subscription(UInt32, "sexy_girl_money", self.recv_money_callback, 10)

        # create_service have three param: msg_type, server_name, callback_name
        self.brorrow_server = self.create_service(BorrowMoney, "borrow_money", self.borrow_money_callback)

        self.image = Image()
        self.create_subscription(Novel, "image", self.recv_image_callback, 10)


    def recv_image_callback(self, image):
        self.image = image

    def timer_callback(self):
        # msg = String()
        # msg.data = "the %d time, meet beauty girl at lian yan park "%self.i
        # self.pub_novel.publish(msg)

        msg = Novel()
        msg.content = '第%d回：潋滟湖 %d 次偶遇胡艳娘' % (self.i,self.i)
        msg.image = self.image
        self.writer.publish(msg)
        # get_logger().info() print log with info level
        self.get_logger().info("li4:i published sexy girl: '%s'" %msg.content)
        self.get_logger().info("li4:i published sexy girl with image: '%d*%d'" %(msg.image.height, msg.image.width))
        self.i += 1

    def recv_money_callback(self, money):
        self.account += money.data
        self.get_logger().info("li4, i have got %d money" % self.account)

    def borrow_money_callback(self, request, response):
        self.get_logger().info("I have get: a request from %s , now my account have %d yuan."%(request.name, self.account))
        if request.money <= int(self.account * 0.1):
            response.success = True
            response.money = request.money
            self.account -= request.money
            self.get_logger().info("success, brorrow out %d yuan, now my account have %d yuan."%(response.money, self.account))
        else:
            response.success = False
            response.money = 0

        return response

        
def main():
    rclpy.init()
    node = WriterNode("li4")
    rclpy.spin(node)# keep node running, and listen to ctrl+c
    rclpy.shutdown()
