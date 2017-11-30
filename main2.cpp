//
// Created by vmachado on 16/11/17.
//

#include <opencv2/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace cv::dnn;

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <opencv/cv.hpp>

using namespace std;


const size_t inWidth = 300;
const size_t inHeight = 300;
const float WHRatio = inWidth / (float)inHeight;
const float inScaleFactor = 0.007843f;
const float meanVal = 127.5;
const char* classNames[] = {"", "ship", "batea"};

const char* about = "This sample uses Single-Shot Detector "
        "(https://arxiv.org/abs/1512.02325)"
        "to detect objects on image.\n"
        ".caffemodel model's file is avaliable here: "
        "https://github.com/chuanqi305/MobileNet-SSD/blob/master/MobileNetSSD_train.caffemodel\n";

const char* params
        = "{ help                 | false | print usage               }"
                "{ pb             |    -- | model configuration       }"
                "{ pbtxt          |    -- | model configuration       }"
                "{ in             |       | image for detection       }"
                "{ out            |       | path to output video file }"
                "{ min_confidence | 0.5   | min confidence            }";


//int main(){
//    VideoCapture cap("/dev/video0");
//    Mat img;
//    while(true){
//        cap >> img;
//        imshow("image", img);
//        waitKey(1);
//    }
//
//}


int main__(int argc, char** argv)
{
    cv::CommandLineParser parser(argc, argv, params);

    if (parser.get<bool>("help"))
    {
        cout << about << endl;
        parser.printMessage();
        return 0;
    }

    String modelBinary = parser.get<string>("pb");
    String modelConfiguration = parser.get<string>("pbtxt");
    String imageFile = parser.get<String>("in");

    //! [Initialize network]
    dnn::Net net = readNetFromTensorflow(modelBinary,modelConfiguration);
    auto lnames = net.getLayerNames();
    cout << "lnames.size: " << lnames.size() << endl;
    for(auto l : lnames) cout << "layer: " << l << endl;
    //! [Initialize network]

    VideoCapture cap;
    Mat frame;
    cap.open(imageFile);
    if(!cap.isOpened())
    {
        cout << "Couldn't open image or video: " << parser.get<String>("video") << endl;
        return -1;
    }
    Size inVideoSize;
    inVideoSize = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH),    //Acquire input size
                       (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));

//    Size cropSize;
//    if (inVideoSize.width / (float)inVideoSize.height > WHRatio)
//    {
//        cropSize = Size(static_cast<int>(inVideoSize.height * WHRatio),
//                        inVideoSize.height);
//    }
//    else
//    {
//        cropSize = Size(inVideoSize.width,
//                        static_cast<int>(inVideoSize.width / WHRatio));
//    }
//
//    Rect crop(Point((inVideoSize.width - cropSize.width) / 2,
//                    (inVideoSize.height - cropSize.height) / 2),
//              cropSize);



    for(;;) {
        cap >> frame;
         if (frame.empty()){
             waitKey();
             break;
        }
        imshow("frame", frame);

        //resize(frame, frame, Size(720, 480));
        cvtColor(frame, frame, CV_BGR2RGB);

        if (frame.channels() == 4)
            cvtColor(frame, frame, COLOR_BGRA2BGR);

        //! [Prepare blob]
        Mat inputBlob = blobFromImage(frame, inScaleFactor,
                                      Size(1080, 720), meanVal, false, false); //Convert Mat to batch of images
        //! [Prepare blob]

        //! [Set input blob]
        net.setInput(inputBlob); //set the network input
        //! [Set input blob]

        //! [Make forward pass]
        Mat detection = net.forward("detection_out"); //compute output
        //! [Make forward pass]

        vector<double> layersTimings;
        double freq = getTickFrequency() / 1000;
        double time = net.getPerfProfile(layersTimings) / freq;

        cout << detection.size[2] << " " <<  detection.size[3] << endl;
        Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

////////        frame = frame(crop);

        ostringstream ss;
        ss << "FPS: " << 1000/time << " ; time: " << time << " ms";
        putText(frame, ss.str(), Point(20,20), 0, 0.5, Scalar(0,0,255));
        cout << "Inference time, ms: " << time << endl;

        float confidenceThreshold = parser.get<float>("min_confidence");
        for(int i = 0; i < detectionMat.rows; i++)
        {
            for(auto j =0; j < detectionMat.cols; ++j){
                cout << j <<  " " << (size_t)(detectionMat.at<float>(i, j)) << endl;
            }
            float confidence = detectionMat.at<float>(i, 2);

            if(confidence > confidenceThreshold)
            {
                size_t objectClass = (size_t)(detectionMat.at<float>(i, 1));

                int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                ss.str("");
                ss << confidence;
                String conf(ss.str());

                Rect object(xLeftBottom, yLeftBottom, (xRightTop - xLeftBottom), (yRightTop - yLeftBottom));

                rectangle(frame, object, Scalar(0, 255, 0));
                String label = String(classNames[objectClass]) + ": " + conf;
                int baseLine = 0;
                Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
                rectangle(frame, Rect(Point(xLeftBottom, yLeftBottom - labelSize.height),
                                      Size(labelSize.width, labelSize.height + baseLine)),
                          Scalar(255, 255, 255), CV_FILLED);
                putText(frame, label, Point(xLeftBottom, yLeftBottom),
                        FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0));
            }
        }

        imshow("detections", frame);
        waitKey(0);
//        if (waitKey(1) >= 0) break;
    }

    return 0;
} // main