#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <iostream>
#include <stdio.h>
#include <direct.h>
#include <math.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include "graph.h"

using namespace cv;
using namespace std;

const String SHARED_PATH = "E:\\Dropbox\\Computer Vision\\Images data\\JPEGs\\";

//const int IMAGE_ROW = 896;
//const int IMAGE_COL = 504;

const int DEFAULT_WIDTH = 896;
const int DEFAULT_HEIGHT = 504;
const int MIN_DISPARITY = -20;
const int MAX_DISPARITY = 40;
const int STEP_DISPARITY = 2;
const int MAX_LABEL = ((MAX_DISPARITY - MIN_DISPARITY) / STEP_DISPARITY) + 1;

//const String IMAGE_NAME = "DSCF7463";//"DSCF7457";//"DSCF1910";//"DSCF1793";//"DSCF0253";
//const String LEFT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-L.jpg";
//const String RIGHT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-R.jpg";
//const String PATH_TO_DISPARITY_MAP_STORAGE = "E:\\Temp\\" + IMAGE_NAME + "\\";

const int CONST_INF = INT_MAX;
const float SMOOTH_EFFICIENT = 2.0;
const float SMOOTH_EFFICIENT2 = 0.5;
const float COLOR_THRESHOLD = 5;
const int NUM_OF_CYCLES = 2;
const bool STORE_DISPARITY_MAP = true;

void graphCut(Mat *, int[], Mat &, const Mat &);
void matchingTwoView(const Mat &, const Mat &, int[], Mat *);
//void edgeDetection(const Mat &, Mat &);

int main(int argc, char** argv) {
	const int NUM_OF_IMAGE = 1;// 16;
	const String images[NUM_OF_IMAGE] = { "96" };//"63", "65", "99", "73", "71", "66", "68", "70", "74", "75", "77", "78", "84", "85", "92", "96" };
	for (int idImage = 0; idImage < NUM_OF_IMAGE; ++idImage){
		const String IMAGE_NAME = "DSCF74" + images[idImage]; //"DSCF7457";//"DSCF1910";//"DSCF1793";//"DSCF0253";
		const String LEFT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-L.jpg";
		const String RIGHT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-R.jpg";
		const String PATH_TO_DISPARITY_MAP_STORAGE = "E:\\Temp\\" + IMAGE_NAME + "\\";

		mkdir(PATH_TO_DISPARITY_MAP_STORAGE.c_str());

		Size size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
		Mat leftImage, leftRGBImage; resize(imread(LEFT_IMAGE_NAME), leftRGBImage, size);
		cvtColor(leftRGBImage, leftImage, CV_BGR2YCrCb);
		Mat rightImage, rightRGBImage; cvtColor(imread(RIGHT_IMAGE_NAME), rightRGBImage, CV_BGR2YCrCb);
		resize(rightRGBImage, rightImage, size);

		Mat matchingCostMaps[MAX_LABEL];
		int label2Disparity[MAX_LABEL];
		for (int id = 0, disparity = MIN_DISPARITY; id < MAX_LABEL; ++id, disparity += 2){
			label2Disparity[id] = disparity;
		}
		matchingTwoView(leftImage, rightImage, label2Disparity, matchingCostMaps);

		if (STORE_DISPARITY_MAP){
			vector<int> compression_params;
			compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
			compression_params.push_back(100);
			for (int idLabel = 0; idLabel < MAX_LABEL; ++idLabel){
				imwrite(PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-" + to_string(idLabel) + ".jpg", matchingCostMaps[idLabel], compression_params);
			}
		}

		Mat disparityMap = Mat(leftImage.rows, leftImage.cols, CV_8UC1);
		graphCut(matchingCostMaps, label2Disparity, disparityMap, rightRGBImage);

		for (int idRow = 0; idRow < disparityMap.rows; ++idRow){
			for (int idCol = 0; idCol < disparityMap.cols; ++idCol){
				disparityMap.at<uchar>(idRow, idCol) *= (256 / MAX_LABEL);
			}
		}

		if (STORE_DISPARITY_MAP){
			vector<int> compression_params;
			compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
			compression_params.push_back(100);
			imwrite(PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-cut.jpg", disparityMap, compression_params);
		}
	}

	return 0;
}

void graphCut(Mat * matchingCostMaps, int label2Disparity[], Mat & disparityMap, const Mat & rightRGBMat){
	for (int idRow = 0; idRow < disparityMap.rows; ++idRow){
		for (int idCol = 0; idCol < disparityMap.cols; ++idCol){
			for (int label = 0; label < MAX_LABEL; ++label){
				disparityMap.at<uchar>(idRow, idCol) = 0;
			}
		}
	}

	//uchar label[IMAGE_ROW][IMAGE_COL]; convMat2Uchar(disparityMap, label);
	//init matrix storage of nodes for graph cut;
	Graph::node_id * * node = new Graph::node_id *[disparityMap.rows];
	for (int idRow = 0; idRow < disparityMap.rows; ++idRow){
		node[idRow] = new Graph::node_id[disparityMap.cols];
	}
	//Graph cut
	for (int idCycle = 0; idCycle < NUM_OF_CYCLES; ++idCycle){
		cout << "\nCycle " << idCycle << ": ";
		for (int idLabel = 0; idLabel < MAX_LABEL; ++idLabel){
			cout << "\tidLabel = " << idLabel;
			Graph * graph = new Graph();
			for (int idRow = 0; idRow < disparityMap.rows; ++idRow){
				for (int idCol = 0; idCol < disparityMap.cols; ++idCol){
					node[idRow][idCol] = graph->add_node();
					//Init node
					int curLabel = disparityMap.at<uchar>(idRow, idCol);
					if (curLabel == idLabel){
						graph->set_tweights(node[idRow][idCol], matchingCostMaps[idLabel].at<float>(idRow, idCol), CONST_INF);
					}
					else {
						graph->set_tweights(node[idRow][idCol], matchingCostMaps[idLabel].at<float>(idRow, idCol), matchingCostMaps[curLabel].at<float>(idRow, idCol));
					}
					float costCurTemp = SMOOTH_EFFICIENT * abs(label2Disparity[curLabel] - label2Disparity[idLabel]);
					//Init edge to up node						
					if (idRow > 0){
						int upLabel = disparityMap.at<uchar>(idRow - 1, idCol);
						float costUpTemp = SMOOTH_EFFICIENT * abs(label2Disparity[upLabel] - label2Disparity[idLabel]);
						float costUpToCurTemp = SMOOTH_EFFICIENT * abs(label2Disparity[upLabel] - label2Disparity[curLabel]);
						//Color segment
						Vec3b rgbCur = rightRGBMat.at<Vec3b>(idRow, idCol);
						Vec3b rgbUp = rightRGBMat.at<Vec3b>(idRow - 1, idCol);
						if (abs(rgbCur.val[0] - rgbUp.val[0]) < COLOR_THRESHOLD && abs(rgbCur.val[1] - rgbUp.val[1]) < COLOR_THRESHOLD && abs(rgbCur.val[2] - rgbUp.val[2]) < COLOR_THRESHOLD){
							costUpTemp *= SMOOTH_EFFICIENT2;
							costCurTemp *= SMOOTH_EFFICIENT2;
							costUpToCurTemp *= SMOOTH_EFFICIENT2;
						}
						if (upLabel == curLabel){
							graph->add_edge(node[idRow][idCol], node[idRow - 1][idCol], costCurTemp, costCurTemp);
						}
						else {
							Graph::node_id auxiliaryNode = graph->add_node();
							graph->set_tweights(auxiliaryNode, 0, costUpToCurTemp);
							graph->add_edge(auxiliaryNode, node[idRow][idCol], costCurTemp, costCurTemp);
							graph->add_edge(auxiliaryNode, node[idRow - 1][idCol], costUpTemp, costUpTemp);
						}
					}
					//Init edge to left node
					if (idCol > 0){
						int leftLabel = disparityMap.at<uchar>(idRow, idCol - 1);
						float costLeftTemp = SMOOTH_EFFICIENT * abs(label2Disparity[leftLabel] - label2Disparity[idLabel]);
						float costLeftToCurTemp = SMOOTH_EFFICIENT * abs(label2Disparity[leftLabel] - label2Disparity[curLabel]);
						//Color segment
						Vec3b rgbCur = rightRGBMat.at<Vec3b>(idRow, idCol);
						Vec3b rgbLeft = rightRGBMat.at<Vec3b>(idRow, idCol - 1);
						if (abs(rgbCur.val[0] - rgbLeft.val[0]) < COLOR_THRESHOLD && abs(rgbCur.val[1] - rgbLeft.val[1]) < COLOR_THRESHOLD && abs(rgbCur.val[2] - rgbLeft.val[2]) < COLOR_THRESHOLD){
							costLeftTemp *= SMOOTH_EFFICIENT2;
							costCurTemp *= SMOOTH_EFFICIENT2;
							costLeftToCurTemp *= SMOOTH_EFFICIENT2;
						}
						if (leftLabel == curLabel){
							graph->add_edge(node[idRow][idCol], node[idRow][idCol - 1], costCurTemp, costCurTemp);
						}
						else {
							Graph::node_id auxiliaryNode = graph->add_node();
							graph->set_tweights(auxiliaryNode, 0, costLeftToCurTemp);
							graph->add_edge(auxiliaryNode, node[idRow][idCol], costCurTemp, costCurTemp);
							graph->add_edge(auxiliaryNode, node[idRow][idCol - 1], costLeftTemp, costLeftTemp);
						}
					}
				}
			}

			Graph::flowtype flow = graph->maxflow();

			for (int idRow = 0; idRow < disparityMap.rows; ++idRow){
				for (int idCol = 0; idCol < disparityMap.cols; ++idCol){
					if (graph->what_segment(node[idRow][idCol]) != Graph::SOURCE){
						disparityMap.at<uchar>(idRow, idCol) = (uchar)idLabel;
					}
				}
			}

			delete graph;
		}
	}
	//release matrix storage for graph cut
	for (int idRow = 0; idRow < disparityMap.rows; ++idRow){
		delete[] node[idRow];
	}
	delete[] node;
}

void matchingTwoView(const Mat & leftImage, const Mat & rightImage, int disparityList[], Mat * matchingCostMaps){
	for (int label = 0; label < MAX_LABEL; ++label){
		int idDisparity = disparityList[label];
		cout << "idDisparity = " << idDisparity << endl;
		matchingCostMaps[label] = Mat(leftImage.rows, leftImage.cols, CV_32FC1);
		for (int idCol = 0; idCol < leftImage.cols; ++idCol){
			int targetCol = idCol - idDisparity;
			for (int idRow = 0; idRow < leftImage.rows; ++idRow){
				if (targetCol >= 0 && targetCol < leftImage.cols){
					//block matching
					int blockSize = 0;
					matchingCostMaps[label].at<float>(idRow, idCol) = 0;
					for (int idRow2 = -1; idRow2 <= 1; ++idRow2){
						for (int idCol2 = -1; idCol2 <= 1; ++idCol2){
							if (idRow + idRow2 >= 0 && idRow + idRow2 < leftImage.rows && idCol + idCol2 >= 0 && idCol + idCol2 < leftImage.cols){
								if (targetCol + idCol2 >= 0 && targetCol + idCol2 < leftImage.cols){
									++blockSize;
									matchingCostMaps[label].at<float>(idRow, idCol) += fabs((float)rightImage.at<Vec3b>(idRow + idRow2, idCol + idCol2).val[0] - (float)leftImage.at<Vec3b>(idRow + idRow2, targetCol + idCol2).val[0]);
								}
							}
						}
					}
					matchingCostMaps[label].at<float>(idRow, idCol) /= blockSize;
				}
				else {
					matchingCostMaps[label].at<float>(idRow, idCol) = 255;
				}
			}
		}
	}
}

//void convMat2Uchar(const Mat & mat, uchar byte[][IMAGE_COL]){
//	for (int idRow = 0; idRow < mat.rows; ++idRow){
//		for (int idCol = 0; idCol < mat.cols; ++idCol){
//			byte[idRow][idCol] = mat.at<uchar>(idRow, idCol);
//		}
//	}
//}
//
//void convUchar2Mat(uchar byte[][IMAGE_COL], Mat & mat){
//	for (int idRow = 0; idRow < mat.rows; ++idRow){
//		for (int idCol = 0; idCol < mat.cols; ++idCol){
//			mat.at<uchar>(idRow, idCol) = byte[idRow][idCol];
//		}
//	}
//}