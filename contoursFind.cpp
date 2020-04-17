// contoursFind.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int rollchecked();//车轮孔检测
int cupcheckde();//红色杯盖检测
int chipCenterchecked();//识别芯片中心部分

int main()
{
    rollchecked();//车轮孔检测
    cupcheckde();//红色杯盖检测
    chipCenterchecked();//识别芯片中心部分
    return 0;
}

int rollchecked()//车轮孔检测
{
    Mat srcImage = imread("D:\\opencv\\myproject\\1picture\\rim.png");
    if (!srcImage.data)
    {
        cout << "Cant' get the picture." << endl;
        return -1;
    }
    Mat srcGray,srcCom;

    cvtColor(srcImage, srcGray, CV_BGR2GRAY);
    Mat keneral = getStructuringElement(MORPH_CROSS, Size(5,5), Point(0, 0));//形态学操作，去除噪声
    morphologyEx(srcGray, srcGray, CV_MOP_CLOSE, keneral);
    threshold(srcGray, srcGray, 100,255, THRESH_BINARY_INV|THRESH_OTSU);
    imshow("srcGray", srcGray);
    waitKey(0);

    Mat states;//定义连通域信息
    Mat centers;//定义连通域中心
    int k = connectedComponentsWithStats(srcGray, srcCom, states, centers, 8, 4);
    vector<Vec3b> colors(50);
    for (int i = 0; i < k; i++)
    {
        colors[i] = Vec3b(0, 0, 0);//将连通域标记为黑色
        double width = states.at<int>(i, CC_STAT_WIDTH);//获取每个连通域的宽，高
        double height = states.at<int>(i, CC_STAT_HEIGHT);
        if (width/height>0.9&& width / height<1.1)
        {
            colors[i]=Vec3b(255,0,0);//将为圆的部分标为蓝色
        }
    }

    for (int y = 0; y < srcImage.rows; y++)           
        for (int x = 0; x < srcImage.cols; x++) 
        { 
            int label = srcCom.at<int>(y, x);                 
            //CV_Assert(0 <= srcCom && srcCom <= k);   
            if(label!=0&&colors[label]!=Vec3b(0,0,0))//选中标记且标记为蓝色
            {
                srcImage.at<cv::Vec3b>(y, x) = colors[label];
            }
        }
    
    imshow("rollchecked", srcImage);
    waitKey(0);
    return 1;
}

int cupcheckde()//红色杯盖检测
{
    Mat srcImage = imread("D:\\opencv\\myproject\\1picture\\topic1.JPG");//获取杯盖图像
    Mat dstImage;
    resize(srcImage, srcImage, Size(srcImage.rows * 0.2, srcImage.cols * 0.2), 0, 0, INTER_AREA);
    srcImage.copyTo(dstImage);
    if (!srcImage.data)
    {
        cout << "Cant' get the picture." << endl;
        return -1;
    }

    Mat srcthre(srcImage.size(), CV_8UC1);//创建一个单通道的图像存储二值化图片
    Mat srcHsv;
    cvtColor(srcImage, srcHsv, CV_BGR2HSV);

    int H_min1 = 0, H_max1 = 8, H_min2 = 160, H_max2 = 180;//红色范围设定
    int S_min = 80;
    int V_min = 50, V_max = 220;

    for (int i = 0; i < srcHsv.rows; i++)//如果图像像素值为红色，设为1；否则设为0
    {
        for (int j = 0; j < srcImage.cols; j++)
        {
            if ((srcHsv.at<Vec3b>(i, j)[0]>H_min1&& srcHsv.at<Vec3b>(i, j)[0]<H_max1|| 
                srcHsv.at<Vec3b>(i, j)[0]>H_min2&& srcHsv.at<Vec3b>(i, j)[0]<H_max2)&&
                srcHsv.at<Vec3b>(i, j)[1]>S_min&& srcHsv.at<Vec3b>(i, j)[2]>V_min&& srcHsv.at<Vec3b>(i, j)[2]<V_max)
            {
                srcImage.at<Vec3b>(i, j)[0] = 255;
                srcImage.at<Vec3b>(i, j)[1] = 255;
                srcImage.at<Vec3b>(i, j)[2] = 255;
            }
            else
            {
                srcImage.at<Vec3b>(i, j)[0] = 0;
                srcImage.at<Vec3b>(i, j)[1] = 0;
                srcImage.at<Vec3b>(i, j)[2] = 0;
            }
        }
    }
    cvtColor(srcImage, srcthre, CV_BGR2GRAY);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5,5), Point(-1, -1));//，设置掩膜，形态学开操作处理
    morphologyEx(srcthre, srcthre, CV_MOP_CLOSE, kernel);
    imshow("dstImage", srcthre);
   
    Mat srcCanny;
    int thresholdValue = 100;
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    RotatedRect rbox;//最小外接矩形
    Canny(srcthre, srcCanny, thresholdValue, thresholdValue * 2, 3, false);//获取图像边缘
    imshow("thecanny", srcCanny);
    findContours(srcCanny, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));//发现轮廓
    for (int i = 0; i < contours.size(); i++)//获取最小矩阵
    {
        rbox = minAreaRect(contours[i]);
        if (rbox.size.width > srcImage.rows / 6 && rbox.size.height / 6)
        {
            Point2f rboxpoint[4];
            rbox.points(rboxpoint);
            for (int i = 0; i < 4; i++)//绘制最小矩阵
            {
                line(dstImage, rboxpoint[i], rboxpoint[(i + 1) % 4], 1, 8, 0);
            }
        }
        
    }
    
    imshow("cuppchecked", dstImage);
    waitKey(0);
    return 1;
}

int chipCenterchecked()//识别芯片中心部分
{
    Mat srcImage = imread("D:\\opencv\\myproject\\1picture\\die_on_chip.png");
    if (!srcImage.data)
    {
        cout << "Cant' get the picture." << endl;
        return -1;
    }
    Mat srcGray;
    cvtColor(srcImage, srcGray, CV_BGR2GRAY);//图像转为灰度图并二值化
    threshold(srcGray, srcGray, 100, 255, THRESH_BINARY | THRESH_OTSU);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));//，设置掩膜，形态学闭操作处理
    morphologyEx(srcGray,srcGray, CV_MOP_OPEN, kernel);
    imshow("the thresholdsrc", srcGray);

    Mat srcCanny;
    int thresholdValue = 100;
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    Canny(srcGray, srcCanny, thresholdValue, thresholdValue * 2, 3, false);//获取图像边缘
    findContours(srcCanny, contours, hierachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));//发现轮廓

    for (int i = 0; i < contours.size(); i++)//绘制轮廓，如果边界太小则不绘制
    {
        Rect r = boundingRect(Mat(contours[i]));
        if (r.area()>400)
        {
            drawContours(srcImage, contours, i, Scalar(255, 0, 0), 2, 8);
        }
    }
    imshow("chipCenterchecked", srcImage);
    waitKey(0);
    return 1;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
