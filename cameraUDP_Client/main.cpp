// branch
//feacturel;;;
//  main.cpp
//  cameraUDP_Client
//
//  Created by Katsuya Fujii on 11/27/13.
//  Copyright (c) 2013 Katsuya Fujii. All rights reserved.
//

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


int main()
{
    int sock;
    struct sockaddr_in addr;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    double w = 320, h = 240;
    int c;
    
    CvCapture *capture=0;
    capture = cvCreateCameraCapture(0);
    IplImage *frame = 0 ;
    cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, w);
    cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, h);
    
    cvNamedWindow ("Capture", CV_WINDOW_AUTOSIZE);
    
    
//(1) jpeg compression
    
    frame = cvQueryFrame (capture);
    cv::Mat Mimg;
    Mimg=frame;
    vector<uchar> ibuff;//buffer for coding
    vector<int> param = vector<int>(2);
    param[0]=CV_IMWRITE_JPEG_QUALITY;
    param[1]=95;//default(95) 0-100

    imencode(".jpg",Mimg,ibuff,param);
    cout<<"coded file size(jpg)"<<ibuff.size()<<endl;//fit buff size automatically.
    
    
    static const int sendSize = 65000;    //１度に送信する画像のバッファサイズ:60KB(61440byte)
    static char buff[sendSize];            //送信するバッファ
   
    while (1) {
        frame = cvQueryFrame (capture);
        Mimg=frame;
        if(ibuff.size()<sendSize){
        imencode(".jpg",Mimg,ibuff,param);
        for(int i=0;i<ibuff.size();i++){
            buff[i]=ibuff[i];
        }
        }
        
        int send_len=sendto(sock, buff, sendSize,0, (struct sockaddr *)&addr, sizeof(addr));
        if(send_len==-1)perror("socket");
        else printf("%d \n",send_len);
        
        imshow("Capture", Mimg);
        c = cvWaitKey (1);
        if (c == '\x1b')
            break;
    }
       
    
    cvDestroyWindow ("Capture");
  
   
    
    close(sock);
    
    return 0;
}
