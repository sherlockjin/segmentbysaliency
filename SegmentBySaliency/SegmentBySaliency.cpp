// SegmentBySaliency.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <tuple>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace std;
using namespace cv;

#define ARGC_MIN               5


///////////////////////////////////////////////////////////////////////////////////////////////////
// New on August 1, 2012 by Yan Qiong
// Get all files specified by fname (single or multiple)
///////////////////////////////////////////////////////////////////////////////////////////////////
int GetFiles(char *fname, vector<string> &files)
{
	files.clear();
	WIN32_FIND_DATAA fileFindData;
	HANDLE hFind = FindFirstFileA(fname, &fileFindData);
	if (hFind == INVALID_HANDLE_VALUE)
		return 0;

	do
	{
		if (fileFindData.cFileName[0] == '.')
			continue; // filter the '..' and '.' in the path
		if (fileFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue; // Ignore sub-folders
		files.push_back(fileFindData.cFileName);
	} while (FindNextFileA(hFind, &fileFindData));
	FindClose(hFind);
	return (int)files.size();
}

//int segment(string originPic, string saliencyPic, string saliencyValue, string filename){
//	//默认方式是以彩色图读
//	Mat originPicMat = imread(originPic);
//	if (originPicMat.empty())
//	{
//		printf("Cannot open file \"%s\"\nExiting...\n", originPic);
//		return 0;
//	}
//	if (originPicMat.channels() != 3)
//	{
//		printf("Cannot process gray-scale image\nExiting..\n");
//		return 0;
//	}
//	
//
//	//注意要有第二个参数，这是以灰度图读
//	Mat saliencyPicMat = imread(saliencyPic,0);
//	if (saliencyPicMat.empty())
//	{
//		printf("Cannot open file \"%s\"\nExiting...\n", saliencyPic);
//		return 0;
//	}
//	
//	//得到显著性值
//	vector<int> saliencyValues;
//	ifstream saliencyValueTxt(saliencyValue);
//	if (saliencyValueTxt){
//		int saliency;
//		while (saliencyValueTxt >> saliency){
//			saliencyValues.push_back(saliency);
//		}
//		saliencyValueTxt.close();
//	}
//	else{
//		return 0;
//	}
//
//	
//	for (int level = 0; level < saliencyValues.size(); level++){
//
//		char tmpname[2000];
//		sprintf(tmpname, "res\\%s_%d.jpg", filename.data(),saliencyValues.at(level));
//
//		//cout << saliencyValues.at(i) << endl;
//		Mat tmpMat;
//		/// 创建与src同类型和大小的矩阵(dst)  
//		//tmpMat.create(originPicMat.size(), originPicMat.type());
//		originPicMat.copyTo(tmpMat);
//		//imwrite("origin.jpg", tmpMat);
//		//cout << "copy finished!" << endl;
//		int nRows = saliencyPicMat.rows;
//		int nCols = saliencyPicMat.cols;
//		int nColsOrigin = tmpMat.cols;
//		/*if (saliencyPicMat.isContinuous())
//		{
//			nCols *= nRows;
//			nRows = 1;
//		}*/
//		int i, j;
//		uchar* saliencyPtr;
//		//ofstream s(tmpname);
//		for (i = 0; i < nRows; i++){
//			saliencyPtr = saliencyPicMat.ptr<uchar>(i);			 
//			for (j = 0; j < nCols; j++){
//				//s << (int)saliencyPtr[j] << " ";
//				if ((int)saliencyPtr[j] != saliencyValues.at(level)){
//					//cout << "hello" << endl;
//					tmpMat.at<Vec3b>(i, j)[0] = 0;
//					tmpMat.at<Vec3b>(i, j)[1] = 0;
//					tmpMat.at<Vec3b>(i, j)[2] = 0;
//
//				}
//				
//			}
//			//s << endl;
//		}
//		
//		imwrite(tmpname, tmpMat);
//		
//	}
//	return 1;
//
//	
//
//
//}


int segment(string originPic, string saliencyPic, string saliencyValue, string filename){
	//默认方式是以彩色图读
	Mat originPicMat = imread(originPic);
	if (originPicMat.empty())
	{
		printf("Cannot open file \"%s\"\nExiting...\n", originPic);
		return 0;
	}
	if (originPicMat.channels() != 3)
	{
		printf("Cannot process gray-scale image\nExiting..\n");
		return 0;
	}


	//注意要有第二个参数，这是以灰度图读
	Mat saliencyPicMat = imread(saliencyPic, 0);
	if (saliencyPicMat.empty())
	{
		printf("Cannot open file \"%s\"\nExiting...\n", saliencyPic);
		return 0;
	}

	//得到显著性值
	vector<tuple<int,int,int>> saliencyValues;
	ifstream saliencyValueTxt(saliencyValue);
	if (saliencyValueTxt){
		int minSaliency,maxSaliency,saliencyCount;
		while (saliencyValueTxt >> minSaliency){
			saliencyValueTxt >> maxSaliency;
			saliencyValueTxt >> saliencyCount;
			tuple<int, int, int> tmp(minSaliency, maxSaliency, saliencyCount);
			saliencyValues.push_back(tmp);
		}
		saliencyValueTxt.close();
	}
	else{
		return 0;
	}


	for (int level = 0; level < saliencyValues.size(); level++){

		char tmpname[2000];
		int minSaliency, maxSaliency, saliencyCount;
		sprintf(tmpname, "res\\%s_%d_%d.jpg", filename.data(), get<0>(saliencyValues.at(level)), get<1>(saliencyValues.at(level)));

		//cout << saliencyValues.at(i) << endl;
		Mat tmpMat;
		/// 创建与src同类型和大小的矩阵(dst)  
		//tmpMat.create(originPicMat.size(), originPicMat.type());
		originPicMat.copyTo(tmpMat);
		//imwrite("origin.jpg", tmpMat);
		//cout << "copy finished!" << endl;
		int nRows = saliencyPicMat.rows;

		int nCols = saliencyPicMat.cols;
		int nColsOrigin = tmpMat.cols;
		/*if (saliencyPicMat.isContinuous())
		{
		nCols *= nRows;
		nRows = 1;
		}*/
		int i, j;
		uchar* saliencyPtr;
		//ofstream s(tmpname);
		for (i = 0; i < nRows; i++){
			saliencyPtr = saliencyPicMat.ptr<uchar>(i);
			for (j = 0; j < nCols; j++){
				//s << (int)saliencyPtr[j] << " ";
				if ((int)saliencyPtr[j] < get<0>(saliencyValues.at(level)) || (int)saliencyPtr[j] > get<1>(saliencyValues.at(level))){
					//cout << "hello" << endl;
					tmpMat.at<Vec3b>(i, j)[0] = 0;
					tmpMat.at<Vec3b>(i, j)[1] = 0;
					tmpMat.at<Vec3b>(i, j)[2] = 0;

				}

			}
			//s << endl;
		}

		imwrite(tmpname, tmpMat);

	}
	return 1;




}

int main(int argc, char* argv[])
{
	if (argc<ARGC_MIN)
	{
		printf("You must specify the input image and output dir\n");
		printf("第二个参数是原图的地址");
		printf("第三个参数是显著性图的目录");
		printf("第四个参数是显著性值的目录");
		printf("第五个参数是输出目录");
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	// find input 
	//
	///////////////////////////////////////////////////////////////////////////////////////////////
	int nfiles;
	vector<string> files;
	nfiles = GetFiles(argv[1], files);
	if (nfiles == 0)
	{
		printf("The specified path is wrong.\n");
		return 0;
	}
	else{
		// Save parameters in the output directory
		//SaveParams(argv[2], argc, argv);


		// Find src_pic  directory
		string originPicDir(argv[1]);
		int id = strlen(argv[1]) - 1;

		while (argv[1][id] != '\\' && argv[1][id] != '/' && id>0) id--;
		if (id>0)
		{
			originPicDir = originPicDir.substr(0, id+1);
		}
		else
		{
			id = -1;
			originPicDir = "";
		}
		//find saliency directory
		char bufferSrcSaliencyDir[1024];
		strcpy_s(bufferSrcSaliencyDir, argv[2]);

		for (int i = 0; i<nfiles; i++)
		{
			// process each image specified in the input argument
			string filename = files[i].substr(0, files[i].size() - 4);
			string originPic = originPicDir+files[i];
			string saliencyPic(argv[2]);
			string saliencyValue(argv[3]);
			saliencyPic = saliencyPic+filename+"_HS.png";
			saliencyValue = saliencyValue+filename+".txt";
			printf("(%d/%d) %s\n", i + 1, nfiles, originPic.data());
			int flag = segment(originPic, saliencyPic, saliencyValue, filename);
			if (flag == 0){
				printf("finished!");
				break;
			}

		}
	}
	system("pause");
	return 0;
}

