#include <opencv2/opencv.hpp>
#include <vitis/ai/runner.hpp>
#include <vitis/ai/profiler.hpp>
#include <vitis/ai/tensor.hpp>
#include <iostream>
#include <memory>
#include <vector>

using namespace cv;
using namespace std;
using namespace vitis::ai;

int main(int argc, char **argv) {
    // Check if model path is provided
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <compiled_model.xmodel>" << endl;
        return -1;
    }

    // Load the compiled Vitis AI model (XIR format)
    string model_path = argv[1];
    auto runner = Runner::create_runner(model_path, false);  // false means CPU mode

    // Access the webcam
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error: Could not access the webcam." << endl;
        return -1;
    }

    cout << "Press 'q' to quit the application." << endl;

    while (true) {
        Mat frame;
        cap >> frame;

        if (frame.empty()) {
            cerr << "Failed to grab frame. Exiting..." << endl;
            break;
        }

        // Define Region of Interest (ROI) - center rectangle
        int height = frame.rows;
        int width = frame.cols;
        int roi_x1 = static_cast<int>(width * 0.3);
        int roi_y1 = static_cast<int>(height * 0.3);
        int roi_x2 = static_cast<int>(width * 0.7);
        int roi_y2 = static_cast<int>(height * 0.7);

        // Draw rectangle on the frame
        rectangle(frame, Point(roi_x1, roi_y1), Point(roi_x2, roi_y2), Scalar(0, 255, 0), 2);

        // Extract the ROI from the frame
        Mat roi = frame(Rect(roi_x1, roi_y1, roi_x2 - roi_x1, roi_y2 - roi_y1));

        // Preprocess the ROI
        Mat resized_roi;
        resize(roi, resized_roi, Size(224, 224));  // Resize to 224x224 for the model input
        resized_roi.convertTo(resized_roi, CV_32F, 1.0 / 255);  // Normalize to [0, 1]

        // Prepare the tensor from the preprocessed image
        auto input_tensor = runner->get_input_tensor(0);
        input_tensor->data().set_data(resized_roi.data);

        // Run inference
        auto output_tensors = runner->run({input_tensor});

        // Get the result and predict the class
        auto output_tensor = output_tensors[0];
        auto result = output_tensor->data().get_data<float>();
        int predicted_class = std::max_element(result, result + output_tensor->get_shape()[0]) - result;

        // Display result
        vector<string> class_messages = {
            "You have a 50 rupees note.",
            "You have a 500 rupees note.",
            "You have a 100 rupees note.",
            "You have a 10 rupees note.",
            "You have a 20 rupees note.",
            "You have a 200 rupees note."
        };
        
        string predicted_message = class_messages[predicted_class];
        putText(frame, predicted_message, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

        // Display the frame with prediction
        imshow("Currency Note Detection", frame);

        // Exit on 'q' key press
        if (waitKey(1) == 'q') {
            break;
        }
    }

    // Release resources
    cap.release();
    destroyAllWindows();

    return 0;
}
