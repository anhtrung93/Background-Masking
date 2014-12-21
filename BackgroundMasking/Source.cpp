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
#define btoa(x) ((x)?"true":"false")

using namespace cv;
using namespace std;

int DEFAULT_WIDTH = 0;
int NUM_COLS = DEFAULT_WIDTH;
int DEFAULT_HEIGHT = 0;
int NUM_ROWS = DEFAULT_HEIGHT;
int MIN_DISPARITY = 0;
int MAX_DISPARITY = 0;
int STEP_DISPARITY = 2;
int MAX_LABEL = ((MAX_DISPARITY - MIN_DISPARITY) / STEP_DISPARITY) + 1;

const int CONST_INF = INT_MAX;
float SMOOTH_EFFICIENT = 0.0f;
float SMOOTH_EFFICIENT2 = 0.0f;

double SPATIAL_RAD = 10.0;
double COLOR_RAD = 10.0;
int MAX_PYR_LEVEL = 3;
float LOWER_COLOR_THRESHOLD = 0.0f;
float UPPER_COLOR_THRESHOLD = 0.0f;
int MIN_SEGMENT_AREA = 0;
int NUM_OF_CYCLES = 2;
int BLOCK_SIZE = 1;

bool USE_SEGMENT = true;

bool SHOW_DISPARITY_MAP = false;
bool STORE_DISPARITY_MAP = false;
bool STORE_PLANE_FITTING = false;
bool SHOW_SEGMENTED_IMAGE = false;
bool STORE_SEGMENTED_IMAGE = false;
bool STORE_MATCHING_COST = false;
//92//59
int LOW_THRES = 0;//40
int HIGH_THRES = 0;//100

string SHARED_PATH;
string IMAGE_NAME;
string BACKGROUND_NAME;

string LEFT_IMAGE;
string RIGHT_IMAGE;
string LEFT_BACKGROUND;
string RIGHT_BACKGROUND;
string STORE_PATH;

void segmentation(const Mat &, Mat &, int * *, int &);
void matchingTwoView(const Mat &, const Mat &, const int, int[], float * *);
void graphCut(float * *, int[], uchar * *, int * *);
void planeFitting(uchar * *, int * * segment, int, uchar * *);
void crossChecking(uchar * *, uchar * *, uchar * *, uchar * *);

void readConfigFile(const string &);
void loadImages(const string &, const string &, Mat &, Mat &);
void storeMat(const Mat &, const string &, vector<int> *);

template<class T>
void release2DArray(T * *, int, int);
template<class T>
T * * malloc2DArray(int, int);

const string CONFIG_FILE = "E:\\Workspace\\OpenCV_ws\\Background-Masking\\BackgroundMasking\\ConfigFile.txt";

int main(int argc, char** argv) {
	bool segmentOnly = false;
	bool graphCutOnly = false;
	bool planeFittingOnly = false;
	bool all = false;
	if (argc > 0){
		if (strcmp(argv[1], "segment") == 0){
			segmentOnly = true;
			STORE_SEGMENTED_IMAGE = true;
			readConfigFile(argv[2]);
		}
		else if (strcmp(argv[1], "graphcut") == 0){
			graphCutOnly = true;
			STORE_DISPARITY_MAP = true;
			readConfigFile(argv[2]);
		}
		else if (strcmp(argv[1], "planefitting") == 0){
			planeFittingOnly = true;
			STORE_PLANE_FITTING = true;
			readConfigFile(argv[2]);
		}
		else {
			readConfigFile(argv[1]);
			all = true;
		}
	}
	else {
		readConfigFile(CONFIG_FILE);
		segmentOnly = true;
		graphCutOnly = true;
		planeFittingOnly = true;
		all = true;
	}
	
	mkdir(STORE_PATH.c_str());

	//additional storage, init
	float * * matchRightCost = malloc2DArray<float>(MAX_LABEL, DEFAULT_HEIGHT * DEFAULT_WIDTH);
	float * * matchLeftCost = malloc2DArray<float>(MAX_LABEL, DEFAULT_HEIGHT * DEFAULT_WIDTH);

	int * * segmentLeft = NULL, ** segmentRight = NULL;
	int size_segment_left, size_segment_right;

	Mat leftImage, rightImage;
	loadImages(LEFT_IMAGE, RIGHT_IMAGE, leftImage, rightImage);
	Mat leftBackground, rightBackground;
	loadImages(LEFT_BACKGROUND, RIGHT_BACKGROUND, leftBackground, rightBackground);
	//Segmentation (required RGB images)
	if (USE_SEGMENT) {
		//Init for segmentation
		segmentLeft = malloc2DArray<int>(NUM_ROWS, NUM_COLS);
		segmentRight = malloc2DArray<int>(NUM_ROWS, NUM_COLS);

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
			storeMat(segmentedLeftImage, STORE_PATH + IMAGE_NAME + "-segmented-L.jpg", NULL);
			storeMat(segmentedRightImage, STORE_PATH + IMAGE_NAME + "-segmented-R.jpg", NULL);
		}
		segmentedLeftImage.release();
		segmentedRightImage.release();
	}

	if (segmentOnly) return 0;

	//Convert to GRAY images for matching
	cvtColor(leftImage, leftImage, CV_BGR2GRAY);
	cvtColor(rightImage, rightImage, CV_BGR2GRAY);
	//Matching
	int * label2Disparity = new int[MAX_LABEL];
	for (int id = 0, disparity = MIN_DISPARITY; id < MAX_LABEL; ++id, disparity += 2){
		label2Disparity[id] = disparity;
	}
	matchingTwoView(rightImage, leftImage, 1, label2Disparity, matchRightCost);
	matchingTwoView(leftImage, rightImage, -1, label2Disparity, matchLeftCost);

	leftImage.release(); rightImage.release();
	//Store matching results
	if (STORE_MATCHING_COST) {
		cout << "Storing: " << endl;
		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
		compression_params.push_back(100);
		for (int idLabel = 0; idLabel < MAX_LABEL; ++idLabel){
			cout << ".";
			Mat temp = Mat(NUM_ROWS, NUM_COLS, CV_32FC1, matchLeftCost[idLabel]);
			storeMat(temp, STORE_PATH + IMAGE_NAME + "-L-" + to_string(idLabel) + ".jpg", &compression_params);
			temp.release();
			Mat temp2 = Mat(NUM_ROWS, NUM_COLS, CV_32FC1, matchRightCost[idLabel]);
			storeMat(temp2, STORE_PATH + IMAGE_NAME + "-R-" + to_string(idLabel) + ".jpg", &compression_params);
			temp2.release();
		}
		cout << endl;
	}
	//Graph Cut
	uchar * * disparityRightMap = malloc2DArray<uchar>(NUM_ROWS, NUM_COLS);
	uchar * * disparityLeftMap = malloc2DArray<uchar>(NUM_ROWS, NUM_COLS);

	graphCut(matchRightCost, label2Disparity, disparityRightMap, segmentRight);
	graphCut(matchLeftCost, label2Disparity, disparityLeftMap, segmentLeft);

	//Release unnecessary memory
	release2DArray<float>(matchLeftCost, MAX_LABEL, DEFAULT_WIDTH * DEFAULT_HEIGHT);
	release2DArray<float>(matchRightCost, MAX_LABEL, DEFAULT_WIDTH * DEFAULT_HEIGHT);

	for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
		for (int idCol = 0; idCol < NUM_COLS; ++idCol){
			disparityLeftMap[idRow][idCol] = label2Disparity[disparityLeftMap[idRow][idCol]];
			disparityRightMap[idRow][idCol] = label2Disparity[disparityRightMap[idRow][idCol]];
		}
	}

	if (label2Disparity != NULL){
		delete[] label2Disparity;
		label2Disparity = NULL;
	}

	//crossChecking(disparityLeftMap, disparityRightMap, crossCheckLeft, crossCheckRight);
	//Store & show disparity Map
	if (STORE_DISPARITY_MAP) {
		Mat disparityRightMat = Mat(NUM_ROWS, NUM_COLS, CV_8UC1);
		Mat disparityLeftMat = Mat(NUM_ROWS, NUM_COLS, CV_8UC1);
		for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol){
				disparityLeftMat.at<uchar>(idRow, idCol) = disparityLeftMap[idRow][idCol];
				disparityRightMat.at<uchar>(idRow, idCol) = disparityRightMap[idRow][idCol];
			}

		}
		storeMat(disparityRightMat, STORE_PATH + IMAGE_NAME + "-cutR.jpg", NULL);
		disparityRightMat.release();
		storeMat(disparityLeftMat, STORE_PATH + IMAGE_NAME + "-cutL.jpg", NULL);
		disparityLeftMat.release();
	}

	if (graphCutOnly) return 0;

	//Cross checking
	uchar * * crossCheckRight = malloc2DArray<uchar>(NUM_ROWS, NUM_COLS);
	uchar * * crossCheckLeft = malloc2DArray<uchar>(NUM_ROWS, NUM_COLS);

	//PlaneFitting
	if (USE_SEGMENT){
		planeFitting(disparityLeftMap, segmentLeft, size_segment_left, crossCheckLeft);
		planeFitting(disparityRightMap, segmentRight, size_segment_right, crossCheckRight);
	}

	if (USE_SEGMENT){
		release2DArray<int>(segmentLeft, NUM_ROWS, NUM_COLS);
		release2DArray<int>(segmentRight, NUM_ROWS, NUM_COLS);
	}

	release2DArray<uchar>(crossCheckLeft, NUM_ROWS, NUM_COLS);
	release2DArray<uchar>(crossCheckRight, NUM_ROWS, NUM_COLS);

	//Store & show disparity Map
	if (STORE_PLANE_FITTING) {
		Mat disparityRightMat = Mat(NUM_ROWS, NUM_COLS, CV_8UC1);
		Mat disparityLeftMat = Mat(NUM_ROWS, NUM_COLS, CV_8UC1);
		for (int idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol){
				disparityLeftMat.at<uchar>(idRow, idCol) = disparityLeftMap[idRow][idCol];
				disparityRightMat.at<uchar>(idRow, idCol) = disparityRightMap[idRow][idCol];
			}
			
		}
		storeMat(disparityRightMat, STORE_PATH + IMAGE_NAME + "-fitR.jpg", NULL);
		disparityRightMat.release();
		storeMat(disparityLeftMat, STORE_PATH + IMAGE_NAME + "-fitL.jpg", NULL);
		disparityLeftMat.release();
	}

	if (SHOW_DISPARITY_MAP) {
		Mat disparityRightMat = Mat(NUM_ROWS, NUM_COLS, CV_8UC1, disparityRightMap);
		Mat disparityLeftMat = Mat(NUM_ROWS, NUM_COLS, CV_8UC1, disparityLeftMap);
		imshow("Disparity right map", disparityRightMat);
		imshow("Disparity left map", disparityLeftMat);
		waitKey();
		disparityRightMat.release();
		disparityLeftMat.release();
	}

	if (planeFittingOnly) return 0;

	//Background Masking
	loadImages(LEFT_IMAGE, RIGHT_IMAGE, leftImage, rightImage);

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
				leftImage.at<Vec3b>(idRow, idCol) = leftBackground.at<Vec3b>(idRow, idCol);
			}
			if (disparityRightMap[idRow][idCol] < LOW_THRES || disparityRightMap[idRow][idCol] > HIGH_THRES || sets2->getSizeOfSet(idCurJointSet) != max2){
				rightImage.at<Vec3b>(idRow, idCol) = rightBackground.at<Vec3b>(idRow, idCol);
			}
		}
	}

	delete sets1;
	delete sets2;

	storeMat(leftImage, STORE_PATH + IMAGE_NAME + "-maskL.jpg", NULL);
	storeMat(rightImage, STORE_PATH + IMAGE_NAME + "-maskR.jpg", NULL);

	leftImage.release(); rightImage.release();
	leftBackground.release(); rightBackground.release();

	release2DArray<uchar>(disparityLeftMap, NUM_ROWS, NUM_COLS);
	release2DArray<uchar>(disparityRightMap, NUM_ROWS, NUM_COLS);

	return 0;
}

void trim(string & str){
	int i = 0, j = str.length() - 1;
	while (str[i] == ' ' || str[i] == '\t'){
		++i;
	}
	while (str[j] == ' ' || str[j] == '\t' || str[j] == '\n'){
		--j;
	}
	str = str.substr(i, j - i + 1);
}

void readConfigFile(const string & CONFIG_FILE){
	char temp[256];

	freopen(CONFIG_FILE.c_str(), "r", stdin);

	scanf("DEFAULT_WIDTH"); scanf("%d\n", &DEFAULT_WIDTH);
	scanf("DEFAULT_HEIGHT"); scanf("%d\n", &DEFAULT_HEIGHT);
	scanf("MIN_DISPARITY"); scanf("%d\n", &MIN_DISPARITY);
	scanf("MAX_DISPARITY"); scanf("%d\n", &MAX_DISPARITY);
	scanf("STEP_DISPARITY"); scanf("%d\n", &STEP_DISPARITY);

	NUM_COLS = DEFAULT_WIDTH;
	NUM_ROWS = DEFAULT_HEIGHT;
	MAX_LABEL = ((MAX_DISPARITY - MIN_DISPARITY) / STEP_DISPARITY) + 1;

	scanf("SMOOTH_EFFICIENT"); scanf("%f\n", &SMOOTH_EFFICIENT);
	scanf("SMOOTH_EFFICIENT2"); scanf("%f\n", &SMOOTH_EFFICIENT2);
	/*scanf("SPATIAL_RAD"); scanf("%lf\n", &SPATIAL_RAD);
	scanf("COLOR_RAD"); scanf("%lf\n", &COLOR_RAD);
	scanf("MAX_PYR_LEVEL"); scanf("%d\n", &MAX_PYR_LEVEL);*/
	scanf("LOWER_COLOR_THRESHOLD"); scanf("%f\n", &LOWER_COLOR_THRESHOLD);
	scanf("UPPER_COLOR_THRESHOLD"); scanf("%f\n", &UPPER_COLOR_THRESHOLD);
	scanf("MIN_SEGMENT_AREA"); scanf("%d\n", &MIN_SEGMENT_AREA);
	//scanf("NUM_OF_CYCLES"); scanf("%d\n", &NUM_OF_CYCLES);
	scanf("BLOCK_SIZE"); scanf("%d\n", &BLOCK_SIZE);
	//scanf("USE_SEGMENT"); scanf("%s\n", &temp);
	//USE_SEGMENT = (strcmp(temp, "true") == 0) ? true : false;
	//scanf("SHOW_DISPARITY_MAP"); scanf("%s\n", temp);
	//SHOW_DISPARITY_MAP = (strcmp(temp, "true") == 0) ? true : false;
	//scanf("STORE_DISPARITY_MAP"); scanf("%s\n", temp);
	//STORE_DISPARITY_MAP = (strcmp(temp, "true") == 0) ? true : false;
	//scanf("SHOW_SEGMENTED_IMAGE"); scanf("%s\n", temp);
	//SHOW_SEGMENTED_IMAGE = (strcmp(temp, "true") == 0) ? true : false;
	//scanf("STORE_SEGMENTED_IMAGE"); scanf("%s\n", temp);
	//STORE_SEGMENTED_IMAGE = (strcmp(temp, "true") == 0) ? true : false;
	//scanf("STORE_MATCHING_COST"); scanf("%s\n", temp);
	//STORE_MATCHING_COST = (strcmp(temp, "true") == 0) ? true : false;
	scanf("LOW_THRES"); scanf("%d\n", &LOW_THRES);
	scanf("HIGH_THRES"); scanf("%d\n", &HIGH_THRES);

	scanf("LEFT_IMAGE"); cin.getline(temp, 256);
	LEFT_IMAGE = temp; trim(LEFT_IMAGE);
	scanf("LEFT_BACKGROUND"); cin.getline(temp, 256);
	LEFT_BACKGROUND = temp; trim(LEFT_BACKGROUND);

	IMAGE_NAME = LEFT_IMAGE.substr(LEFT_IMAGE.length() - 14, 8);
	BACKGROUND_NAME = LEFT_BACKGROUND.substr(LEFT_BACKGROUND.length() - 14, 8);
	RIGHT_IMAGE = LEFT_IMAGE.substr(0, LEFT_IMAGE.length() - 6) + "-R.jpg";
	RIGHT_BACKGROUND = LEFT_BACKGROUND.substr(0, LEFT_BACKGROUND.length() - 6) + "-R.jpg";

	scanf("STORE_PATH"); cin.getline(temp, 256);
	STORE_PATH = temp; trim(STORE_PATH);

	/*printf("%d\n", DEFAULT_WIDTH);
	printf("%d\n", DEFAULT_HEIGHT);
	printf("%d\n", MIN_DISPARITY);
	printf("%d\n", MAX_DISPARITY);
	printf("%d\n", STEP_DISPARITY);
	printf("%f\n", SMOOTH_EFFICIENT);
	printf("%f\n", SMOOTH_EFFICIENT2);
	printf("%lf\n", SPATIAL_RAD);
	printf("%lf\n", COLOR_RAD);
	printf("%d\n", MAX_PYR_LEVEL);
	printf("%f\n", LOWER_COLOR_THRESHOLD);
	printf("%f\n", UPPER_COLOR_THRESHOLD);
	printf("%d\n", MIN_SEGMENT_AREA);
	printf("%d\n", NUM_OF_CYCLES);
	printf("%d\n", BLOCK_SIZE);
	printf("%s\n", btoa(USE_SEGMENT));
	printf("%s\n", btoa(SHOW_DISPARITY_MAP));
	printf("%s\n", btoa(STORE_DISPARITY_MAP));
	printf("%s\n", btoa(SHOW_SEGMENTED_IMAGE));
	printf("%s\n", btoa(STORE_SEGMENTED_IMAGE));
	printf("%s\n", btoa(STORE_MATCHING_COST));
	printf("%d\n", LOW_THRES);
	printf("%d\n", HIGH_THRES);

	cout << "\"" << IMAGE_NAME << "\"" << endl;
	cout << "\"" << BACKGROUND_NAME << "\"" << endl;
	cout << "\"" << LEFT_IMAGE << "\"" << endl;
	cout << "\"" << RIGHT_IMAGE << "\"" << endl;
	cout << "\"" << LEFT_BACKGROUND << "\"" << endl;
	cout << "\"" << RIGHT_BACKGROUND << "\"" << endl;
	cout << "\"" << STORE_PATH << "\"" << endl;*/
}

template<class T>
void release2DArray(T * * arr, int numRows, int numCols){
	if (arr == NULL){
		return;
	}
	for (int idRow = 0; idRow < numRows; ++idRow){
		if (arr[idRow] != NULL){
			delete[] arr[idRow];
			arr[idRow] = NULL;
		}
	}
	delete[] arr;
	arr = NULL;
}

template<class T>
T * * malloc2DArray(int numRows, int numCols){
	T * * result = new T *[numRows];
	for (int idRow = 0; idRow < numRows; ++idRow){
		result[idRow] = new T[numCols];
	}
	return result;
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

void planeFitting(uchar * * disparityMap, int * * segment, int size_segment, uchar * * crossCheck){
	cout << "PlaneFitting: " << endl;
	clock_t start = clock();

	int * numInSegment = new int[size_segment];
	unsigned long long * fitValue = new unsigned long long[size_segment];
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

void graphCut(float * * matchCost, int label2Disparity[], uchar * * disparityMap, int * * segment){
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

void matchingTwoView(const Mat & referImage, const Mat & searchImage, const int CONST_REF, int disparityList[], float * * matchCost){
	cout << "Matching Two Views: ";
	clock_t start = clock();

	unsigned char * refer = (unsigned char *)(referImage.data);
	unsigned char * search = (unsigned char *)(searchImage.data);

	for (int label = 0; label < MAX_LABEL; ++label){
		int idDisparity = disparityList[label];
		for (int pp = 0, idRow = 0; idRow < NUM_ROWS; ++idRow){
			for (int idCol = 0; idCol < NUM_COLS; ++idCol, ++pp){
				int targetCol = idCol + CONST_REF * idDisparity;
				if (targetCol >= 0 && targetCol < NUM_COLS){
					//block matching
					int blockSize = 0;	float temporary = 0.0f;
					matchCost[label][pp] = 0;
					int halfBlockSize = (BLOCK_SIZE - 1) / 2;
					for (int idRow2 = -halfBlockSize; idRow2 <= halfBlockSize; ++idRow2){
						for (int idCol2 = -halfBlockSize; idCol2 <= halfBlockSize; ++idCol2){
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
	cout << " number of segment: " << size_segment << endl;
}

void crossChecking(uchar * * disparityLeftMap, uchar * * disparityRightMap, uchar * * crossCheckLeft, uchar * * crossCheckRight){
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
