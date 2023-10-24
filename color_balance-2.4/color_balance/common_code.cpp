#include "common_code.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

cv::Mat fsiv_color_rescaling(const cv::Mat& in, const cv::Scalar& from, const cv::Scalar& to)
{
    CV_Assert(in.type()==CV_8UC3);
    cv::Mat out;
    //TODO
    //Cuidado con dividir por cero.
    //Evita los bucles.

    cv::Scalar fact;
    cv::divide(to,from,fact);
    cv::multiply(fact,in,out);//quizas cambiar orden de fact e in

    /*
    Aqui se hace la operacion O=axI que aparece en el enunciado de la practica
    En el divide se hace a
    y multiply será el producto de a por I

    O =   I con =(255/W*R , 255/W*G , 255/W*B).
    */

    //
    CV_Assert(out.type()==in.type());
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    return out;
}

cv::Mat fsiv_wp_color_balance(cv::Mat const& in)
{
    CV_Assert(in.type()==CV_8UC3);
    cv::Mat out;
    //TODO
    //Sugerencia: utiliza el espacio de color GRAY para
    //saber la ilumimancia de un pixel.
    cv::Point max;
    cv::Mat auxMat;
    cv::cvtColor(in,auxMat,cv::COLOR_BGR2GRAY);
    //pasamos de in a una matriz auxiliar en formato GRAY

    cv::minMaxLoc(auxMat,NULL,NULL,NULL,&max);//aunque en la documentacion de opencv aparezcan 6 parametros 
    //el ultimo no hace falta ponerlo, pero si pondremos el *Point maxLoc
    //Localizamos el maximo dentro de auxMat, es decir de la imagen en escala de grises y guardamos en max
    //cv::Scalar mediana = cv::mean(auxMat);

    cv::Scalar color_base = in.at<cv::Vec3b>(max);
    //cogemos el punto en in(imagen de entrada en formato BGR donde se encunetre el punto con el valor maximo)

    out = fsiv_color_rescaling(in, color_base, cv::Scalar(255, 255, 255));
    /*le pasamos a la funcion de reescalado la imagen de entrada
    el punto de referencia blanco
    y le decimos que lo convierta a blanco cv::Scalar(255, 255, 255)
    */
    //
    CV_Assert(out.type()==in.type());
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    return out;
}

cv::Mat fsiv_gw_color_balance(cv::Mat const& in)
{
    CV_Assert(in.type()==CV_8UC3);
    cv::Mat out;
    //TODO
    //cv::Mat auxMat;
    //cv::cvtColor(in,auxMat,cv::COLOR_BGR2GRAY);
    cv::Scalar mean =cv::mean(in); 
    cv::Scalar color_base = mean;

    out = fsiv_color_rescaling(in, color_base, cv::Scalar(128, 128, 128));

    //
    CV_Assert(out.type()==in.type());
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    return out;
}

cv::Mat fsiv_color_balance(cv::Mat const& in, float p)
{
    CV_Assert(in.type()==CV_8UC3);
    CV_Assert(0.0f<p && p<100.0f);
    cv::Mat out;
    //TODO
    //Sugerencia: utiliza el espacio de color GRAY para
    //saber la ilumimancia de un pixel.
    //in.convertTo(in, CV_32F,1.0/255.0,0.0);

    cv::Mat hist = cv::Mat::zeros(256, 1, CV_32FC1);

    cv::Mat auxMat;
    cv::cvtColor(in,auxMat,cv::COLOR_BGR2GRAY);

    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    cv::calcHist(&auxMat, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);
    cv::normalize(hist, hist, 1.0, 0.0, cv::NORM_L1);
    //Calcular histograma y normalizarlo

    for(int i = 1 ; i < hist.rows; i++){
        hist.at<float>(i) = hist.at<float>(i) + hist.at<float>(i - 1);
    }
    //Esto acumula el histograma

    float maxValue = 0;
    for (int i = 0; i < hist.rows; i++){
        if (hist.at<float>(i) >= (1 - p/100)){
            maxValue = i;
            break;
        }
    }
    //coger el valor maximo en el histograma en el p
    //p.e: si el percentil cae en el valor 234 me guarda 234 y luego solo cogere a partir de ese en la media


    cv::Mat mask = auxMat>=maxValue;
    //crear una mascar con los valores de auxMat a partir 
    //del valor del percentil dado y el resto a cero
    
    cv::Scalar color_base =cv::mean(in, mask); 

    out = fsiv_color_rescaling(in, color_base, cv::Scalar(255, 255, 255));

    //auxMat.convertTo(out, CV_8U,255.0,0.0);

    //calcular histograma,calcular percentil con el porcentaje dado y 
    //posteriormente todo lo que sea menor que los valores 
    //del percentil ponerlo a cero y esa será mi mascara.
    //Luego calcular media con la mascara y hacer el erescalado


    //Para calcular el punto de mayor iluminacion : Y=0.299R +0.587G + 0.114B

    //Valor promedio W obtenido a partir de un porcentaje P

    //
    CV_Assert(out.type()==in.type());
    CV_Assert(out.rows==in.rows && out.cols==in.cols);
    return out;
}
