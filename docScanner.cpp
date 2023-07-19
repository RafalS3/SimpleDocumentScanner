#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std; 
Mat imgOrg, imgDil, imgGray, imgCanny, imgBlur, preImg, imgWarp;
float w = 620, h = 627;
string path = "Resources/doc1.jpg";
vector<Point> initialPoints, docPoints;
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
    if(area >= 17000)
    {
      float peri = arcLength(contours[i], true);
      approxPolyDP(contours[i], cornPoly[i], 0.02*peri, true);
        if( area > maxArea && cornPoly[i].size() == 4)
        {
          biggestPoints = {cornPoly[i][0], cornPoly[i][1], cornPoly[i][2], cornPoly[i][3]};
          maxArea = area;
        }
      drawContours(imgOrg, cornPoly, i, Scalar(255, 0, 255), 2);
    }
  }
  return biggestPoints;
}

void drawPoints(vector<Point> points, Scalar color)
{
  for(int i = 0; i < points.size(); ++i)
  {
    circle(imgOrg, points[i], 10, color, FILLED);
    putText(imgOrg, to_string(i), points[i], FONT_HERSHEY_PLAIN, 5, color, 2);
  }
}

vector<Point> reorder(vector<Point> points)
{
  vector<Point> newPoints;
  vector<int> sumPoints, subPoints;
  for(int i = 0; i < 4; ++i)
  {
    sumPoints.push_back(points[i].x + points[i].y);
    subPoints.push_back(points[i].x - points[i].y);
  }
  newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
  newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
  newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
  newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
 return newPoints;

}

Mat getWarp(Mat img, vector<Point> docPoints, float w, float h)
{
  Point2f src[4] = { docPoints[0], docPoints[1], docPoints[2], docPoints[3] };
  Point2f dst[4] = { {0.0f, 0.0f}, {w, 0.0f}, {0.0f, h}, {w, h} };
  
  Mat matrix = getPerspectiveTransform(src, dst);
  warpPerspective(img, imgWarp, matrix, Point(w, h));
  return imgWarp;
}
int main()
{
imgOrg = imread(path);
//Preprocess
preImg = preProcess(imgOrg);
initialPoints = getContours(preImg);
docPoints = reorder(initialPoints);
//drawPoints(docPoints, Scalar(0, 0, 255));
imgWarp = getWarp(imgOrg, docPoints, w, h);

imshow("points", imgWarp);
waitKey(0);
return 0;
}
