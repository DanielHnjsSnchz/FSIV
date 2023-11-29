#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include "common_code.hpp"

std::vector<cv::Point3f>
fsiv_generate_3d_calibration_points(const cv::Size& board_size,
                                    float square_size)
{
    std::vector<cv::Point3f> ret_v;
    //TODO
    //Remember: the first inner point has (1,1) in board coordinates.

    //hacemos lo mismo que en el ejercicio 1

    for(int i = 1; i<=board_size.height; i++){

        for(int j = 1; j<=board_size.width; j++){
            ret_v.push_back(cv::Point3f(j*square_size, i*square_size,0));
        }

    }

    //
    CV_Assert(ret_v.size() == 
        static_cast<size_t>(board_size.width*board_size.height));
    return ret_v;
}


bool
fsiv_fast_find_chessboard_corners(const cv::Mat& img, const cv::Size &board_size,
                             std::vector<cv::Point2f>& corner_points)
{
    CV_Assert(img.type()==CV_8UC3);
    bool was_found = false;
    // TODO
    // Hint: use cv::findChessboardCorners adding fast check to the defaults flags.
    
    was_found = cv::findChessboardCorners(img, board_size, corner_points);

    cv::drawChessboardCorners(img, board_size, corner_points, was_found);
    
    /*
    cv::imshow("Tablero", img);

    int k = cv::waitKey() & 0xff;
    */
    //
    return was_found;
}


void fsiv_compute_camera_pose(const std::vector<cv::Point3f> &_3dpoints,
                              const std::vector<cv::Point2f> &_2dpoints,
                              const cv::Mat& camera_matrix,
                              const cv::Mat& dist_coeffs,
                              cv::Mat& rvec,
                              cv::Mat& tvec)
{
    CV_Assert(_3dpoints.size()>=4 && _3dpoints.size()==_2dpoints.size());
    // TODO
    // Hint: use cv::solvePnP to the pose of a calibrated camera.
    cv::solvePnP(_3dpoints, _2dpoints, camera_matrix, dist_coeffs,rvec,tvec);
    //
    CV_Assert(rvec.rows==3 && rvec.cols==1 && rvec.type()==CV_64FC1);
    CV_Assert(tvec.rows==3 && tvec.cols==1 && tvec.type()==CV_64FC1);
}

void
fsiv_draw_axes(cv::Mat& img,
               const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs,
               const cv::Mat& rvec, const cv::Mat& tvec,
               const float size, const int line_width)
{
    // TODO
    // Hint: use cv::projectPoints to get the image coordinates of the 3D points
    // (0,0,0), (size, 0, 0), (0, size, 0) and (0, 0, -size) and draw a line for
    // each axis: blue for axis OX, green for axis OY and red for axis OZ.
    // Warning: use of cv::drawFrameAxes() is not allowed.

    std::vector<cv::Point2f> img_points;
    std::vector<cv::Point3f> points;

    points.push_back(cv::Point3f(0,0,0));
    points.push_back(cv::Point3f(size,0,0));
    points.push_back(cv::Point3f(0,size,0));
    points.push_back(cv::Point3f(0,0,-size));

    cv::projectPoints(points, rvec, tvec, camera_matrix, dist_coeffs, img_points);
    

    //FORMATO BGR!!
    cv::line(img, img_points[0], img_points[1], cv::Scalar(255,0,0), line_width);
    cv::line(img, img_points[0], img_points[2], cv::Scalar(0,255,0), line_width);
    cv::line(img, img_points[0], img_points[3], cv::Scalar(0,0,255), line_width);

    //
}

void
fsiv_load_calibration_parameters(cv::FileStorage &fs,
                                 cv::Size &camera_size,
                                 float& error,
                                 cv::Mat& camera_matrix,
                                 cv::Mat& dist_coeffs,
                                 cv::Mat& rvec,
                                 cv::Mat& tvec)
{
    CV_Assert(fs.isOpened());
    //TODO
    // Hint: use fs["label"] >> var to load data items from the file.
    // @see cv::FileStorage operators "[]" and ">>"

    fs["image-width"] >> camera_size.width;
    fs["image-height"] >> camera_size.height;
    fs["error"] >> error;
    fs["camera-matrix"] >> camera_matrix;
    fs["distortion-coefficients"] >> dist_coeffs;
    fs["rvec"] >> rvec;
    fs["tvec"] >> tvec;

    //
    CV_Assert(fs.isOpened());
    CV_Assert(camera_matrix.type()==CV_64FC1 && camera_matrix.rows==3 && camera_matrix.cols==3);
    CV_Assert(dist_coeffs.type()==CV_64FC1 && dist_coeffs.rows==1 && dist_coeffs.cols==5);
    CV_Assert(rvec.type()==CV_64FC1 && rvec.rows==3 && rvec.cols==1);
    CV_Assert(tvec.type()==CV_64FC1 && tvec.rows==3 && tvec.cols==1);
    return;
}

void
fsiv_draw_3d_model(cv::Mat &img, const cv::Mat& M, const cv::Mat& dist_coeffs,
                   const cv::Mat& rvec, const cv::Mat& tvec,
                   const float size)
{
    CV_Assert(img.type()==CV_8UC3);

    // TODO
    // Hint: build a 3D object points vector with pair of segments end points.
    // Use cv::projectPoints to get the 2D image coordinates of 3D object points,
    // build a vector of vectors of Points, one for each segment, and use
    // cv::polylines to draw the wire frame projected model.
    
    //cv::polylines()    

    std::vector<cv::Point2f> img_pointsC1,img_pointsC2,img_medioC1,img_medioC2,img_cono1;
    std::vector<cv::Point3f> pointsC1,medioC1;
    std::vector<cv::Point3f> pointsC2,medioC2,cono1;

    float radius = 0.5 * size;  // Radio del arco
    
    //
    //TORRE N1
    //

    // Circulo 1
    for (int i = 0; i <= 360; i += 10) {
        float theta = static_cast<float>(i) * CV_PI / 180.0;
        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        pointsC1.push_back(cv::Point3f(x+size, y+size, 0));
        if(i==180)
            medioC1.push_back(cv::Point3f(x+size, y+size, 0));

    }

    // Circulo 2
    for (int i = 0; i <= 360; i += 10) {
        float theta = static_cast<float>(i) * CV_PI / 180.0;
        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        pointsC2.push_back(cv::Point3f(x+size, y+size, -1.5*size));
        if(i==180)
            medioC2.push_back(cv::Point3f(x+size, y+size, -1.5*size));
        /*if(i==270)
            medioC2.push_back(cv::Point3f(x+2*size, y+2*size, -1.5*size));*/
    }

    cono1.push_back(cv::Point3f((medioC2.front()).x+radius,(medioC2.front()).y,(medioC2.front()).z-1.25*size));

    cv::projectPoints(pointsC1, rvec, tvec, M, dist_coeffs, img_pointsC1);
    cv::projectPoints(pointsC2, rvec, tvec, M, dist_coeffs, img_pointsC2);

    cv::projectPoints(medioC1, rvec, tvec, M, dist_coeffs, img_medioC1);
    cv::projectPoints(medioC2, rvec, tvec, M, dist_coeffs, img_medioC2);

    cv::projectPoints(cono1, rvec, tvec, M, dist_coeffs, img_cono1);

    for (size_t i = 0; i < img_pointsC1.size() - 1; ++i) {
        cv::line(img, img_pointsC1[i], img_pointsC1[i + 1], cv::Scalar(0, 255, 0), 3);
    }

    for (size_t i = 0; i < img_pointsC2.size() - 1; ++i) {
        cv::line(img, img_pointsC2[i], img_pointsC2[i + 1], cv::Scalar(0, 255, 0), 3);
    }

    cv::line(img, img_pointsC1.front(), img_pointsC2.front(), cv::Scalar(0, 255, 0), 3);

    cv::line(img, img_medioC1.front(), img_medioC2.front(), cv::Scalar(0, 255, 0), 3);

    cv::line(img, img_cono1.front(), img_medioC2.front(), cv::Scalar(0, 0, 255), 3);

    cv::line(img, img_cono1.front(), img_pointsC2.front(), cv::Scalar(0, 0, 255), 3);

    
    //
    //TORRE N2
    //


    std::vector<cv::Point2f> img_pointsC3,img_pointsC4,img_medioC3,img_medioC4,img_cono2;
    std::vector<cv::Point3f> pointsC3,medioC3;
    std::vector<cv::Point3f> pointsC4,medioC4,cono2;


    // Circulo 1
    for (int i = 0; i <= 360; i += 10) {
        float theta = static_cast<float>(i) * CV_PI / 180.0;
        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        pointsC3.push_back(cv::Point3f(x+5*size, y+size, 0));
        if(i==180)
            medioC3.push_back(cv::Point3f(x+5*size, y+size, 0));

    }

    // Circulo 2
    for (int i = 0; i <= 360; i += 10) {
        float theta = static_cast<float>(i) * CV_PI / 180.0;
        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        pointsC4.push_back(cv::Point3f(x+5*size, y+size, -1.5*size));
        if(i==180)
            medioC4.push_back(cv::Point3f(x+5*size, y+size, -1.5*size));
    }

    cono2.push_back(cv::Point3f((medioC4.front()).x+radius,(medioC4.front()).y,(medioC4.front()).z-1.25*size));

    cv::projectPoints(pointsC3, rvec, tvec, M, dist_coeffs, img_pointsC3);
    cv::projectPoints(pointsC4, rvec, tvec, M, dist_coeffs, img_pointsC4);

    cv::projectPoints(medioC3, rvec, tvec, M, dist_coeffs, img_medioC3);
    cv::projectPoints(medioC4, rvec, tvec, M, dist_coeffs, img_medioC4);

    cv::projectPoints(cono2, rvec, tvec, M, dist_coeffs, img_cono2);

    for (size_t i = 0; i < img_pointsC3.size() - 1; ++i) {
        cv::line(img, img_pointsC3[i], img_pointsC3[i + 1], cv::Scalar(0, 255, 0), 3);
    }

    for (size_t i = 0; i < img_pointsC4.size() - 1; ++i) {
        cv::line(img, img_pointsC4[i], img_pointsC4[i + 1], cv::Scalar(0, 255, 0), 3);
    }

    cv::line(img, img_pointsC3.front(), img_pointsC4.front(), cv::Scalar(0, 255, 0), 3);

    cv::line(img, img_medioC3.front(), img_medioC4.front(), cv::Scalar(0, 255, 0), 3);

    cv::line(img, img_cono2.front(), img_medioC4.front(), cv::Scalar(0, 0, 255), 3);

    cv::line(img, img_cono2.front(), img_pointsC4.front(), cv::Scalar(0, 0, 255), 3);


    //
    //IGLESIA
    //


    std::vector<cv::Point2f> img_points;
    std::vector<cv::Point3f> points;

    float half_square = size/2.0;

    points.push_back(cv::Point3f(size,2*size,0));
    points.push_back(cv::Point3f(5*size,2*size,0));
    points.push_back(cv::Point3f(size,3*size,0));
    points.push_back(cv::Point3f(5*size,3*size,0));
 
    points.push_back(cv::Point3f(2*size,2*size,-size));
    points.push_back(cv::Point3f(5*size,2*size,-size));
    points.push_back(cv::Point3f(2*size,3*size,-size));
    points.push_back(cv::Point3f(5*size,3*size,-size));    

    points.push_back(cv::Point3f(size,2*size,-2*size));
    points.push_back(cv::Point3f(2*size,2*size,-2*size));
    points.push_back(cv::Point3f(size,3*size,-2*size));
    points.push_back(cv::Point3f(2*size,3*size,-2*size));
    points.push_back(cv::Point3f(2*size,2*size + half_square,-2*size));
    points.push_back(cv::Point3f(5*size,2*size + half_square,-2*size));

    points.push_back(cv::Point3f(1*size + half_square,2*size + half_square,-3*size));

    cv::projectPoints(points, rvec, tvec, M, dist_coeffs, img_points);

    cv::line(img, img_points[0], img_points[1], cv::Scalar(255,5,0), 3);
    cv::line(img, img_points[0], img_points[2], cv::Scalar(255,5,0), 3);
    cv::line(img, img_points[2], img_points[3], cv::Scalar(255,5,0), 3);
    cv::line(img, img_points[1], img_points[3], cv::Scalar(255,5,0), 3);

    cv::line(img, img_points[4], img_points[5], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[6], img_points[7], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[5], img_points[7], cv::Scalar(0,0,255), 3);
    
    cv::line(img, img_points[1], img_points[5], cv::Scalar(255,5,0), 3);
    cv::line(img, img_points[3], img_points[7], cv::Scalar(255,5,0), 3);

    cv::line(img, img_points[8], img_points[9], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[8], img_points[10], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[9], img_points[11], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[10], img_points[11], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[12], img_points[13], cv::Scalar(0,0,255), 3);

    cv::line(img, img_points[12], img_points[4], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[12], img_points[6], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[13], img_points[5], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[13], img_points[7], cv::Scalar(0,0,255), 3);

    cv::line(img, img_points[0], img_points[8], cv::Scalar(255,5,0), 3);
    cv::line(img, img_points[4], img_points[9], cv::Scalar(255,5,0), 3);
    cv::line(img, img_points[2], img_points[10], cv::Scalar(255,5,0), 3);
    cv::line(img, img_points[6], img_points[11], cv::Scalar(255,5,0), 3);
 
    cv::line(img, img_points[14], img_points[8], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[14], img_points[9], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[14], img_points[10], cv::Scalar(0,0,255), 3);
    cv::line(img, img_points[14], img_points[11], cv::Scalar(0,0,255), 3);



    //
    //TORRE N3
    //


    std::vector<cv::Point2f> img_pointsC5,img_pointsC6,img_medioC5,img_medioC6,img_cono3;
    std::vector<cv::Point3f> pointsC5,medioC5;
    std::vector<cv::Point3f> pointsC6,medioC6,cono3;


    // Circulo 1
    for (int i = 0; i <= 360; i += 10) {
        float theta = static_cast<float>(i) * CV_PI / 180.0;
        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        pointsC5.push_back(cv::Point3f(x+size, y+4*size, 0));
        if(i==180)
            medioC5.push_back(cv::Point3f(x+size, y+4*size, 0));

    }

    // Circulo 2
    for (int i = 0; i <= 360; i += 10) {
        float theta = static_cast<float>(i) * CV_PI / 180.0;
        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        pointsC6.push_back(cv::Point3f(x+size, y+4*size, -1.5*size));
        if(i==180)
            medioC6.push_back(cv::Point3f(x+size, y+4*size, -1.5*size));
        /*if(i==270)
            medioC2.push_back(cv::Point3f(x+2*size, y+2*size, -1.5*size));*/
    }

    cono3.push_back(cv::Point3f((medioC6.front()).x+radius,(medioC6.front()).y,(medioC6.front()).z-1.25*size));

    cv::projectPoints(pointsC5, rvec, tvec, M, dist_coeffs, img_pointsC5);
    cv::projectPoints(pointsC6, rvec, tvec, M, dist_coeffs, img_pointsC6);

    cv::projectPoints(medioC5, rvec, tvec, M, dist_coeffs, img_medioC5);
    cv::projectPoints(medioC6, rvec, tvec, M, dist_coeffs, img_medioC6);

    cv::projectPoints(cono3, rvec, tvec, M, dist_coeffs, img_cono3);

    for (size_t i = 0; i < img_pointsC5.size() - 1; ++i) {
        cv::line(img, img_pointsC5[i], img_pointsC5[i + 1], cv::Scalar(0, 255, 0), 3);
    }

    for (size_t i = 0; i < img_pointsC6.size() - 1; ++i) {
        cv::line(img, img_pointsC6[i], img_pointsC6[i + 1], cv::Scalar(0, 255, 0), 3);
    }

    cv::line(img, img_pointsC5.front(), img_pointsC6.front(), cv::Scalar(0, 255, 0), 3);

    cv::line(img, img_medioC5.front(), img_medioC6.front(), cv::Scalar(0, 255, 0), 3);

    cv::line(img, img_cono3.front(), img_medioC6.front(), cv::Scalar(0, 0, 255), 3);

    cv::line(img, img_cono3.front(), img_pointsC6.front(), cv::Scalar(0, 0, 255), 3);



    //
    //TORRE N4
    //


    std::vector<cv::Point2f> img_pointsC7,img_pointsC8,img_medioC7,img_medioC8,img_cono4;
    std::vector<cv::Point3f> pointsC7,medioC7;
    std::vector<cv::Point3f> pointsC8,medioC8,cono4;


    // Circulo 1
    for (int i = 0; i <= 360; i += 10) {
        float theta = static_cast<float>(i) * CV_PI / 180.0;
        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        pointsC7.push_back(cv::Point3f(x+5*size, y+4*size, 0));
        if(i==180)
            medioC7.push_back(cv::Point3f(x+5*size, y+4*size, 0));

    }

    // Circulo 2
    for (int i = 0; i <= 360; i += 10) {
        float theta = static_cast<float>(i) * CV_PI / 180.0;
        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        pointsC8.push_back(cv::Point3f(x+5*size, y+4*size, -1.5*size));
        if(i==180)
            medioC8.push_back(cv::Point3f(x+5*size, y+4*size, -1.5*size));
        /*if(i==270)
            medioC2.push_back(cv::Point3f(x+2*size, y+2*size, -1.5*size));*/
    }

    cono4.push_back(cv::Point3f((medioC8.front()).x+radius,(medioC8.front()).y,(medioC8.front()).z-1.25*size));

    cv::projectPoints(pointsC7, rvec, tvec, M, dist_coeffs, img_pointsC7);
    cv::projectPoints(pointsC8, rvec, tvec, M, dist_coeffs, img_pointsC8);

    cv::projectPoints(medioC7, rvec, tvec, M, dist_coeffs, img_medioC7);
    cv::projectPoints(medioC8, rvec, tvec, M, dist_coeffs, img_medioC8);

    cv::projectPoints(cono4, rvec, tvec, M, dist_coeffs, img_cono4);

    for (size_t i = 0; i < img_pointsC7.size() - 1; ++i) {
        cv::line(img, img_pointsC7[i], img_pointsC7[i + 1], cv::Scalar(0, 255, 0), 3);
    }

    for (size_t i = 0; i < img_pointsC8.size() - 1; ++i) {
        cv::line(img, img_pointsC8[i], img_pointsC8[i + 1], cv::Scalar(0, 255, 0), 3);
    }

    cv::line(img, img_pointsC7.front(), img_pointsC8.front(), cv::Scalar(0, 255, 0), 3);

    cv::line(img, img_medioC7.front(), img_medioC8.front(), cv::Scalar(0, 255, 0), 3);

    cv::line(img, img_cono4.front(), img_medioC8.front(), cv::Scalar(0, 0, 255), 3);

    cv::line(img, img_cono4.front(), img_pointsC8.front(), cv::Scalar(0, 0, 255), 3);




    //
}

void
fsiv_project_image(const cv::Mat& input, cv::Mat& output,
                   const cv::Size& board_size,
                   const std::vector<cv::Point2f>& chess_board_corners)
{
    CV_Assert(!input.empty() && input.type()==CV_8UC3);
    CV_Assert(!output.empty() && output.type()==CV_8UC3);
    CV_Assert(static_cast<size_t>(board_size.area()) == 
        chess_board_corners.size());
    
    // TODO
    // Hint: get the upper-left, upper-right, bottom-right and bottom-left 
    //   chess_board_corners and map to the upper-left, upper-right, bottom-right 
    //   and bottom-left input image point coordinates. 
    //   Use cv::getPerspectiveTransform compute such mapping.
    // Hint: use cv::wrapPerspective to get a wrap version of the input image
    //   using the computed mapping. Use INTER_LINEAR as interpolation method 
    //   and use BORDER_TRANSPARENT as a border extrapolation method 
    //   to maintain the underlying image.
    // 

    std::vector<cv::Point2f> input_corners;
    input_corners.push_back(cv::Point2f(0, 0));
    input_corners.push_back(cv::Point2f(input.cols-1, 0));
    input_corners.push_back(cv::Point2f(0, input.rows-1));
    input_corners.push_back(cv::Point2f(input.cols-1, input.rows-1));

    std::vector<cv::Point2f> board_corners;
    board_corners.push_back(chess_board_corners[0]);
    board_corners.push_back(chess_board_corners[board_size.width-1]);
    board_corners.push_back(chess_board_corners[(board_size.height-1) * board_size.width]);
    board_corners.push_back(chess_board_corners[board_size.height * board_size.width - 1]);

    cv::Mat M = cv::getPerspectiveTransform(input_corners, board_corners);
    cv::Mat aux = output.clone();

    cv::warpPerspective(input, output, M, aux.size(),cv::INTER_LINEAR,cv::BORDER_TRANSPARENT);


    //
}
