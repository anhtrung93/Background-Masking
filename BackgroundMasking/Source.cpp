#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <stdio.h>
#include <direct.h>
#include <vector>
#include <algorithm>
#include <math.h>
#include <time.h>
#include "graph.h"
#include "DisjointSetInt.h"

#include <iostream>

#define MIN(a, b) (((a) < (b)?(a):(b)))
#define MAX(a, b) (((a) > (b)?(a):(b)))

using namespace cv;
using namespace std;

//const String SHARED_PATH = "E:\\Dropbox\\Computer Vision Topic - Background Masking\\Images data\\Rectified image\\";
const String SHARED_PATH = "E:\\Dropbox\\Computer Vision Topic - Background Masking\\Images data\\JPEGs\\";

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
const float SMOOTH_EFFICIENT = 2.0f;
const float SMOOTH_EFFICIENT2 = 0.3f;

const double SPATIAL_RAD = 10.0;
const double COLOR_RAD = 10.0;
const int MAX_PYR_LEVEL = 3;
const float LOWER_COLOR_THRESHOLD = 3.0f;
const float UPPER_COLOR_THRESHOLD = 200.0f;
const int MIN_SEGMENT_AREA = 30;
const int NUM_OF_CYCLES = 2;
const int K_SIZE = 3;

const bool USE_SEGMENT = true;

const bool SHOW_DISPARITY_MAP = false;
const bool STORE_DISPARITY_MAP = true;
const bool SHOW_SEGMENTED_IMAGE = false;
const bool STORE_SEGMENTED_IMAGE = true;
const bool STORE_MATCHING_COST = false;
//92//59
const int LOW_THRES = 0;//40
const int HIGH_THRES = 40;//100

void segmentation(const Mat &, Mat &, int * *, int &);
void matchingTwoView(const Mat &, const Mat &, const int, int[], float *[MAX_LABEL]);
void graphCut(float *[MAX_LABEL], int[], uchar[][NUM_COLS], int * *);
void planeFitting(uchar [][NUM_COLS], int * * segment, int, uchar * * );
void crossChecking(uchar[][NUM_COLS], uchar[][NUM_COLS], uchar * *, uchar * *);

void loadImages(const string &, const string &, Mat &, Mat &);
void storeMat(const Mat &, const string &, vector<int> *);

int main(int argc, char** argv) {
	const int NUM_OF_IMAGE = 1;// 16;
	const String images[NUM_OF_IMAGE] = { "92" };//"65", "99", "73", "71", "66", "68", "70", "74", "75", "77", "78", "85", "96", "92", "59"};
	for (int idImage = 0; idImage < NUM_OF_IMAGE; ++idImage){
		const String IMAGE_NAME = "DSCF74" + images[idImage]; //"DSCF7457";//"DSCF1910";//"DSCF1793";//"DSCF0253";
		const String LEFT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-L.jpg";
		const String RIGHT_IMAGE_NAME = SHARED_PATH + IMAGE_NAME + "-R.jpg";

		const String BACKGROUND_NAME = "DSCF7482";
		const String LEFT_BACKGROUND_NAME = SHARED_PATH + BACKGROUND_NAME + "-L.JPG";
		const String RIGHT_BACKGROUND_NAME = SHARED_PATH + BACKGROUND_NAME + "-R.JPG";

		const String PATH_TO_DISPARITY_MAP_STORAGE = "E:\\Temp\\" + IMAGE_NAME + "\\";

		mkdir(PATH_TO_DISPARITY_MAP_STORAGE.c_str());

		//additional storage, init
		float * matchRightCost[MAX_LABEL], * matchLeftCost[MAX_LABEL];
		for (int idLabel = 0; idLabel < MAX_LABEL; ++idLabel){
			matchRightCost[idLabel] = matchLeftCost[idLabel] = NULL;
		}
		int * * segmentLeft = NULL, *  * segmentRight = NULL;
		int size_segment_left, size_segment_right;

		Mat leftImage, rightImage;
		loadImages(LEFT_IMAGE_NAME, RIGHT_IMAGE_NAME, leftImage, rightImage);
		//Segmentation (required RGB images)
		if (USE_SEGMENT) {
			//Init for segmentation
			segmentLeft = new int * [NUM_ROWS];
			segmentRight = new int * [NUM_ROWS];
			for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
				segmentLeft[idRow] = new int[NUM_COLS];
				segmentRight[idRow] = new int[NUM_COLS];
			}

			//Do the segment
			Mat segmentedRightImage, segmentedLeftImage;
			segmentation(rightImage, segmentedRightImage, segmentRight, size_segment_right);
			segmentation(leftImage, segmentedLeftImage, segmentLeft, size_segment_left);

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
		graphCut(matchRightCost, label2Disparity, disparityRightMap, segmentRight);
		graphCut(matchLeftCost, label2Disparity, disparityLeftMap, segmentLeft);

		for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol){
				disparityLeftMap[idRow][idCol] = label2Disparity[disparityLeftMap[idRow][idCol]];
				disparityRightMap[idRow][idCol] = label2Disparity[disparityRightMap[idRow][idCol]];
			}
		}

		//Cross checking
		uchar * * crossCheckRight = new uchar *[NUM_ROWS];
		uchar * * crossCheckLeft = new uchar *[NUM_ROWS];
		for (int id = 0; id < NUM_ROWS; ++id){
			crossCheckLeft[id] = new uchar[NUM_COLS];
			crossCheckRight[id] = new uchar[NUM_COLS];
		}
		//crossChecking(disparityLeftMap, disparityRightMap, crossCheckLeft, crossCheckRight);
		//Store & show disparity Map
		if (STORE_DISPARITY_MAP) {
			Mat disparityRightMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityRightMap);
			storeMat(disparityRightMat, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-cutR.jpg", NULL);
			Mat disparityLeftMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityLeftMap);
			storeMat(disparityLeftMat, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-cutL.jpg", NULL);
		}

		//PlaneFitting
		planeFitting(disparityLeftMap, segmentLeft, size_segment_left, crossCheckLeft);
		planeFitting(disparityRightMap, segmentRight, size_segment_right, crossCheckRight);

		//Store & show disparity Map
		if (STORE_DISPARITY_MAP) {
			Mat disparityRightMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityRightMap);
			storeMat(disparityRightMat, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-fitR.jpg", NULL);
			Mat disparityLeftMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityLeftMap);
			storeMat(disparityLeftMat, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-fitL.jpg", NULL);
		}
		if (SHOW_DISPARITY_MAP) {
			Mat disparityRightMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityRightMap);
			Mat disparityLeftMat = Mat(leftImage.rows, leftImage.cols, CV_8UC1, disparityLeftMap);
			imshow("Disparity right map", disparityRightMat);
			imshow("Disparity left map", disparityLeftMat);
			waitKey();
		}

		//Background Masking
		Mat imageRightOut, imageLeftOut;
		Mat leftBackground, rightBackground;
		loadImages(LEFT_BACKGROUND_NAME, RIGHT_BACKGROUND_NAME, leftBackground, rightBackground);
		loadImages(LEFT_IMAGE_NAME, RIGHT_IMAGE_NAME, imageLeftOut, imageRightOut);

		DisjointSetInt * sets1 = new DisjointSetInt(NUM_COLS * NUM_ROWS);
		DisjointSetInt * sets2 = new DisjointSetInt(NUM_COLS * NUM_ROWS);

		for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol){
				int idCurJointSet = idRow * NUM_COLS + idCol;
				if (disparityLeftMap[idRow][idCol] > LOW_THRES && disparityLeftMap[idRow][idCol] < HIGH_THRES){
					if (idRow > 0 && (disparityLeftMap[idRow - 1][idCol] > LOW_THRES && disparityLeftMap[idRow - 1][idCol] < HIGH_THRES)){
						sets1->mergeSets(idCurJointSet, (idRow - 1) * NUM_COLS + idCol);
					}
					if (idCol > 0 && (disparityLeftMap[idRow][idCol - 1] > LOW_THRES && disparityLeftMap[idRow][idCol - 1] < HIGH_THRES)){
						sets1->mergeSets(idCurJointSet, idRow* NUM_COLS + idCol - 1);
					}
				}
				if (disparityRightMap[idRow][idCol] > LOW_THRES && disparityRightMap[idRow][idCol] < HIGH_THRES){
					if (idRow > 0 && (disparityRightMap[idRow - 1][idCol] > LOW_THRES && disparityRightMap[idRow - 1][idCol] < HIGH_THRES)){
						sets2->mergeSets(idCurJointSet, (idRow - 1) * NUM_COLS + idCol);
					}
					if (idCol > 0 && (disparityRightMap[idRow][idCol - 1] > LOW_THRES && disparityRightMap[idRow][idCol - 1] < HIGH_THRES)){
						sets2->mergeSets(idCurJointSet, idRow* NUM_COLS + idCol - 1);
					}
				}
			}
		}

		int max1 = 0; int max2 = 0;
		for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol){
				max1 = MAX(max1, sets1->getSizeOfSet(idRow * NUM_COLS + idCol));
				max2 = MAX(max2, sets2->getSizeOfSet(idRow * NUM_COLS + idCol));
			}
		}

		for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol){
				int idCurJointSet = idRow * NUM_COLS + idCol;
				if (disparityLeftMap[idRow][idCol] < LOW_THRES || disparityLeftMap[idRow][idCol] > HIGH_THRES || sets1->getSizeOfSet(idCurJointSet) != max1){
					imageLeftOut.at<Vec3b>(idRow, idCol) = leftBackground.at<Vec3b>(idRow, idCol);
				}
				if (disparityRightMap[idRow][idCol] < LOW_THRES || disparityRightMap[idRow][idCol] > HIGH_THRES || sets2->getSizeOfSet(idCurJointSet) != max2){
					imageRightOut.at<Vec3b>(idRow, idCol) = rightBackground.at<Vec3b>(idRow, idCol);
				}
			}
		}

		delete sets1;
		delete sets2;

		storeMat(imageLeftOut, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-maskL.jpg", NULL);
		storeMat(imageRightOut, PATH_TO_DISPARITY_MAP_STORAGE + IMAGE_NAME + "-maskR.jpg", NULL);

		//Release memory
		for (int idLabel = 0; idLabel < MAX_LABEL; ++idLabel) {
			if (matchLeftCost[idLabel] != NULL) {
				delete[] matchLeftCost[idLabel];
				matchLeftCost[idLabel] = NULL;
			}
			if (matchRightCost[idLabel] != NULL) {
				delete[] matchRightCost[idLabel];
				matchRightCost[idLabel] = NULL;
			}
		}
		if (segmentLeft != NULL){
			for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
				if (segmentLeft[idRow] != NULL){
					delete[] segmentLeft[idRow];
					segmentLeft[idRow] = NULL;
				}
			}
			delete[] segmentLeft;
			segmentLeft = NULL;
		}
		if (segmentRight != NULL){
			for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
				if (segmentRight[idRow] != NULL){
					delete[] segmentRight[idRow];
					segmentRight[idRow] = NULL;
				}
			}
			delete[] segmentRight;
			segmentRight = NULL;
		}
		if (crossCheckLeft != NULL){
			for (int id = 0; id < NUM_ROWS; ++id){
				if (crossCheckLeft[id] != NULL){
					delete[] crossCheckLeft[id];
					crossCheckLeft[id] = NULL;
				}
			}
			delete[] crossCheckLeft;
			crossCheckLeft = NULL;
		}
		if (crossCheckRight != NULL){
			for (int id = 0; id < NUM_ROWS; ++id){
				if (crossCheckRight[id] != NULL){
					delete[] crossCheckRight[id];
					crossCheckRight[id] = NULL;
				}
			}
			delete[] crossCheckRight;
			crossCheckRight = NULL;
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

void planeFitting(uchar disparityMap[][NUM_COLS], int * * segment, int size_segment, uchar * * crossCheck){
	cout << "PlaneFitting: " <<  endl;
	clock_t start = clock();

	int * numInSegment = new int[size_segment];
	unsigned long long * fitValue = new unsigned long long [size_segment];
	for (int idSegment = 0; idSegment < size_segment; ++idSegment){
		numInSegment[idSegment] = 0;
		fitValue[idSegment] = 255;
	}

	for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
		for (int idCol = 0; idCol < NUM_COLS; ++idCol){
			int idSegment = segment[idRow][idCol];
			if (idSegment < 0 || idSegment >= size_segment){
				cerr << "wrong idSegment = " << idSegment << " size_segment" << size_segment << endl;
			}
			if (crossCheck[idRow][idCol] != 255){
				fitValue[idSegment] += disparityMap[idRow][idCol] * 3;
				numInSegment[idSegment] += 3;
			}
			else {
				fitValue[idSegment] += disparityMap[idRow][idCol];
				++numInSegment[idSegment];
			}
		}
	}

	for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
		for (int idCol = 0; idCol < NUM_COLS; ++idCol){
			int idSegment = segment[idRow][idCol];
			disparityMap[idRow][idCol] = fitValue[idSegment] / numInSegment[idSegment];
		}
	}

	delete[] numInSegment;
	delete[] fitValue;

	clock_t finish = clock();
	cout << " End after " << ((double)(finish - start) / CLOCKS_PER_SEC) << " sec" << endl;
}

void graphCut(float * matchCost[MAX_LABEL], int label2Disparity[], uchar disparityMap[][NUM_COLS], int * * segment){
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
						if (USE_SEGMENT && segment[idRow][idCol] != segment[idRow - 1][idCol]){
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
						if (USE_SEGMENT && segment[idRow][idCol] != segment[idRow][idCol - 1]){
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

//Functions && classes for segmentation
class Edge{
public:
	int node1, node2;
	int weight;

	Edge(int init_node1, int init_node2, int init_weight){
		node1 = init_node1;
		node2 = init_node2;
		weight = init_weight;
	}
};

bool my_edge_compare(Edge edge1, Edge edge2){
	return (edge1.weight < edge2.weight);
}

void segmentation(const Mat & img, Mat & res, int * * segment, int & size_segment) {
	cout << "Segmentation: " << endl;
	clock_t start = clock();
	pyrMeanShiftFiltering(img, res, SPATIAL_RAD, COLOR_RAD, MAX_PYR_LEVEL);

	DisjointSetInt * sets = new DisjointSetInt(img.rows * img.cols);

	vector<Edge> edges;

	for (int idRow = 0; idRow < img.rows; ++idRow){
		for (int idCol = 0; idCol < img.cols; ++idCol){
			segment[idRow][idCol] = -1;//init segment for the next loops over all matrix

			Vec3b colorCur = res.at<Vec3b>(idRow, idCol);//TODO increase speed
			int idCurInDisjoint = idRow * img.cols + idCol;
			if (idRow > 0){
				Vec3b colorUp = res.at<Vec3b>(idRow - 1, idCol);//TODO increase speed
				if (abs(colorCur[0] - colorUp[0]) <= LOWER_COLOR_THRESHOLD && abs(colorCur[1] - colorUp[1]) <= LOWER_COLOR_THRESHOLD 
					&& abs(colorCur[2] - colorUp[2]) <= LOWER_COLOR_THRESHOLD){
					sets->mergeSets((idRow - 1) * img.cols + idCol, idCurInDisjoint);
				}
				else if (abs(colorCur[0] - colorUp[0]) <= UPPER_COLOR_THRESHOLD && abs(colorCur[1] - colorUp[1]) <= UPPER_COLOR_THRESHOLD
					&& abs(colorCur[2] - colorUp[2]) <= UPPER_COLOR_THRESHOLD){
					Edge edge = Edge(idCurInDisjoint, (idRow - 1) * img.cols + idCol, 
						MAX(abs(colorCur[0] - colorUp[0]), MAX(abs(colorCur[1] - colorUp[1]), abs(colorCur[2] - colorUp[2]))));
					edges.push_back(edge);
				}
			}
			if (idCol > 0){
				Vec3b colorLeft = res.at<Vec3b>(idRow, idCol - 1);//TODO increase speed
				if (abs(colorCur[0] - colorLeft[0]) <= LOWER_COLOR_THRESHOLD && abs(colorCur[1] - colorLeft[1]) <= LOWER_COLOR_THRESHOLD
					&& abs(colorCur[2] - colorLeft[2]) <= LOWER_COLOR_THRESHOLD){
					sets->mergeSets(idRow * img.cols + idCol - 1, idCurInDisjoint);
				}
				else if (abs(colorCur[0] - colorLeft[0]) <= UPPER_COLOR_THRESHOLD && abs(colorCur[1] - colorLeft[1]) <= UPPER_COLOR_THRESHOLD
					&& abs(colorCur[2] - colorLeft[2]) <= UPPER_COLOR_THRESHOLD){
					Edge edge = Edge(idCurInDisjoint, idRow * img.cols + idCol - 1,
						MAX(abs(colorCur[0] - colorLeft[0]), MAX(abs(colorCur[1] - colorLeft[1]), abs(colorCur[2] - colorLeft[2]))));
					edges.push_back(edge);
				}
			}
		}
	}

	sort(edges.begin(), edges.end(), my_edge_compare);
	vector<Edge>::iterator it;
	for (it = edges.begin(); it != edges.end(); ++it){
		int node1 = it->node1;
		int node2 = it->node2;

		if (!sets->inSameSet(node1, node2) && (sets->getSizeOfSet(node1) < MIN_SEGMENT_AREA || sets->getSizeOfSet(node2) < MIN_SEGMENT_AREA)){
			sets->mergeSets(node1, node2);
		}
	}

	RNG rng = RNG();
	size_segment = 0;
	for (int idRow = 0; idRow < img.rows; ++idRow){
		for (int idCol = 0; idCol < img.cols; ++idCol){
			int idCurInDisjoint = idRow * img.cols + idCol;
			int head = sets->getHead(idCurInDisjoint);
			int headRow = head / img.cols;
			int headCol = head % img.cols;
			if (segment[headRow][headCol] < 0){
				segment[headRow][headCol] = size_segment;
				++size_segment;
				res.at<Vec3b>(headRow, headCol) = Vec3b(rng(256), rng(256), rng(256));//TODO increase speed
			}
			segment[idRow][idCol] = segment[headRow][headCol];
			res.at<Vec3b>(idRow, idCol) = res.at<Vec3b>(headRow, headCol);//TODO increase speed
		}
	}

	delete sets;

	clock_t finish = clock();
	cout << " End after " << ((double)(finish - start) / CLOCKS_PER_SEC) << " sec" << endl;
	cout << " number of segment: " << size_segment <<  endl;
}

void crossChecking(uchar disparityLeftMap[][NUM_COLS], uchar disparityRightMap[][NUM_COLS], uchar * * crossCheckLeft, uchar * * crossCheckRight){
	for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
		for (int idCol = 0; idCol < NUM_COLS; ++idCol){
			crossCheckLeft[idRow][idCol] = disparityLeftMap[idRow][idCol];
			crossCheckRight[idRow][idCol] = disparityRightMap[idRow][idCol];
		}
	}
	for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
		for (int idCol = 0; idCol < NUM_COLS; ++idCol){
			if (idCol + crossCheckRight[idRow][idCol] >= 0 && idCol + crossCheckRight[idRow][idCol] < NUM_COLS
				&& crossCheckRight[idRow][idCol] == crossCheckLeft[idRow][idCol + crossCheckRight[idRow][idCol]]) {
				//do nothing
			}
			else {
				crossCheckRight[idRow][idCol] = 255;
			}
			if (idCol - crossCheckLeft[idRow][idCol] >= 0 && idCol - crossCheckLeft[idRow][idCol] < NUM_COLS
				&& crossCheckLeft[idRow][idCol] == crossCheckRight[idRow][idCol - crossCheckLeft[idRow][idCol]]) {
				//do nothing
			}
			else {
				crossCheckLeft[idRow][idCol] = 255;
			}
		}
	}
}
