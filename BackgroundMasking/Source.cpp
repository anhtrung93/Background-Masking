#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <stdio.h>
#include <direct.h>
#include <math.h>
#include <time.h>
#include "graph.h"

#include <iostream>

using namespace cv;
using namespace std;

const String SHARED_PATH = "E:\\Dropbox\\Computer Vision Topic - Background Masking\\Images data\\Rectified image\\";
//const String SHARED_PATH = "E:\\Dropbox\\Computer Vision Topic - Background Masking\\Images data\\JPEGs\\";

const int DEFAULT_WIDTH = 896;
const int NUM_COLS = DEFAULT_WIDTH;
const int DEFAULT_HEIGHT = 504;
const int NUM_ROWS = DEFAULT_HEIGHT;
const int MIN_DISPARITY = -40;
const int MAX_DISPARITY = 80;
const int STEP_DISPARITY = 2;
const int MAX_LABEL = ((MAX_DISPARITY - MIN_DISPARITY) / STEP_DISPARITY) + 1;

//const String IMAGE_NAME = "DSCF7463";//"DSCF7457";//"DSCF1910";//"DSCF1793";//"DSCF0253";
//const String LEFT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-L.jpg";
//const String RIGHT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-R.jpg";
//const String PATH_TO_DISPARITY_MAP_STORAGE = "E:\\Temp\\" + IMAGE_NAME + "\\";

const int CONST_INF = INT_MAX;
const float SMOOTH_EFFICIENT = 2.0;
const float SMOOTH_EFFICIENT2 = 0.3;
const double SPATIAL_RAD = 20.0;
const double COLOR_RAD = 20.0;
const int MAX_PYR_LEVEL = 3;
const float COLOR_THRESHOLD = 5;
const int NUM_OF_CYCLES = 2;

const bool USE_SEGMENT = true;

const bool SHOW_DISPARITY_MAP = true;
const bool STORE_DISPARITY_MAP = true;
const bool SHOW_SEGMENTED_IMAGE = false;
const bool STORE_SEGMENTED_IMAGE = true;
const bool STORE_MATCHING_COST = false;

void graphCut(float *[MAX_LABEL], int[], uchar[][NUM_COLS], const Mat &);
void matchingTwoView(const Mat &, const Mat &, const int, int[], float *[MAX_LABEL]);
void segmentation(const Mat &, Mat &);

void loadImages(const string &, const string &, Mat &, Mat &);
void storeMat(const Mat &, const string &, vector<int> *);


int main(int argc, char** argv) {
	const int NUM_OF_IMAGE = 1;// 16;
	const String images[NUM_OF_IMAGE] = { "59" };//"59", "65", "99", "73", "71", "66", "68", "70", "74", "75", "77", "78", "84", "85", "92", "96"
	for (int idImage = 0; idImage < NUM_OF_IMAGE; ++idImage){
		const String IMAGE_NAME = "DSCF74" + images[idImage]; //"DSCF7457";//"DSCF1910";//"DSCF1793";//"DSCF0253";
		const String LEFT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-L.jpg";
		const String RIGHT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-R.jpg";
		const String PATH_TO_DISPARITY_MAP_STORAGE = "E:\\Temp\\" + IMAGE_NAME + "\\";

		mkdir(PATH_TO_DISPARITY_MAP_STORAGE.c_str());

		float * matchRightCost[MAX_LABEL];
		float * matchLeftCost[MAX_LABEL];

		Mat leftImage, rightImage;
		loadImages(LEFT_IMAGE_NAME, RIGHT_IMAGE_NAME, leftImage, rightImage);
		//Segmentation
		Mat segmentedRightImage, segmentedLeftImage;
		if (USE_SEGMENT) {
			segmentation(rightImage, segmentedRightImage);
			segmentation(leftImage, segmentedLeftImage);

			if (SHOW_SEGMENTED_IMAGE) {
				imshow("Segmented Left Image", segmentedLeftImage);
				imshow("Segmented Right Image", segmentedRightImage);
				waitKey();
			}
			if (STORE_SEGMENTED_IMAGE) {
				storeMat(segmentedLeftImage, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-segmented-L.jpg", NULL);
				storeMat(segmentedRightImage, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-segmented-R.jpg", NULL);
			}
		}
		
		//Convert to GRAY images for matching
		cvtColor(leftImage, leftImage, CV_BGR2GRAY);
		cvtColor(rightImage, rightImage, CV_BGR2GRAY);
		//Matching
		int label2Disparity[MAX_LABEL];
		for (int id = 0, disparity = MIN_DISPARITY; id < MAX_LABEL; ++id, disparity += 2){
			label2Disparity[id] = disparity;
		}
		matchingTwoView(rightImage, leftImage, 1, label2Disparity, matchRightCost);
		matchingTwoView(leftImage, rightImage, -1, label2Disparity, matchLeftCost);
		//Store matching results
		if (STORE_MATCHING_COST) {
			cout << "Storing: " << endl;
			vector<int> compression_params;
			compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
			compression_params.push_back(100);
			for (int idLabel = 0; idLabel < MAX_LABEL; ++idLabel){
				cout << ".";
				Mat temp = Mat(leftImage.rows, leftImage.cols, CV_32FC1, matchLeftCost[idLabel]);
				storeMat(temp, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-L-" + to_string(idLabel) + ".jpg", &compression_params);
				Mat temp2 = Mat(leftImage.rows, leftImage.cols, CV_32FC1, matchRightCost[idLabel]);
				storeMat(temp2, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-R-" + to_string(idLabel) + ".jpg", &compression_params);
			}
			cout << endl;
		}
		//Graph Cut
		uchar disparityRightMap[NUM_ROWS][NUM_COLS];
		uchar disparityLeftMap[NUM_ROWS][NUM_COLS];
		graphCut(matchRightCost, label2Disparity, disparityRightMap, segmentedRightImage);
		graphCut(matchLeftCost, label2Disparity, disparityLeftMap, segmentedLeftImage);
		//Cross checking
		for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol){
				disparityLeftMap[idRow][idCol] = label2Disparity[disparityLeftMap[idRow][idCol]];
				disparityRightMap[idRow][idCol] = label2Disparity[disparityRightMap[idRow][idCol]];
			}
		}
		for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol){
				if (idCol + disparityRightMap[idRow][idCol] >= 0 && idCol + disparityRightMap[idRow][idCol] < NUM_COLS 
					&& disparityRightMap[idRow][idCol] == disparityLeftMap[idRow][idCol + disparityRightMap[idRow][idCol]]) {
					//do nothing
				}
				else {
					disparityRightMap[idRow][idCol] = 255;
				}
				if (idCol - disparityLeftMap[idRow][idCol] >= 0 && idCol - disparityLeftMap[idRow][idCol] < NUM_COLS
					&& disparityLeftMap[idRow][idCol] == disparityRightMap[idRow][idCol - disparityLeftMap[idRow][idCol]]) {
					//do nothing
				}
				else {
					disparityLeftMap[idRow][idCol] = 255;
				}
			}
		}
		//Store & show disparity Map
		if (STORE_DISPARITY_MAP) {
			Mat disparityRightMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityRightMap);
			storeMat(disparityRightMat, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-Rcut.jpg", NULL);
			Mat disparityLeftMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityLeftMap);
			storeMat(disparityLeftMat, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-Lcut.jpg", NULL);
		}
		if (SHOW_DISPARITY_MAP) {
			Mat disparityRightMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityRightMap);
			Mat disparityLeftMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityLeftMap);
			imshow("Disparity right map", disparityRightMat);
			imshow("Disparity left map", disparityLeftMat);
			waitKey();
		}

		for (int idLabel = 0; idLabel < MAX_LABEL; ++idLabel) {
			if (matchLeftCost[idLabel] != NULL) {
				free(matchLeftCost[idLabel]);
				matchLeftCost[idLabel] = NULL;
			}
			if (matchRightCost[idLabel] != NULL) {
				free(matchRightCost[idLabel]);
				matchRightCost[idLabel] = NULL;
			}
		}
	}

	return 0;
}

void loadImages(const string & LEFT_IMAGE_NAME, const string & RIGHT_IMAGE_NAME, Mat & leftImage, Mat & rightImage) {
	Size size(DEFAULT_WIDTH, DEFAULT_HEIGHT);

	Mat leftRGBImage = imread(LEFT_IMAGE_NAME);
	if (leftRGBImage.empty()){
		cerr << "Load left image at " << LEFT_IMAGE_NAME << " failed!" << endl;
		exit(-1);
	}
	resize(leftRGBImage, leftImage, size);

	Mat rightRGBImage = imread(RIGHT_IMAGE_NAME);
	if (rightRGBImage.empty()){
		cerr << "Load right image at " << RIGHT_IMAGE_NAME << " failed!" << endl;
		exit(-1);
	}
	resize(rightRGBImage, rightImage, size);
}

void storeMat(const Mat & image, const string & FILE_NAME, vector<int> * compression_params) {
	if (compression_params == NULL) {
		compression_params = new vector<int>;
		compression_params->push_back(CV_IMWRITE_JPEG_QUALITY);
		compression_params->push_back(100);
	}
	for (int idLabel = 0; idLabel < MAX_LABEL; ++idLabel){
		imwrite(FILE_NAME, image, *compression_params);
	}
}

void graphCut(float * matchCost[MAX_LABEL], int label2Disparity[], uchar disparityMap[][NUM_COLS], const Mat & segmentedMat){
	cout << "Graph cut: " << endl;
	clock_t start = clock();
	for (int idRow = 0; idRow < NUM_ROWS; ++idRow) {
		for (int idCol = 0; idCol < NUM_COLS; ++idCol) {
			disparityMap[idRow][idCol] = 0;
		}
	}

	//init matrix storage of nodes for graph cut;
	Graph::node_id * * node = new Graph::node_id *[NUM_COLS];
	for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
		node[idRow] = new Graph::node_id[NUM_COLS];
	}
	//Graph cut
	for (int idCycle = 0; idCycle < NUM_OF_CYCLES; ++idCycle){
		cout << "\nCycle " << idCycle << ": ";
		for (int idLabel = 0; idLabel < MAX_LABEL; ++idLabel){
			cout << ".";
			Graph * graph = new Graph();
			for (int pp = 0, idRow = 0; idRow < NUM_ROWS; ++idRow){
				for (int idCol = 0; idCol < NUM_COLS; ++idCol, ++pp){
					node[idRow][idCol] = graph->add_node();
					//Init node
					int curLabel = disparityMap[idRow][idCol];
					if (curLabel == idLabel){
						graph->set_tweights(node[idRow][idCol], matchCost[idLabel][pp], CONST_INF);
					}
					else {
						graph->set_tweights(node[idRow][idCol], matchCost[idLabel][pp], matchCost[curLabel][pp]);
					}
					float costCurTemp = SMOOTH_EFFICIENT * abs(label2Disparity[curLabel] - label2Disparity[idLabel]);
					//Init edge to up node						
					if (idRow > 0){
						int upLabel = disparityMap[idRow - 1][idCol];
						float costUpTemp = SMOOTH_EFFICIENT * abs(label2Disparity[upLabel] - label2Disparity[idLabel]);
						float costUpToCurTemp = SMOOTH_EFFICIENT * abs(label2Disparity[upLabel] - label2Disparity[curLabel]);
						//Color segment
						if (USE_SEGMENT && segmentedMat.at<Vec3b>(idRow, idCol) != segmentedMat.at<Vec3b>(idRow - 1, idCol)){
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
						int leftLabel = disparityMap[idRow][idCol - 1];
						float costLeftTemp = SMOOTH_EFFICIENT * abs(label2Disparity[leftLabel] - label2Disparity[idLabel]);
						float costLeftToCurTemp = SMOOTH_EFFICIENT * abs(label2Disparity[leftLabel] - label2Disparity[curLabel]);
						//Color segment
						if (USE_SEGMENT && segmentedMat.at<Vec3b>(idRow, idCol) != segmentedMat.at<Vec3b>(idRow, idCol - 1)){
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

			for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
				for (int idCol = 0; idCol < NUM_COLS; ++idCol){
					if (graph->what_segment(node[idRow][idCol]) != Graph::SOURCE){
						disparityMap[idRow][idCol] = (uchar)idLabel;
					}
				}
			}

			delete graph;
		}
	}
	//release matrix storage for graph cut
	for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
		delete[] node[idRow];
	}
	delete[] node;

	clock_t finish = clock();
	cout << " End after " << ((double)(finish - start) / CLOCKS_PER_SEC) << " sec" << endl;
}

void matchingTwoView(const Mat & referImage, const Mat & searchImage, const int CONST_REF, int disparityList[], float * matchCost[MAX_LABEL]){
	cout << "Matching Two Views: ";
	clock_t start = clock();

	unsigned char * refer = (unsigned char *)(referImage.data);
	unsigned char * search = (unsigned char *)(searchImage.data);

	for (int label = 0; label < MAX_LABEL; ++label){
		int idDisparity = disparityList[label];
		//cout << "idDisparity = " << idDisparity << endl;
		matchCost[label] = (float *)malloc(DEFAULT_WIDTH * DEFAULT_HEIGHT * sizeof(float));
		for (int pp = 0, idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol, ++pp){
				int targetCol = idCol + CONST_REF * idDisparity;
				if (targetCol >= 0 && targetCol < NUM_COLS){
					//block matching
					int blockSize = 0;	float temporary = 0.0f;
					matchCost[label][pp] = 0;
					for (int idRow2 = -1; idRow2 <= 1; ++idRow2){
						for (int idCol2 = -1; idCol2 <= 1; ++idCol2){
							if (idRow + idRow2 >= 0 && idRow + idRow2 < NUM_ROWS && idCol + idCol2 >= 0 && idCol + idCol2 < NUM_COLS){
								if (targetCol + idCol2 >= 0 && targetCol + idCol2 < NUM_COLS){
									++blockSize;
									temporary += (float)abs(refer[(idRow + idRow2) * NUM_COLS + (idCol + idCol2)]
										- search[(idRow + idRow2) * NUM_COLS + (targetCol + idCol2)]);
								}
							}
						}
					}
					matchCost[label][pp] = ((float)temporary) / blockSize;
				}
				else {
					matchCost[label][pp] = 255.0f;
				}
			}
		}
	}
	clock_t finish = clock();
	cout << " End after " << ((double)(finish - start) / CLOCKS_PER_SEC) << " sec" << endl;
}

void segmentation(const Mat & img, Mat & res) {
	cout << "Segmentation: " << endl;
	clock_t start = clock();
	pyrMeanShiftFiltering(img, res, SPATIAL_RAD, COLOR_RAD, MAX_PYR_LEVEL);
	//floodFillPostprocess(res, Scalar::all(COLOR_THRESHOLD));
	RNG rng = theRNG();
	Mat mask(img.rows + 2, img.cols + 2, CV_8UC1, Scalar::all(0));
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			if (mask.at<uchar>(y + 1, x + 1) == 0)	{
				Scalar newVal(rng(256), rng(256), rng(256));
				floodFill(res, mask, Point(x, y), newVal, 0, Scalar::all(COLOR_THRESHOLD), Scalar::all(COLOR_THRESHOLD));
			}
		}
	}
	clock_t finish = clock();
	cout << " End after " << ((double)(finish - start) / CLOCKS_PER_SEC) << " sec" << endl;
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