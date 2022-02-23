#ifndef __TENSORFLOW_CLSS_H
#define __TENSORFLOW_CLSS_H
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/model.h"

/**
 * This module comes from code by a coworker of this thesis. Some of this code comes from other projects which will be mentioned below.
 * @author Jesús Á. Coll
 * @date 02/06/2021
 * @see https://github.com/finnickniu/tensorflow_object_detection_tflite
 * @version v0.0.1
**/

const float confidence_value = 0.67f;

/**
 * This class contains methods for Computer Vision using Tensorflow-Lite API.
 * 
**/
class TFlite_C
{
public:
    /**
     * Constructor.
     * 
    **/
    TFlite_C()
    {
        /*Initialize objects*/
        output_locations = nullptr;
        output_classes = nullptr;
        num_detections = nullptr;
        scores = nullptr;
    }

    /**
     * Set classification labels.
     * 
     * @param {lab} a string vector for labels
     * 
    **/
    void sLabels(std::vector<std::string> lab)
    {
        labels = lab;
    }

    /**
     * Delete class functions. 
     * 
    **/
    TFlite_C &operator=(const TFlite_C &) = delete;
    TFlite_C(const TFlite_C &) = delete;

    /**
     * Set a image size and a interpreter to get information for bounding boxes and output score.
     * 
     * @param {interpreter} a unique pointer for an interpreter that comes from TFL-API
     * @param {cam_heigth} image heigth
     * @param {cam_width} image width
     * @param {out_scor} a float output score
     * 
    **/
    std::vector<std::vector<int>> tfLocate(std::unique_ptr<tflite::Interpreter> &interpreter, int cam_height, int cam_width, std::vector<float> &out_scor)
    {
        std::vector<std::vector<int>> boxes;
        setOutputs(interpreter);

        auto output_data = output_locations->data.f;
        auto out_cls = output_classes->data.f;
        auto scrs = scores->data.f;
        auto nums = num_detections->data.f;

        for (int i = 0; i < 120; i++)
        {
            auto output = output_data[i];
            locations.push_back(output);
            cls.push_back(out_cls[i]);
        }

        int count = 0;

        for (int j = 0; j < locations.size(); j += 4)
        {
            float confidence = expit(scrs[count]); // Use SIGMOID function to get the score
            if (confidence > confidence_value)
            {
                auto ymin = locations[j] * cam_height;
                auto xmin = locations[j + 1] * cam_width;
                auto ymax = locations[j + 2] * cam_height;
                auto xmax = locations[j + 3] * cam_width;
                auto xwidth = xmax - xmin;
                auto yheight = ymax - ymin;

                int label_id = cls[count] + 1; // Look at the classes that have been
                                               // detected to see if it is a person or not

                if (labels[label_id] != "person")
                    continue;

                // If this detection is a person, it will be introduced to the list
                // as a bounding box
                boxes.insert(boxes.end(), {(int)xmin, (int)ymin, (int)xmax, (int)ymax});
                out_scor.push_back(confidence);
            }
            count++;
        }
        return boxes;
    }

private:
    float expit(float x)
    {
        // SIGMOID function
        return 1.f / (1.f + expf(-x));
    }

    void setOutputs(std::unique_ptr<tflite::Interpreter> &interpreter)
    {
        output_locations = interpreter->tensor(interpreter->outputs()[0]);
        output_classes = interpreter->tensor(interpreter->outputs()[1]);
        scores = interpreter->tensor(interpreter->outputs()[2]);
        num_detections = interpreter->tensor(interpreter->outputs()[3]);
    }
    /**/
    tflite::ops::builtin::BuiltinOpResolver resolver;
    /**/
    TfLiteTensor *output_locations;
    TfLiteTensor *output_classes;
    TfLiteTensor *num_detections;
    TfLiteTensor *scores;
    std::vector<std::string> labels;
    /**/
    std::vector<float> locations; // Object Locations
    std::vector<float> cls;       // Object Classes
};

#endif //__TENSORFLOW_CLSS_H
