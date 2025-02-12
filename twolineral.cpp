 // /home/rimuru/Downloads/6d091bda191082d788ab05cc5bf432dd.jpg
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 雙線性插值函數
Mat bilinearInterpolation(const Mat& src, Size dst_size) {
    if (src.empty()) {
        cerr << "輸入圖像為空！" << endl;
        return Mat();
    }

    // 計算縮放比例（注意：這裡是逆向映射）
    float scale_x = static_cast<float>(src.cols - 1) / (dst_size.width - 1);  // 避免邊界溢出
    float scale_y = static_cast<float>(src.rows - 1) / (dst_size.height - 1);

    Mat dst(dst_size, src.type());

    for (int y = 0; y < dst.rows; ++y) {
        for (int x = 0; x < dst.cols; ++x) {
            // 計算原圖中的浮點坐標（逆向映射）
            float src_x = x * scale_x;
            float src_y = y * scale_y;

            // 找到四個鄰近像素的整數坐標
            int x0 = static_cast<int>(src_x);
            int y0 = static_cast<int>(src_y);
            int x1 = min(x0 + 1, src.cols - 1);  // 邊界保護
            int y1 = min(y0 + 1, src.rows - 1);

            // 計算小數部分（權重）
            float dx = src_x - x0;
            float dy = src_y - y0;
            float w1 = (1 - dx) * (1 - dy);
            float w2 = dx * (1 - dy);
            float w3 = (1 - dx) * dy;
            float w4 = dx * dy;

            if (src.channels() == 1) {
                // 灰度圖像處理
                float interpolated = 
                    w1 * src.at<uchar>(y0, x0) +
                    w2 * src.at<uchar>(y0, x1) +
                    w3 * src.at<uchar>(y1, x0) +
                    w4 * src.at<uchar>(y1, x1);
                dst.at<uchar>(y, x) = static_cast<uchar>(interpolated + 0.5);  // 四捨五入
            } else if (src.channels() == 3) {
                // 彩色圖像處理（BGR三通道）
                for (int c = 0; c < 3; ++c) {
                float interpolated = 
                    w1 * src.at<Vec3b>(y0, x0)[c] +
                    w2 * src.at<Vec3b>(y0, x1)[c] +
                    w3 * src.at<Vec3b>(y1, x0)[c] +
                    w4 * src.at<Vec3b>(y1, x1)[c];
                dst.at<Vec3b>(y, x)[c] = static_cast<uchar>(interpolated + 0.5);
                }
            }
        }
    }
    return dst;
}

// 驗證與OpenCV實現的一致性
void verifyWithOpenCV(const Mat& src, Size size) {
    Mat cv_result;
    resize(src, cv_result, size, 0, 0, INTER_LINEAR);

    Mat custom_result = bilinearInterpolation(src, size);

    // 計算絕對誤差
    Mat diff;
    absdiff(cv_result, custom_result, diff);

    // 統計最大誤差（理想情況應為0）
    double max_diff;
    minMaxLoc(diff, nullptr, &max_diff);
    cout << "與OpenCV結果的最大差異：" << max_diff << endl;

    // 注意：由於浮點計算精度差異，可能會有1以內的誤差
}

int main() {
    Mat img = imread("/home/rimuru/Downloads/6d091bda191082d788ab05cc5bf432dd.jpg", IMREAD_COLOR);  // 替換為你的圖片路徑
    if (img.empty()) {
        cerr << "無法讀取圖像！" << endl;
        return -1;
    }

    // 縮放至800x600
    Size target_size(img.cols * 2, img.rows * 2);

    // 自定義插值
    Mat custom_result = bilinearInterpolation(img, target_size);

    // 驗證正確性
    verifyWithOpenCV(img, target_size);

    // 顯示結果
    imshow("原始圖像", img);
    imshow("自定義雙線性插值", custom_result);
    waitKey(0);

    return 0;
}