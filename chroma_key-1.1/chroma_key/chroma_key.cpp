//! \file chroma_key.cpp
//! Replace background based on color
//! University of Cordoba
//! (c) MJMJ/2020 FJMC/2022-

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "common_code.hpp"

const char * keys =
    "{help usage ? |      | print this message   }"
    "{i interactive  |      | Activate interactive mode.}"
    "{h hue          |  60  | Chroma key (hue). Def. 60}"
    "{s sensitivity  |  20 | sensitivity. Def. 20}"
    "{v video        |     | the input is a videofile.}"
    "{c camera       |     | the input is a capture device index.}"
    "{@input         |<none>| input source (pathname or camera idx).}"
    "{@background    |<none>| pathname of background image.}"
    ;


typedef struct
{
    cv::Mat foreground;
    cv::Mat background;
    cv::Mat out;
    int hue;
    int sensitivity;
} AppState;



void
on_change_hue(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->hue = v; 
    
    app_state->out = fsiv_apply_chroma_key(app_state->foreground,app_state->background,app_state->hue,app_state->sensitivity);

    cv::imshow("MERGE", app_state->out);
}

void
on_change_sensitivity(int v, void* app_state_)
{
    AppState* app_state = static_cast<AppState*>(app_state_);
    app_state->sensitivity = v; 
    
    app_state->out = fsiv_apply_chroma_key(app_state->foreground,app_state->background,app_state->hue,app_state->sensitivity);

    cv::imshow("MERGE", app_state->out);
}


/*
void on_trackbar(int, void*)
{
	if (alpha_slider % 2 == 0)
		alpha_slider = alpha_slider + 1;

	printf("%d\n", alpha_slider);
}*/



int
main(int argc, char * argv[])
{
    int retCode = EXIT_SUCCESS;
    try {
        /// Init the parser
        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Replace background based on color.");
        if (parser.has("help"))
        {
            parser.printMessage();
            return 0;
        }

        // Get the values
        /*
        cv::String foreg_n = parser.get<cv::String>(0);
        cv::String backg_n = parser.get<cv::String>(1);
        int hue = parser.get<int>(2);
        int sensitivity = parser.get<int>(3);
        */

        cv::String foreg_n = parser.get<cv::String>(0);
        cv::String backg_n = parser.get<cv::String>(1);
        int hue, sensitivity;
        
        if (!parser.check())
        {
            parser.printErrors();
            return EXIT_FAILURE;
        }

        //DO the work!!.

        /*cv::Mat foreground = cv::imread(foreg_n, cv::IMREAD_UNCHANGED);
        cv::Mat background = cv::imread(backg_n, cv::IMREAD_UNCHANGED);
        cv::Mat out;

        AppState app_state;
        app_state.foreground = foreground;
        app_state.background = background;
        app_state.hue = hue;
        app_state.sensitivity = sensitivity;

        cv::namedWindow("FOREGROUND", cv::WINDOW_GUI_EXPANDED);
        cv::namedWindow("BACKGROUND",  cv::WINDOW_GUI_EXPANDED);
        cv::namedWindow("MERGE",  cv::WINDOW_GUI_EXPANDED);*/

        if(parser.has("c")){        

            cv::VideoCapture cap(0);

            if(!cap.isOpened())
                return -1;

            cv::namedWindow("webcam");

            for(;;)
            {
                cv::Mat frame;

                cap >> frame; 

                cv::Mat background = cv::imread(backg_n, cv::IMREAD_UNCHANGED);
                cv::Mat out;

                AppState app_state;
                app_state.foreground = frame;
                app_state.background = background;
                app_state.hue = hue;
                app_state.sensitivity = sensitivity;

                cv::namedWindow("BACKGROUND",  cv::WINDOW_GUI_EXPANDED);
                cv::namedWindow("MERGE",  cv::WINDOW_GUI_EXPANDED);

                if(parser.has("i")){
                //app_state.out = 
                    fsiv_apply_chroma_key(app_state.foreground,app_state.background,app_state.hue,app_state.sensitivity);

                    cv::createTrackbar("H [0, 360]", "MERGE", NULL, 100, on_change_hue, &app_state);
                    cv::setTrackbarPos("H [0, 360]", "MERGE", app_state.hue * 100);
                    cv::createTrackbar("S [0, 100]", "MERGE", NULL, 100, on_change_sensitivity, &app_state);
                    cv::setTrackbarPos("S [0, 100]", "MERGE", app_state.hue * 100);

                }else{

                    if(parser.has("h"))
                        hue = parser.get<int>("h");
                    if(parser.has("s"))
                        sensitivity = parser.get<int>("s");


                    app_state.out = fsiv_apply_chroma_key(frame,background,hue,sensitivity);
             
                }

                cv::imshow("webcam", frame);
                cv::imshow("BACKGROUND", app_state.background);
                cv::imshow("MERGE", app_state.out);
        
            int key = cv::waitKey(0) & 0xff;


                if(cv::waitKey(50) >= 0) break;
            }

        }else if(parser.has("v")){

            cv::Mat foreground = cv::imread(foreg_n, cv::IMREAD_UNCHANGED);//cv::VideoCapture foreground(foreg_n);;
            cv::Mat background = cv::imread(backg_n, cv::IMREAD_UNCHANGED);
            cv::Mat out;

            AppState app_state;
            app_state.foreground = foreground;
            app_state.background = background;
            app_state.hue = hue;
            app_state.sensitivity = sensitivity;

            cv::namedWindow("FOREGROUND", cv::WINDOW_GUI_EXPANDED);
            cv::namedWindow("BACKGROUND",  cv::WINDOW_GUI_EXPANDED);
            cv::namedWindow("MERGE",  cv::WINDOW_GUI_EXPANDED);

            if(parser.has("i")){
            //app_state.out = 
            fsiv_apply_chroma_key(app_state.foreground,app_state.background,app_state.hue,app_state.sensitivity);

            cv::createTrackbar("H [0, 360]", "MERGE", NULL, 100, on_change_hue, &app_state);
            cv::setTrackbarPos("H [0, 360]", "MERGE", app_state.hue * 100);
            cv::createTrackbar("S [0, 100]", "MERGE", NULL, 100, on_change_sensitivity, &app_state);
            cv::setTrackbarPos("S [0, 100]", "MERGE", app_state.hue * 100);

            }else{

                if(parser.has("h"))
                    hue = parser.get<int>("h");
                if(parser.has("s"))
                    sensitivity = parser.get<int>("s");


                app_state.out = fsiv_apply_chroma_key(foreground,background,hue,sensitivity);
             
            }

            cv::imshow("FOREGROUND", app_state.foreground);
            cv::imshow("BACKGROUND", app_state.background);
            cv::imshow("MERGE", app_state.out);
        
            int key = cv::waitKey(0) & 0xff;

        }else{

            cv::Mat foreground = cv::imread(foreg_n, cv::IMREAD_UNCHANGED);
            cv::Mat background = cv::imread(backg_n, cv::IMREAD_UNCHANGED);
            cv::Mat out;

            AppState app_state;
            app_state.foreground = foreground;
            app_state.background = background;
            app_state.hue = hue;
            app_state.sensitivity = sensitivity;

            cv::namedWindow("FOREGROUND", cv::WINDOW_GUI_EXPANDED);
            cv::namedWindow("BACKGROUND",  cv::WINDOW_GUI_EXPANDED);
            cv::namedWindow("MERGE",  cv::WINDOW_GUI_EXPANDED);
            
            if(parser.has("i")){
                //app_state.out = 
                fsiv_apply_chroma_key(app_state.foreground,app_state.background,app_state.hue,app_state.sensitivity);

                cv::createTrackbar("H [0, 360]", "MERGE", NULL, 100, on_change_hue, &app_state);
                cv::setTrackbarPos("H [0, 360]", "MERGE", app_state.hue * 100);
                cv::createTrackbar("S [0, 100]", "MERGE", NULL, 100, on_change_sensitivity, &app_state);
                cv::setTrackbarPos("S [0, 100]", "MERGE", app_state.sensitivity * 100);

             }else{

                    if(parser.has("h"))
                        hue = parser.get<int>("h");
                    if(parser.has("s"))
                        sensitivity = parser.get<int>("s");


                    app_state.out = fsiv_apply_chroma_key(foreground,background,hue,sensitivity);
             
            }

            cv::imshow("FOREGROUND", app_state.foreground);
            cv::imshow("BACKGROUND", app_state.background);
            cv::imshow("MERGE", app_state.out);
        
            int key = cv::waitKey(0) & 0xff;

        }

        

    }
    catch (std::exception& e)
    {
        std::cerr << "Excepcion: " << e.what() << std::endl;
        retCode = EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Unknown excepcion." << std::endl;
        retCode = EXIT_FAILURE;
    }

    return retCode;
}
