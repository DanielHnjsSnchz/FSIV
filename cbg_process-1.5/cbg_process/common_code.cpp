#include "common_code.hpp"

cv::Mat
fsiv_convert_image_byte_to_float(const cv::Mat& img, cv::Mat& out)
{
    CV_Assert(img.depth()==CV_8U);
    //TODO
    img.convertTo(out, CV_32F,1.0/255.0,0.0);
    //
    CV_Assert(out.rows==img.rows && out.cols==img.cols);
    CV_Assert(out.depth()==CV_32F);
    CV_Assert(img.channels()==out.channels());
    return out;
}

cv::Mat
fsiv_convert_image_float_to_byte(const cv::Mat& img, cv::Mat& out)
{
    CV_Assert(img.depth()==CV_32F);
    //TODO
    img.convertTo(out, CV_8U,255.0,0.0);
    //
    CV_Assert(out.rows==img.rows && out.cols==img.cols);
    CV_Assert(out.depth()==CV_8U);
    CV_Assert(img.channels()==out.channels());
    return out;
}

cv::Mat
fsiv_convert_bgr_to_hsv(const cv::Mat& img, cv::Mat& out)
{
    CV_Assert(img.channels()==3);
    //TODO
    cv::cvtColor(img,out, cv::COLOR_BGR2HSV);
    //
    CV_Assert(out.channels()==3);
    return out;
}

cv::Mat
fsiv_convert_hsv_to_bgr(const cv::Mat& img, cv::Mat& out)
{
    CV_Assert(img.channels()==3);
    //TODO
    cv::cvtColor(img,out, cv::COLOR_HSV2BGR);
    //
    CV_Assert(out.channels()==3);
    return out;
}

cv::Mat
fsiv_cbg_process (const cv::Mat & in, cv::Mat& out,
             double contrast, double brightness, double gamma,
             bool only_luma)
{
    CV_Assert(in.depth()==CV_8U);
    //TODO
    //Recuerda: es recomendable trabajar en flotante [0,1]
    //Después deshacer el cambio a byte [0,255]
    //Recuerda: usa código vectorizado.
    std::vector<cv::Mat> canales;
    cv::Mat auxMat;
    cv::Mat gammaPower;

    //auxMat=fsiv_convert_image_byte_to_float(in,out);    

    if(only_luma==false){
            auxMat=fsiv_convert_image_byte_to_float(in,out);
            cv::pow(auxMat,gamma,gammaPower);
            //cv::Scalar b =cv::Scalar(brightness,0,0);
            out=(contrast*gammaPower+brightness);
            out = fsiv_convert_image_float_to_byte(out,out);
        
    }
    else{

            auxMat=fsiv_convert_image_byte_to_float(in,out);
            auxMat=fsiv_convert_bgr_to_hsv(auxMat,out);

            cv::split(auxMat,canales);

            cv::pow(canales[2],gamma,gammaPower);

            canales[2]=(contrast*gammaPower+brightness);

            cv::merge(canales,auxMat);


            out = fsiv_convert_hsv_to_bgr(auxMat,out);
            out = fsiv_convert_image_float_to_byte(out,out);

    }

    

    //
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    CV_Assert(out.depth()==CV_8U);
    CV_Assert(out.channels()==in.channels());
    return out;
}
