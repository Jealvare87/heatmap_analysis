#include "image_ops.h"

bool iou(cv::Rect &rectA, cv::Rect rectB, float nms_threshold)
{
    int x1 = cv::max(rectA.x, rectB.x);
    int y1 = cv::max(rectA.y, rectB.y);
    int x2 = cv::min(rectA.x + rectA.width, rectB.x + rectB.width);
    int y2 = cv::min(rectA.y + rectA.height, rectB.y + rectB.height);
    int w = cv::max(0, (x2 - x1 + 1));
    int h = cv::max(0, (y2 - y1 + 1));
    float inter = w * h;
    float areaA = rectA.width * rectA.height;
    float areaB = rectB.width * rectB.height;

    // IoU = Intersection / Union
    float ext = inter / (areaA + areaB - inter);

    if (ext > nms_threshold)
    {
        rectA.x = x1;
        rectA.y = y1;
        rectA.width = cv::max(rectA.width, rectB.width);
        rectA.height = cv::max(rectA.height, rectB.height);
        return true;
    }
    return false;
}

std::vector<float> nms(cv::Mat frame, std::vector<std::vector<int>> &boxes, std::vector<float> scor, const double nms_threshold)
{
    std::vector<int> index;
    for (int i = 0; i < scor.size(); ++i)
    {
        index.push_back(i);
    }

    sort(index.begin(), index.end(), [&](int a, int b) { // The indices are ordered from highest to lowest according to the scores
        return scor[a] > scor[b];
    });

    std::vector<bool> del(scor.size(), false);

    /** 
     * After this loop we know if the intersection between two objects exceeds the threshold we have set. 
     * If they are very close, it will not be included in the output
     * 
    **/

    for (size_t i = 0; i < index.size(); i++)
    {
        if (!del[index[i]])
        {
            for (size_t j = i + 1; j < index.size(); j++)
            {
                cv::Rect rec1((int)boxes[index[i]][0], (int)boxes[index[i]][1], (int)boxes[index[i]][2], (int)boxes[index[i]][3]);
                cv::Rect rec2((int)boxes[index[j]][0], (int)boxes[index[j]][1], (int)boxes[index[j]][2], (int)boxes[index[j]][3]);

                if (iou(rec1, rec2,nms_threshold))
                {
                    boxes[index[i]][0] = rec1.x;
                    boxes[index[i]][1] = rec1.y;
                    boxes[index[i]][2] = rec1.width;
                    boxes[index[i]][3] = rec1.height;

                    del[index[j]] = true;
                }
            }
        }
    }
    std::vector<std::vector<int>> new_obj;
    std::vector<float> new_scor;

    /*
    * Once we know what is included and that we cannot create a new series of objects
    */
    for (const auto i : index)
    {
        std::vector<int> obj;
        if (!del[i])
        {
            obj = boxes[i];
            cv::Rect object((int)boxes[i][0], (int)boxes[i][1], (int)(boxes[i][2]-boxes[i][0]),
                        (int)(boxes[i][3]-boxes[i][1]));
            cv::rectangle(frame, object, cv::Scalar(0, 255, 138), 2);
            new_scor.push_back(scor[i]);
            new_obj.push_back(obj);
        }
        
    }

    boxes = new_obj;
    return new_scor;
}
