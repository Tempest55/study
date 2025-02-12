#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 最近邻插值函数
Mat nearestNeighborInterpolation(const Mat& src, Size dst_size) {
    // 检查输入图像是否为空
    if (src.empty()) {
        cerr << "Input image is empty!" << endl;
        return Mat();
    }

    // 计算缩放比例
    float scale_x = static_cast<float>(src.cols) / dst_size.width;
    float scale_y = static_cast<float>(src.rows) / dst_size.height;

    // 创建目标图像（与源图像数据类型一致）
    Mat dst(dst_size, src.type());

    // 遍历目标图像每个像素
    for (int y = 0; y < dst.rows; y++) {
        for (int x = 0; x < dst.cols; x++) {
            // 计算源图像的最近邻坐标
            int src_x = cvRound(x * scale_x); // 四舍五入取整
            int src_y = cvRound(y * scale_y);

            // 确保坐标不越界
            src_x = min(src_x, src.cols - 1);
            src_y = min(src_y, src.rows - 1);

            // 复制像素值（处理多通道，如BGR图像）
            if (src.channels() == 1) {
                // 灰度图像
                dst.at<uchar>(y, x) = src.at<uchar>(src_y, src_x);
            } else if (src.channels() == 3) {
                // 彩色图像（BGR）
                dst.at<Vec3b>(y, x) = src.at<Vec3b>(src_y, src_x);
            }
        }
    }
    return dst;
}

// 主函数测试
int main() {
    // 读取图像
    Mat img = imread("/home/rimuru/Downloads/6d091bda191082d788ab05cc5bf432dd.jpg", IMREAD_COLOR);
    if (img.empty()) {
        cerr << "Could not read the image!" << endl;
        return -1;
    }

    // 定义目标尺寸（例如放大到原图的2倍）
    Size target_size(img.cols * 2, img.rows * 2);

    // 使用手写方法缩放
    Mat custom_result = nearestNeighborInterpolation(img, target_size);

    // 使用 OpenCV 内置函数验证结果
    Mat cv_result;
    resize(img, cv_result, target_size, 0, 0, INTER_NEAREST);

    // 显示结果
    imshow("Original", img);
    imshow("Custom Nearest Neighbor", custom_result);
    imshow("OpenCV Nearest Neighbor", cv_result);
    waitKey(0);

    return 0;
}