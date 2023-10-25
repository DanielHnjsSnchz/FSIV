#include "common_code.hpp"

cv::Mat
fsiv_create_box_filter(const int r)
{
    CV_Assert(r>0);
    cv::Mat ret_v;
    // TODO
    // Hint: use the constructor of cv::Mat to set the proper initial value.
    ret_v = cv::Mat::ones(2*r+1,2*r+1,CV_32FC1);
    cv::normalize(ret_v,ret_v,1.0,0.0,cv::NORM_L1);
    //
    CV_Assert(ret_v.type()==CV_32FC1);
    CV_Assert(ret_v.rows==(2*r+1) && ret_v.rows==ret_v.cols);
    CV_Assert(std::abs(1.0-cv::sum(ret_v)[0])<1.0e-6);
    return ret_v;
}

cv::Mat
fsiv_create_gaussian_filter(const int r)
{
    CV_Assert(r>0);
    cv::Mat ret_v;
    // TODO
    // Remember: 6*sigma is approx 99,73% of the distribution.
    // Remember: scale the filter coefficients to sum 1.

    float sigma = ((float)(2*r+1))/6.0;
    ret_v = cv::Mat::zeros(2*r+1, 2*r+1, CV_32F);

    /*
    ● Filtros paso baja: Gaussian Filter.
    w (x , y)≈ 1/(σ √ 2 π)* exp [ −( x^2 + y^2 )/(2σ^2) ])
    */

    for (int i = r; i >= -r; --i){
        for(int j = -r; j <= r; ++j){
            //−( x^2 + y^2 )
            float dividendo = - (std::pow(i, 2) + std::pow(j, 2));
            //−( x^2 + y^2 )/(2σ^2)
            float divisor = 2 * std::pow(sigma, 2);
            //exp [ −( x^2 + y^2 )/(2σ^2) ]
            ret_v.at<float>(i+r, j+r) = std::exp(dividendo/divisor);
            
        }
    }

    //1/(σ √ 2 π)--> esto es para normalizar
    cv::normalize(ret_v,ret_v,1.0,0.0,cv::NORM_L1);

    //
    CV_Assert(ret_v.type()==CV_32FC1);
    CV_Assert(ret_v.rows==(2*r+1) && ret_v.rows==ret_v.cols);
    CV_Assert(std::abs(1.0-cv::sum(ret_v)[0])<1.0e-6);
    return ret_v;
}

cv::Mat
fsiv_fill_expansion(cv::Mat const& in, const int r)
{
    CV_Assert(!in.empty());
    CV_Assert(r>0);
    cv::Mat ret_v;
    //TODO:
    //Use of cv::copyMakeBorder is not allowed.
    //Hint you don't need use any for sentence.

    //crear matriz de tamaño in.size + r en cada lado(en total 2*r) y luego ir poniendo a 0 esos bordes comprobando 
    //si el pixel que estamos pintado esta fuera del borde original con 2*r,2*-r o 2*r y 2*-r y el 
    //resto copiamos de la imagen original

    //tambien se puede hacer con cv::Rect() para seleccionar la zona de interes de la imagen original
    //con sintaxis cv::Rect(Punto_X,Punto_Y,Ancho,Largo)->ancho son cols y largo rows


    ret_v = cv::Mat::zeros(in.rows+2*r, in.cols+2*r,in.type());
    in.copyTo(ret_v(cv::Rect(r,r,in.cols,in.rows)));

    //
    CV_Assert(ret_v.type()==in.type());
    CV_Assert(ret_v.rows == in.rows+2*r);
    CV_Assert(ret_v.cols == in.cols+2*r);
    return ret_v;
}

cv::Mat
fsiv_circular_expansion(cv::Mat const& in, const int r)
{
    CV_Assert(!in.empty());
    CV_Assert(r>0);
    cv::Mat ret_v;
    //TODO
    // Use of cv::copyMakeBorder is not allowed.
    // Hint you don't need use any "for" sentence, only 9 copyTo from "in"
    // rois to "ret_v" rois.
    //circular expansion se refiere a expansion que copia el lado simetrico u opuesto de la imagen en el 
    //punto que necesita expandir, es decir trata la imagen como si fuera un "bola"

    //primero hacemos una expansion normal poniendo la expansion a 0 como la funcion de antes

    ret_v = fsiv_fill_expansion(in,r);

    //ahora copiamos los rectangulos opuestos de la imagen real en la expansion con tamaño de rectangulo r(ventana)

    in(cv::Rect(in.cols - r, in.rows - r, r, r)).copyTo(ret_v(cv::Rect(0, 0, r, r)));
    in(cv::Rect(in.cols - r, 0, r, in.rows)).copyTo(ret_v(cv::Rect(0, r, r, in.rows)));
    in(cv::Rect(in.cols - r, 0, r, r)).copyTo(ret_v(cv::Rect(0, ret_v.rows - r, r, r)));
    in(cv::Rect(0, 0, in.cols, r)).copyTo(ret_v(cv::Rect(r, ret_v.rows - r, in.cols, r)));

    in(cv::Rect(0, 0, r, r)).copyTo(ret_v(cv::Rect(ret_v.cols - r, ret_v.rows - r, r, r)));
    in(cv::Rect(0, 0, r, in.rows)).copyTo(ret_v(cv::Rect(ret_v.cols - r, r, r, in.rows)));
    in(cv::Rect(0, in.rows - r, r, r)).copyTo(ret_v(cv::Rect(ret_v.cols - r, 0, r, r)));
    in(cv::Rect(0, in.rows - r, in.cols, r)).copyTo(ret_v(cv::Rect(r, 0, in.cols, r)));
    
    
    //hacer dibujo para explicar


    //
    CV_Assert(ret_v.type()==in.type());
    CV_Assert(ret_v.rows == in.rows+2*r);
    CV_Assert(ret_v.cols == in.cols+2*r);
    CV_Assert(!(in.type()==CV_8UC1) || ret_v.at<uchar>(0,0)==in.at<uchar>(in.rows-r, in.cols-r));
    CV_Assert(!(in.type()==CV_8UC1) || ret_v.at<uchar>(0,ret_v.cols/2)==in.at<uchar>(in.rows-r, in.cols/2));
    CV_Assert(!(in.type()==CV_8UC1) || ret_v.at<uchar>(0,ret_v.cols-1)==in.at<uchar>(in.rows-r, r-1));
    CV_Assert(!(in.type()==CV_8UC1) || ret_v.at<uchar>(ret_v.rows/2,0)==in.at<uchar>(in.rows/2, in.cols-r));
    CV_Assert(!(in.type()==CV_8UC1) || ret_v.at<uchar>(ret_v.rows/2,ret_v.cols/2)==in.at<uchar>(in.rows/2, in.cols/2));
    CV_Assert(!(in.type()==CV_8UC1) || ret_v.at<uchar>(ret_v.rows-1,0)==in.at<uchar>(r-1, in.cols-r));
    CV_Assert(!(in.type()==CV_8UC1) || ret_v.at<uchar>(ret_v.rows-1,ret_v.cols/2)==in.at<uchar>(r-1, in.cols/2));
    CV_Assert(!(in.type()==CV_8UC1) || ret_v.at<uchar>(ret_v.rows-1,ret_v.cols-1)==in.at<uchar>(r-1, r-1));
    return ret_v;
}

cv::Mat
fsiv_filter2D(cv::Mat const& in, cv::Mat const& filter)
{
    CV_Assert(!in.empty() && !filter.empty());
    CV_Assert(in.type()==CV_32FC1 && filter.type()==CV_32FC1);
    cv::Mat ret_v;

    // TODO
    // Remember: use of cv::filter2D is not allowed.

    //en un bucle for iremos seleccionando la ventana con radio r con cv::Rect(i,j,filter.cols,filter.rows) 
    //y copiandolo a una nueva matriz ventana

    //luego haremos la operacion de correlacion/convolucion multiplicando ventana por filtro y guardando en aux 
    //para hacer luego el sumatorio de este espacio y guardarlo en el punto i,j en el que estamos trabajando

    cv::Mat ventana, aux;
    ret_v = cv::Mat::zeros(in.rows - (filter.rows - 1), in.cols - (filter.cols - 1), CV_32F);

    for (int i = 0; i < in.rows - (filter.rows - 1); i++){
        for (int j = 0; j < in.cols - (filter.cols - 1); j++){

            //seleccionamos la ventana con la que trabajaremos de la imagen original
            ventana = in(cv::Rect(j, i, filter.cols, filter.rows)).clone();

            //multiplicamos ventana por filtro y guardamos la matriz resultante en aux
            cv::multiply(ventana, filter, aux);

            //sumatorio de los valores de aux, su suma debe estar normalizada?
            float sum = cv::sum(aux)[0];
            ret_v.at<float>(i, j) = sum;
        }
    }


    //
    CV_Assert(ret_v.type()==CV_32FC1);
    CV_Assert(ret_v.rows==in.rows-2*(filter.rows/2));
    CV_Assert(ret_v.cols==in.cols-2*(filter.cols/2));
    return ret_v;
}

cv::Mat
fsiv_combine_images(const cv::Mat src1, const cv::Mat src2,
                    double a, double b)
{
    CV_Assert(src1.type()==src2.type());
    CV_Assert(src1.rows==src2.rows);
    CV_Assert(src1.cols==src2.cols);
    cv::Mat ret_v;
    
    //TODO
    // Remember: use vectorized code.

    ret_v = a * src1 + b * src2;

    //
    CV_Assert(ret_v.type()==src2.type());
    CV_Assert(ret_v.rows==src2.rows);
    CV_Assert(ret_v.cols==src2.cols);
    return ret_v;
}

cv::Mat
fsiv_usm_enhance(cv::Mat  const& in, double g, int r,
                 int filter_type, bool circular, cv::Mat *unsharp_mask)
{
    CV_Assert(!in.empty());
    CV_Assert(in.type()==CV_32FC1);
    CV_Assert(r>0);
    CV_Assert(filter_type>=0 && filter_type<=1);
    CV_Assert(g>=0.0);
    cv::Mat ret_v;
    // TODO
    // Remember: use your own functions fsiv_xxxx
    // Remember: when unsharp_mask pointer is nullptr, means don't save the
    // unsharp mask on in.

    cv::Mat expanded, imgLow, filter;

    if (unsharp_mask == nullptr){
        unsharp_mask = &imgLow;
    }

    if(filter_type==0){
        if(circular){
            filter = fsiv_create_box_filter(r);
            expanded = fsiv_circular_expansion(in,r);

            cv::flip(filter, filter, -1);
            //filtrado
            *unsharp_mask = fsiv_filter2D(expanded, filter);

            //aqui se hace realmente el realce
            //Proceso: G = (1+g)I - gIL (Si g>1 se denomina high-boost filtering.)
            ret_v = fsiv_combine_images(in, *unsharp_mask, g+1, -g);
        }else{
            filter = fsiv_create_box_filter(r);
            expanded = fsiv_fill_expansion(in,r);

            cv::flip(filter, filter, -1);
            *unsharp_mask = fsiv_filter2D(expanded, filter);
            ret_v = fsiv_combine_images(in, *unsharp_mask, g+1, -g);
            
        }
    }else if(filter_type==1){
        if(circular){
            filter = fsiv_create_gaussian_filter(r);
            expanded = fsiv_circular_expansion(in,r);

            cv::flip(filter, filter, -1);
            *unsharp_mask = fsiv_filter2D(expanded, filter);
            ret_v = fsiv_combine_images(in, *unsharp_mask, g+1, -g);
        }else{
            filter = fsiv_create_gaussian_filter(r); 
            expanded = fsiv_fill_expansion(in,r);

            cv::flip(filter, filter, -1);
            *unsharp_mask = fsiv_filter2D(expanded, filter);
            ret_v = fsiv_combine_images(in, *unsharp_mask, g+1, -g);
        }
    }else{

    }

    //
    CV_Assert(ret_v.rows==in.rows);
    CV_Assert(ret_v.cols==in.cols);
    CV_Assert(ret_v.type()==CV_32FC1);
    return ret_v;
}
