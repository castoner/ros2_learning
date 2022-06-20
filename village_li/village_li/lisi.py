import rclpy
from rclpy.node import Node


def test(args=None):
    rclpy.init(args=args)
    node = Node("li4")
    node.get_logger().info("hello i'm writer lisi")
    rclpy.spin(node)
    rclpy.shutdown()