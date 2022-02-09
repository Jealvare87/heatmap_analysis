#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <time.h>
#include <unistd.h>

#include <centroidtracker.h>
#include <omp.h>
#include "publisher.h"
#include "tensorflow_clss.h"
#include "image_ops.h"
#include "heatmap.h"

using namespace std;
using namespace cv;
using namespace cv::dnn;

#define NET "../mobilenet_ssd/model.tflite"
#define LABELMAP "../mobilenet_ssd/labelmap.txt"

int cam_width;
int cam_height;

const int skipFrames = 7;
long long int datos;

/**
 * From an image, how many people there are, are established and the follow-up begins
 * 
 * @param {frame} an input image
 * @param {m} results of people's behavior
 * @param {centroidTracker} a centroidtracker object
 * @param {interpreter} a unique pointer for an interpreter that comes from TFL-API
 * @param {labels} which represent classified objects
 * @param {h} heatMap object
 * 
 * @return resulting frame including BB
 * 
**/
Mat detectVideo(Mat frame, tResult &m, CentroidTracker *centroidTracker, unique_ptr<tflite::Interpreter> &interpreter,
                vector<string> labels, HeatMap *h)
{
    Mat image;
    TFlite_C tfl;
    tfl.sLabels(labels);

    // Set the input of the neuronal network
    resize(frame, image, Size(320, 320));

    vector<vector<int>> boxes;

    interpreter->AllocateTensors();

    uchar *input = interpreter->typed_input_tensor<uchar>(0);

    // Feed input
    auto image_height = image.rows;
    auto image_width = image.cols;
    auto image_channels = 3;
    int number_of_pixels = image_height * image_width * image_channels;
    int base_index = 0;

    // Copy image to input as input tensor
    memcpy(interpreter->typed_input_tensor<uchar>(0), image.data, image.total() * image.elemSize());
    interpreter->SetAllowFp16PrecisionForFp32(true);
    interpreter->SetNumThreads(16);
    interpreter->Invoke();

    vector<float> out_scor;

    // Locate and interpret the results of clasification
    boxes = tfl.tfLocate(interpreter, cam_height, cam_width, out_scor);

    /**NON-MAXIMUM SUPPRESSION**/
    vector<float> new_scr = nms(frame, boxes, out_scor, 0.6);

    // Update locations of centroids
    auto objects = centroidTracker->update(boxes);

    if (!objects.empty())
    {
        for (auto obj : objects)
        {
            string ID = to_string(obj.first);
            int k = 1;
            
            // Showing ID
            putText(frame, ID, Point(obj.second.first - 10, obj.second.second - 10),
                    FONT_HERSHEY_COMPLEX, 0.5, Scalar(0, 255, 0), 2);

            for (int i = 0; i < centroidTracker->path_keeper[obj.first].size(); i++)
            {
                // Set centroid into the heatmap matrix
                if (h->getFrames() % 10 == 0)
                {
                    h->addCell(centroidTracker->path_keeper[obj.first][i].first, centroidTracker->path_keeper[obj.first][i].second);
                }
            }
        }
    }

    return frame;
}

void parser(int argc, const char **argv, cv::VideoCapture &vs)
{
    string argument;
    int id = -1;

    /// Use the cmdlineparser to process input arguments
    CommandLineParser parser(argc, argv,
                             "{ help h            |      | show this message }"
                             "{ video v           |      | (required) path to video }"
                             "{ ip i              |      | ip camera }"
                             "{ cam c             |      | webcam }"
                             "{ version           |      | version }");

    if (parser.has("help"))
    {
        parser.printMessage();
        exit(0);
    }
    else if (parser.has("video"))
    {
        string path;
        path = parser.get<string>("video");

        argument = "../videos/" + path;
    }
    else if (parser.has("ip"))
    {
        string ip;
        ip = parser.get<string>("ip");
        cout << ip + "\n";
        argument = "http://" + ip + "/video.cgi?mjpg";
    }
    else if (parser.has("cam"))
    {
        id = parser.get<int>("cam");
        cout << id << "\n";
        argument = "cam";
    }
    else if (parser.has("version"))
    {
        cout << "\033[1;44mVersion 0.3\033[0m\n";
        exit(0);
    }
    if (argument.empty())
    {
        parser.printMessage();
        exit(-1);
    }

    if (id != -1)
    {
        vs.open(id, CAP_ANY);
    }
    else
    {
        vs.open(argument, CAP_ANY);
    }
}

int main(int argc, const char **argv)
{

    int totalFrames = 0;
    initPublisher();
    VideoCapture vs;
    HeatMap *h;
    auto centroidTracker = new CentroidTracker(20);

    Mat frame, rez;

    /****************FPS********************/
    float FPS[16];
    chrono::steady_clock::time_point Tbegin, Tend;
    int i, Fcnt = 0;
    float f = 0.0f;
    /***************************************/

    parser(argc, argv, vs);

    cout << "\033[1;36m[INFO] opening video file...\033[0m\n";

    if (!vs.isOpened())
    {
        cerr << "No ha encontrado una imagen de entrada."
             << "\n";
        return 1;
    }

    /**************TENSORFLOW***************/

    std::unique_ptr<tflite::FlatBufferModel> model =
        tflite::FlatBufferModel::BuildFromFile(NET);
    // Build the interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::InterpreterBuilder(*model.get(), resolver)(&interpreter);

    std::vector<std::string> labels;

    ifstream input(LABELMAP);
    for (std::string line; getline(input, line);)
    {
        labels.push_back(line);
    }
    input.close();

    // Get image source size
    cam_width = vs.get(CAP_PROP_FRAME_WIDTH);
    cam_height = vs.get(CAP_PROP_FRAME_HEIGHT);

    h = new HeatMap(cam_width, cam_height);

    /***************************************/
    vs.read(frame); //Read each frame

    if (frame.empty())
    {
        cerr << "No ha se ha cargado ninguna imagen." << endl;
        exit(-1);
    }

    // Send background image
    sendImage(frame); 
    /***************************************/

    while (1)
    {

        vs.read(frame); //Read each frame

        if (frame.empty())
        {
            cerr << "No ha se ha cargado ninguna imagen." << endl;
            exit(-1);
        }

        Tbegin = chrono::steady_clock::now();

        rez = detectVideo(frame, m, centroidTracker, interpreter, labels, h);

        //--------------------------------------------------------------------
        Tend = chrono::steady_clock::now();
        f = chrono::duration_cast<chrono::milliseconds>(Tend - Tbegin).count();
        if (f > 0.0)
            FPS[((Fcnt++) & 0x0F)] = 1000.0 / f;
        for (f = 0.0, i = 0; i < 16; i++)
        {
            f += FPS[i];
        }
        //--------------------------------------------------------------------

        if (h->getFrames() % 200 == 0){
		            publicar(cam_width / 10, cam_height / 10, h->getTotal(), h->getMatr());
	    }

        ++totalFrames;
        h->countFrame();

        imshow("tracker", rez);
        //h->printMatrix();
        char esc = waitKey(5);
        if (esc == 27)
            break;
    }

    finishPublisher();
    destroyAllWindows();
    return 0;
}
