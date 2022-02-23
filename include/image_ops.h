#ifndef __IMAGE_OPS_H
#define __IMAGE_OPS_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <opencv2/tracking.hpp>

/**
 * This module comes from code by a coworker of this thesis. Some of this code comes from other projects which will be mentioned below.
 * 
 * Used for NMS functions.
 * 
 * @author Jesús Á. Coll
 * @date 02/06/2021
 * @see https://github.com/finnickniu/tensorflow_object_detection_tflite
 * @version v0.0.1
**/

/**
 *
 * Intersection Over Union method.
 * 
 * @param {rectA} first bounding box and if the are above of threshold, this will be an output
 * @param {rectB} second bounding box
 * @param {nms_threshold} A iou maximum threshold
 * 
 * @return if these BB are above of threshold
 *  
**/
bool iou(cv::Rect &rectA, cv::Rect rectB, float nms_threshold);

/**
 *
 * Non-Maximum Suppresion method.
 * 
 * @param {frame} image for draw bounding boxes 
 * @param {boxes} in boxes
 * @param {scor} initial scores
 * @param {nms_threshold} A iou maximum threshold
 * 
 * @return output scores
 * 
**/
std::vector<float> nms(cv::Mat frame, std::vector<std::vector<int>> &boxes, std::vector<float> scor, const double nms_threshold);

#endif //__IMAGE_OPS_H#ifndef __IMAGE_OPS_H
#define __IMAGE_OPS_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <opencv2/tracking.hpp>

/**
 * This module comes from code by a coworker of this thesis. Some of this code comes from other projects which will be mentioned below.
 * 
 * Used for NMS functions.
 * 
 * @author Jesús Á. Coll
 * @date 02/06/2021
 * @see https://github.com/finnickniu/tensorflow_object_detection_tflite
 * @version v0.0.1
**/

/**
 *
 * Intersection Over Union method.
 * 
 * @param {rectA} first bounding box and if the are above of threshold, this will be an output
 * @param {rectB} second bounding box
 * @param {nms_threshold} A iou maximum threshold
 * 
 * @return if these BB are above of threshold
 *  
**/
bool iou(cv::Rect &rectA, cv::Rect rectB, float nms_threshold);

/**
 *
 * Non-Maximum Suppresion method.
 * 
 * @param {frame} image for draw bounding boxes 
 * @param {boxes} in boxes
 * @param {scor} initial scores
 * @param {nms_threshold} A iou maximum threshold
 * 
 * @return output scores
 * 
**/
std::vector<float> nms(cv::Mat frame, std::vector<std::vector<int>> &boxes, std::vector<float> scor, const double nms_threshold);

#endif //__IMAGE_OPS_H