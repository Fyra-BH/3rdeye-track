#include "cv_utils.h"
#include "det_utils.h"
#include <iomanip>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

std::string float_to_string(float value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}


void letterbox(cv::Mat& img, int new_width, int new_height, letterbox_info& info)
{
    int width = img.cols;
    int height = img.rows;
    int & offset_x = info.offset_x, & offset_y = info.offset_y;
    float & scale = info.scale;
    scale = std::min((float)new_width / (float)width, (float)new_height / (float)height);
    int new_unscaled_width = (int)(scale * (float)width);
    int new_unscaled_height = (int)(scale * (float)height);
    offset_x = (new_width - new_unscaled_width) / 2;
    offset_y = (new_height - new_unscaled_height) / 2;
    cv::resize(img, img, cv::Size(new_unscaled_width, new_unscaled_height));
    cv::copyMakeBorder(img, img, offset_y, offset_y, offset_x, offset_x, cv::BORDER_CONSTANT, cv::Scalar(114, 114, 114));
}


void draw_box_in_color(cv::Mat& img, Box box, cv::Scalar color, int thickness)
{
    cv::Point topLeft(static_cast<int>(box.x - box.w / 2), static_cast<int>(box.y - box.h / 2));
    cv::Point bottomRight(static_cast<int>(box.x + box.w / 2), static_cast<int>(box.y + box.h / 2));
    cv::rectangle(img, topLeft, bottomRight, color, thickness);
}

void draw_bboxes(cv::Mat& img,const std::vector<BBox> &bboxes, std::vector<std::string> &classes, float text_scale, int thickness)
{
    cv::Scalar color(0, 255, 0); // 边界框颜色，可以根据需要更改
    cv::Scalar textColor(0, 0, 255); // 文本颜色，白色
    cv::Point textOrg;
    int fontFace = cv::FONT_HERSHEY_SIMPLEX; // 选择字体风格

    for (size_t i = 0; i < bboxes.size(); i++) {
        // 计算边界框的左上角和右下角坐标
        cv::Point topLeft(static_cast<int>(bboxes[i].x - bboxes[i].w / 2), static_cast<int>(bboxes[i].y - bboxes[i].h / 2));
        cv::Point bottomRight(static_cast<int>(bboxes[i].x + bboxes[i].w / 2), static_cast<int>(bboxes[i].y + bboxes[i].h / 2));
        // 绘制边界框
        cv::rectangle(img, topLeft, bottomRight, color, thickness);

        // 确保类别索引在范围内
        if (bboxes[i].cls < classes.size()) {
            // 绘制文本
            std::string label = classes[bboxes[i].cls] + float_to_string(bboxes[i].score, 2);
            int baseLine = 0;
            textOrg = cv::Point(topLeft.x, topLeft.y - 5); // 文本位置在边界框上方

            // 计算文本大小
            cv::Size labelSize = cv::getTextSize(label, fontFace, text_scale, thickness, &baseLine);

            // 绘制文本背景，确保背景可以完全覆盖文本
            cv::rectangle(img, cv::Point(textOrg.x, textOrg.y + baseLine - labelSize.height - 5),
                          cv::Point(textOrg.x + labelSize.width, textOrg.y + baseLine + thickness), color, cv::FILLED);

            // 绘制文本
            cv::putText(img, label, textOrg, fontFace, text_scale, textColor, thickness);
        }
    }
}

void draw_tracks(cv::Mat& img, const trackmap_t &tracks, int thickness)
{
    for (auto &item : tracks) {
        auto track_id = item.first;
        auto track = item.second;
        if (track.size() > 1) {
            auto box = xywh2xyxy(track.back());
            auto leftTop = cv::Point((int)box.x1 - 5, (int)box.y1 - 15);
            auto c = color_list[track_id % 80];
            auto color = cv::Scalar(c[0], c[1], c[2]);
            cv::rectangle(img, cv::Point((int)box.x1, (int)box.y1), cv::Point((int)box.x2, (int)box.y2), cv::Scalar(255, 255, 255), thickness);
            cv::putText(img, "id=" + std::to_string(track_id), leftTop, cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
            for (size_t i = 1; i < track.size(); i++) {
                auto p1 = track[i];
                auto p2 = track[i - 1];
                int x1 = static_cast<int>(p1.x),
                    y1 = static_cast<int>(p1.y),
                    x2 = static_cast<int>(p2.x),
                    y2 = static_cast<int>(p2.y);
                cv::line(img, cv::Point(x1, y1), cv::Point(x2, y2), color, thickness);
            }
        }
    }
}
