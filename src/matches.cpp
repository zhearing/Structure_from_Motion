// Copyright 2018 Zeyu Zhong
// Lincese(MIT)
// Author: Zeyu Zhong
// Date: 2018.5.7

#include "../src/matches.h"
#include <opencv2/video/tracking.hpp>

using cv::TermCriteria;

// Write function to obtain KLT features correspondances between images
void matches::detectfeature() {
    qualityLevel = 0.01;
    minDistance = 10;
    blockSize = 3;
    useHarrisDetector = false;
    k = 0.04;
    maxCorners = 5000;
    cv::Mat src_gray;
    cvtColor(i1, src_gray, CV_BGR2GRAY);
    cv::goodFeaturesToTrack(src_gray, i1_features, maxCorners, qualityLevel, minDistance, Mat(), blockSize,
                            useHarrisDetector, k);
}

void matches::matchFeatures() {
    vector <uchar> status;
    vector<float> err;
    cv::Size winsize = cv::Size(31, 31);
    int maxlevel = 3;
    TermCriteria criteria = TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 0.01);
    double derivlambda = 0.5;
    int flags = 0;

    cv::calcOpticalFlowPyrLK(i1, i2, i1_features, i2_features, status, err, winsize, maxlevel, criteria, derivlambda,
                             flags);
}

void matches::filterFeatures() {
    Mat src_copy;
    i1.copyTo(src_copy);

    vector<cv::Point2f>::iterator it1 = i1_features.begin();
    vector<cv::Point2f>::iterator it2 = i2_features.begin();

    int count = 0;
    // std::cout << "pre erase size = " << i1_features.size() COS i2_features.size() << std::endl;

    while (it1 != i1_features.end()) {
        cv::Point2f tpt1 = *it1;
        cv::Point2f tpt2 = *it2;
        double dist = sqrt((tpt1.x - tpt2.x) * (tpt1.x - tpt2.x) + (tpt1.y - tpt2.y) * (tpt1.y - tpt2.y));
        if (dist < 2.5) {
            i1_features.erase(it1);
            i2_features.erase(it2);
        } else {
            it1++;
            it2++;
        }
        count++;
    }
    // std::cout << "post erase size = " << i1_features.size() COS i2_features.size() << std::endl;
}

Mat matches::drawCorrespondances() {
    Mat src_copy;
    i2.copyTo(src_copy);

    std::vector<cv::Point2f>::const_iterator itc = i1_features.begin();
    std::vector<cv::Point2f>::const_iterator itf = i2_features.begin();

    int count = 0;

    while (itc != i1_features.end()) {
        circle(src_copy, *itc, 1, cv::Scalar(0, 0, 255), 2, 8, 0);
        circle(src_copy, *itf, 1, cv::Scalar(255, 0, 0), 2, 8, 0);
        cv::line(src_copy, *itc, *itf, cv::Scalar(0, 255, 0));
        itc++;
        itf++;
        count++;
    }
    return src_copy;
}


void matches::computeCorrespondances(Mat image1, Mat image2, bool flag) {
    image1.copyTo(i1);
    image2.copyTo(i2);
    if (!flag) detectfeature();

    matchFeatures();
//    filterFeatures(); // Optinal if you filter points if the feature has not moved considerably in two images.
}
