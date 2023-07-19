#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std; 
Mat imgOrg, imgDil, imgGray, imgCanny, imgBlur, preImg;
string path = "Resources/doc1.jpg";
vector<Point> initialPoints;
Mat preProcess(Mat img)
{
  cvtColor(img, imgGray, COLOR_BGR2GRAY);
  GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
  Canny(imgBlur, imgCanny, 25, 75);
  Mat kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
  dilate(imgCanny, imgDil, kernel);
  return imgDil;
}

vector<Point> getContours(Mat preImg)
{
  vector<Point> biggestPoints;
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  findContours(preImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  vector<vector<Point>> cornPoly(contours.size());
  vector<Rect> boundRect(contours.size());
  string objectType;
  int maxArea = 0;
  for(int i = 0; i < contours.size(); ++i)
  {
    int area = contourArea(contours[i]);
    if(area >= 1000)
    {
      float peri = arcLength(contours[i], true);
      approxPolyDP(contours[i], cornPoly[i], 0.02*peri, true);
        if( area > maxArea && cornPoly[i].size() == 4)
        {
          biggestPoints = {cornPoly[i][0], cornPoly[i][1], cornPoly[i][2], cornPoly[i][3]};
          maxArea = area;
        }
      drawContours(preImg, cornPoly, i, Scalar(255, 0, 255), 2);
    }
  }
  return biggestPoints;
}

void drawPoints(vector<Point> points, Scalar color)
{
  for(int i = 0; i < points.size(); ++i)
  {
    circle(imgOrg, points[i], 10, color, FILLED);
    putText(imgOrg, to_string(i + 1), points[i], FONT_HERSHEY_PLAIN, 5, color, 2);
  }
}

int main()
{
imgOrg = imread(path);
//Preprocess
preImg = preProcess(imgOrg);
initialPoints = getContours(preImg);
drawPoints(initialPoints, Scalar(0, 0, 255));
imshow("points", imgOrg);
waitKey(0);
return 0;
}
