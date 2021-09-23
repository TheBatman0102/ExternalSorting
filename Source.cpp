#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <queue>
#include <chrono>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <windows.h>

typedef unsigned long long ull;
using namespace std;

ull numOfChunk = 0;
ull FileIndex = 0;
ull mem_lim = 0;
ull RAM = 0;

ull sort_chunk(char* input) {
	//open input file
	FILE* fi = freopen(input, "r", stdin);

	if (fi == NULL) {
		cout << "Error opening file";
		exit(1);
	}

	//calculate input file's size
	fseek(fi, 0, SEEK_END);
	ull file_size = ftell(fi);
	fseek(fi, 0, SEEK_SET);

	bool lastLine = false;

	ull curFileNo = 0;
	char curFileName[26];

	while (ftell(fi) != file_size) {
		vector<string> temp;
		ull curReadSize = 0;
		string bufLastLine;

		while (1) {
			ull curPos = ftell(fi);
			string buf;
			getline(cin, buf);

			curReadSize += buf.size();

			if (curReadSize >= mem_lim/8*9) {
				if (ftell(fi) != file_size)
					fseek(fi, curPos, SEEK_SET);
				else {
					lastLine = true;
					bufLastLine = buf;
				}
				break;
			}
			temp.push_back(buf);
			if (ftell(fi) == file_size)
				break;
		}

		sort(temp.begin(), temp.end());

		curFileNo++;
		sprintf(curFileName, "trash//%lld.txt", curFileNo);

		ofstream outFile;
		outFile.open(curFileName, ofstream::out | ofstream::trunc);
		//for (const auto& s : temp)
		//	outFile << s << endl;
		copy(temp.begin(), temp.end(), ostream_iterator<string>(outFile, "\n"));

		outFile.close();

		if (lastLine) {
			curFileNo++;
			sprintf(curFileName, "trash//%lld.txt", curFileNo);
			outFile.open(curFileName, ofstream::out | ofstream::trunc);
			outFile << bufLastLine << endl;
			outFile.close();
			break;
		}
	}
	fclose(fi);
	return curFileNo;
}

ull mergeFiles(ull start, ull end, char* output = NULL) {
	if (start >= end) return start;
	char tempName[26];
	char* outFileName;
	ull outFileIndex;

	if (start == 1 && end == numOfChunk) outFileName = output;
	else {
		sprintf(tempName, "trash//%lld.txt", ++FileIndex);
		outFileName = tempName;
	}

	outFileIndex = FileIndex;
	ull mid = start + (end - start) / 2;
	ull file1 = mergeFiles(start, mid);
	ull file2 = mergeFiles(mid + 1, end);
	char file1name[16], file2name[16];
	sprintf(file1name, "trash//%lld.txt", file1);
	sprintf(file2name, "trash//%lld.txt", file2);
	ifstream if1, if2;
	ofstream outFile;

	if1.open(file1name, ifstream::in);
	if2.open(file2name, ifstream::in);
	outFile.open(outFileName, ofstream::out | ofstream::trunc);

	string buf1, buf2, content;
	char whichBuf;
	bool file1remain = true, file2remain = true;

	getline(if1, buf1);
	getline(if2, buf2);
	if (buf1 < buf2) {
		whichBuf = 1;
		content = buf1 + '\n';
	}
	else {
		whichBuf = 2;
		content = buf2 + '\n';
	}

	while (1) {
		if (whichBuf == 1) {
			if (!getline(if1, buf1)) {
				file1remain = false;
				buf1.clear();
				buf1.shrink_to_fit();
				whichBuf = 2;
			}
		}
		else {
			if (!getline(if2, buf2)) {
				file2remain = false;
				buf2.clear();
				buf2.shrink_to_fit();
				whichBuf = 1;
			}
		}
		if (!file1remain && !file2remain)
			break;
		if (!file1remain)
			content += buf2 + '\n';
		else if (!file2remain)
			content += buf1 + '\n';
		else {
			if (buf1 < buf2) {
				content += buf1 + '\n';
				whichBuf = 1;
			}
			else {
				content += buf2 + '\n';
				whichBuf = 2;
			}
		}

		if (content.capacity()+buf1.capacity()+buf2.capacity() >= mem_lim/7*4)
		{
			outFile << content;
			content.clear();
			content.shrink_to_fit();
		}

	}
	if (!content.empty())
		outFile << content;
	if1.close();
	if2.close();
	outFile.close();

	return outFileIndex;

}

void merge_chunks(char* output) {
	FileIndex = numOfChunk;
	if (numOfChunk == 1) {
		ifstream inFile("trash//1.txt");
		ofstream outFile(output);
		string content = "";
		string buf;
		while (getline(inFile, buf))
			content += buf + '\n';
		inFile.close();
		outFile << content;
		outFile.close();
	}
	else
		mergeFiles(1, numOfChunk, output);
}

void sort_process(char* input, char* output) {
	//Divide the file into various chunks and sort every chunk
	numOfChunk = sort_chunk(input);

	merge_chunks(output);
}

int main(int argc, char** argv) {

	/*Debugging*/

	char in_file[] = "test4.txt";
	char out_file[] = "res4.txt";
	RAM = 1024 * 1024 * 30;

	/*End of Debugging*/

	/*if (argc < 4) {
		cout << "Usage: main.cpp <input file name> <output file name> <memory limit>";
		exit(1);
	}

	char* in_file = argv[1], * out_file = argv[2];
	mem_lim = atoll(argv[3]);*/

	mem_lim = RAM / 2;
	auto start = clock();
	sort_process(in_file, out_file);
	auto end = clock(); cout << "Running Time: " << (end - start) / CLOCKS_PER_SEC;
	Sleep(30 * 1000);
	return 0;
}