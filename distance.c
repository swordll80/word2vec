//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#ifdef _WIN32
#define max_size ((long long)2000)       // max length of strings
#define N  ((long long)40)               // number of closest words that will be shown，输出时展示与输入单词余弦相似度最高的前 N 个单词
#define max_w  ((long long)50)           // max length of vocabulary entries，词汇表中单词的最大长度
#else
const long long max_size = 2000;         // max length of strings
const long long N = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
#endif

void print_word_vec(char* word_str, float* M, long long size); //打印单个单词的向量值

int main(int argc, char** argv) {
	FILE* f;
	char st1[max_size];			//存储用户输入的字符串（单词或句子）
	char* bestw[N];				//存储与输入单词最接近的前 N 个单词。
	char file_name[max_size];	//存储词向量文件的路径。
	char st[100][max_size];		//将用户输入的字符串拆分成单个单词存储
	float dist;					//用于计算输入向量与其他向量之间的余弦相似度。
	float len;					//向量的长度（模），用于归一化。
	float bestd[N];				//存储与输入单词最接近的前 N 个单词的余弦相似度值。
	float vec[max_size];		//用户输入单词向量的累积值。
	long long words;			//文件中包含的单词总数。
	long long size;				//每个单词向量的维度。
	long long a, b, c, d;		//循环变量。
	long long cn;				//输入的单词数
	long long bi[100];			//输入单词在词汇表中的索引
	float* M;					//存储所有词向量的矩阵（按行存储）
	char* vocab;				//存储词汇表中所有单词。
	if (argc < 2) {
		printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
		return 0;
	}
	strcpy_s(file_name, sizeof(file_name), argv[1]);// C:/code/word2vec/vectors.bin word2vec程序训练出来的二进制数据文件。
	errno_t err = fopen_s(&f, file_name, "rb");
	if (f == NULL || 0 != err) {
		printf("Input file not found\n");
		return -1;
	}
	fscanf_s(f, "%lld", &words);	//71291 (用字符串存储，后面是一个空格0x20)
	fscanf_s(f, "%lld", &size);	//200 (用字符串存储，后面是一个0x0A==\n==换行)
	vocab = (char*)malloc((long long)words * max_w * sizeof(char));//单词表，每个单词最长50
	for (a = 0; a < N; a++) bestw[a] = (char*)malloc(max_size * sizeof(char));//相似度最高的词条或句子
	M = (float*)malloc((long long)words * (long long)size * sizeof(float));//每个单词用200个32位浮点数存储
	if (M == NULL) {
		printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
		return -1;
	}
	for (b = 0; b < words; b++) {//71291个单词循环
		a = 0;
		while (1) {//读一个单词
			vocab[b * max_w + a] = (char)fgetc(f);//读一个字符
			if (feof(f) || (vocab[b * max_w + a] == ' ')) break;//空格或文件结束
			if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
		}
		vocab[b * max_w + a] = 0;//单词结束符
		for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);//读取一个单词的200个float
		len = 0;
		if (0 == b)print_word_vec(&vocab[b * max_w], &M[b * size], size);//输出测试
		for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];//向量平方和
		len = (float)sqrt(len);
		for (a = 0; a < size; a++) M[a + b * size] /= len;//归一化向量？
		if (0 == b)print_word_vec(&vocab[b * max_w], &M[b * size], size);//输出测试
	}
	fclose(f);
	while (1) {
		for (a = 0; a < N; a++) bestd[a] = 0;
		for (a = 0; a < N; a++) bestw[a][0] = 0;
		printf("Enter word or sentence (EXIT to break): ");
		a = 0;
		while (1) {//读取用户输入的单词或句子，例如 france
			st1[a] = (char)fgetc(stdin);
			if ((st1[a] == '\n') || (a >= max_size - 1)) {
				st1[a] = 0;
				break;
			}
			a++;
		}
		if (!strcmp(st1, "EXIT")) break;
		cn = 0;
		b = 0;
		c = 0;
		while (1) {
			st[cn][b] = st1[c];
			b++;
			c++;
			st[cn][b] = 0;
			if (st1[c] == 0) break;
			if (st1[c] == ' ') {//输入的是句子，拆为多个单词存放
				cn++;
				b = 0;
				c++;
			}
		}
		cn++;
		for (a = 0; a < cn; a++) {
			for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
			if (b == words) b = -1;
			bi[a] = b;//找到每个单词的序号，例如 france 对应 vocab[303]
			printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
			if (b == -1) {
				printf("Out of dictionary word!\n");
				break;
			}
		}
		if (b == -1) continue;
		printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
		for (a = 0; a < size; a++) vec[a] = 0;//单词向量累计值
		for (b = 0; b < cn; b++) {
			if (bi[b] == -1) continue;
			for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
		}
		len = 0;
		for (a = 0; a < size; a++) len += vec[a] * vec[a];
		len = (float)sqrt(len);
		for (a = 0; a < size; a++) vec[a] /= len;
		for (a = 0; a < N; a++) bestd[a] = -1;//初始化距离
		for (a = 0; a < N; a++) bestw[a][0] = 0;//初始化字符串
		for (c = 0; c < words; c++) {//与所有单词计算距离
			a = 0;
			for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
			if (a == 1) continue;
			dist = 0;
			for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];//距离计算
			for (a = 0; a < N; a++) {
				if (dist > bestd[a]) {//插入找到的距离更近的单词
					for (d = N - 1; d > a; d--) {//移动后面的单词
						bestd[d] = bestd[d - 1];
						strcpy_s(bestw[d], max_size, bestw[d - 1]);
					}
					bestd[a] = dist;
					strcpy_s(bestw[a], max_size, &vocab[c * max_w]);
					break;
				}
			}
		}
		for (a = 0; a < N; a++) printf("%50s\t\t%f\n", bestw[a], bestd[a]);
	}
	return 0;
}

void print_word_vec(char* word_str, float* M, long long size) {
	//打印一个单词的向量，典型值就是100个浮点数
	const int count_per_line = 10;//每行的浮点数
	const int line_count = (int)(size / count_per_line);//完整的行数
	const int less_count = size % count_per_line;//最后一行的数目
	printf("%s\n", NULL == word_str ? "NULL" : word_str);
	for (int line_index = 0; line_index < line_count; ++line_index) {
		for (int data_index = 0; data_index < count_per_line; ++data_index) {
			printf("%10.6f ", M[line_index * count_per_line + data_index]);
		}
		printf("\n");
	}
	if (less_count > 0) {
		for (int data_index = 0; data_index < less_count; ++data_index) {
			printf("%10.6f ", M[line_count * count_per_line + data_index]);
		}
		printf("\n");
	}
}