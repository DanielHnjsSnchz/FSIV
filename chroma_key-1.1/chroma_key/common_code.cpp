#include <iostream>
#include "common_code.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

cv::Mat
fsiv_combine_images(const cv::Mat& foreground, const cv::Mat& background,
               const cv::Mat& mask)
{
    CV_Assert(background.size()==foreground.size());
    CV_Assert(background.type()==foreground.type());
    CV_Assert(mask.size() == foreground.size());
    cv::Mat output;
    //TODO
    // HINT: you can copy the backg on the foreg using the mask.
    background.copyTo(output);
    foreground.copyTo(output, mask);
    //se copia foreground a output y posteriormente el fondo con la mascara
    //
    CV_Assert(output.size() == foreground.size());
    CV_Assert(output.type()==foreground.type());
    return output;
}

cv::Mat
fsiv_create_mask_from_hsv_range(const cv::Mat& img,
                                 const cv::Scalar& lower_bound,
                                 const cv::Scalar& upper_bound)
{
    CV_Assert(img.channels()==3);
    cv::Mat mask;

    //TODO
    //Hint: use cv::cvtColor to change to HSV color space.
    //Hint: use cv::inRange to get the mask.
    cv::Mat hsvImg;
    cv::cvtColor(img,hsvImg, cv::COLOR_RGB2HSV);
    cv::inRange(hsvImg,lower_bound,upper_bound,mask);
    //
    CV_Assert(mask.size()==img.size());
    CV_Assert(mask.depth()==CV_8U);
    return mask;
}

cv::Mat
fsiv_apply_chroma_key(const cv::Mat &foreg, const cv::Mat& backg, int hue,
                           int sensitivity)
{
    cv::Mat out;
    cv::Scalar lower_b, upper_b; //HSV range.

    //TODO
    //Hint: use fsiv_xxx defined functions.
    //Hint: use all range for channels S and V.
    //Remember: the backg img must have the same dimensions to combine with
    //  foreg img. You can use cv::resize to assure this.

    cv::Mat newBackg;
    backg.copyTo(newBackg);

    if(foreg.size()!=backg.size()){
        cv::resize(backg,newBackg,foreg.size()/*,0,0,cv::INTER_AREA*/);
    }

    lower_b=cv::Scalar(hue-sensitivity,0,0);
    upper_b=cv::Scalar(hue+sensitivity,255,255);
    //dar valores a los escalares a parte de este en minimo y maximo rango

    cv::Mat mask;

    mask=fsiv_create_mask_from_hsv_range(foreg,lower_b,upper_b);
    mask=255-mask;
    out=fsiv_combine_images(foreg,newBackg,mask);
    //
    CV_Assert(out.size()==foreg.size());
    CV_Assert(out.type()==foreg.type());
    return out;
}
