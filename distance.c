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
#define N  ((long long)40)               // number of closest words that will be shown�����ʱչʾ�����뵥���������ƶ���ߵ�ǰ N ������
#define max_w  ((long long)50)           // max length of vocabulary entries���ʻ���е��ʵ���󳤶�
#else
const long long max_size = 2000;         // max length of strings
const long long N = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
#endif

void print_word_vec(char* word_str, float* M, long long size); //��ӡ�������ʵ�����ֵ

int main(int argc, char** argv) {
	FILE* f;
	char st1[max_size];			//�洢�û�������ַ��������ʻ���ӣ�
	char* bestw[N];				//�洢�����뵥����ӽ���ǰ N �����ʡ�
	char file_name[max_size];	//�洢�������ļ���·����
	char st[100][max_size];		//���û�������ַ�����ֳɵ������ʴ洢
	float dist;					//���ڼ���������������������֮����������ƶȡ�
	float len;					//�����ĳ��ȣ�ģ�������ڹ�һ����
	float bestd[N];				//�洢�����뵥����ӽ���ǰ N �����ʵ��������ƶ�ֵ��
	float vec[max_size];		//�û����뵥���������ۻ�ֵ��
	long long words;			//�ļ��а����ĵ���������
	long long size;				//ÿ������������ά�ȡ�
	long long a, b, c, d;		//ѭ��������
	long long cn;				//����ĵ�����
	long long bi[100];			//���뵥���ڴʻ���е�����
	float* M;					//�洢���д������ľ��󣨰��д洢��
	char* vocab;				//�洢�ʻ�������е��ʡ�
	if (argc < 2) {
		printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
		return 0;
	}
	strcpy_s(file_name, sizeof(file_name), argv[1]);// C:/code/word2vec/vectors.bin word2vec����ѵ�������Ķ����������ļ���
	errno_t err = fopen_s(&f, file_name, "rb");
	if (f == NULL || 0 != err) {
		printf("Input file not found\n");
		return -1;
	}
	fscanf_s(f, "%lld", &words);	//71291 (���ַ����洢��������һ���ո�0x20)
	fscanf_s(f, "%lld", &size);	//200 (���ַ����洢��������һ��0x0A==\n==����)
	vocab = (char*)malloc((long long)words * max_w * sizeof(char));//���ʱ�ÿ�������50
	for (a = 0; a < N; a++) bestw[a] = (char*)malloc(max_size * sizeof(char));//���ƶ���ߵĴ��������
	M = (float*)malloc((long long)words * (long long)size * sizeof(float));//ÿ��������200��32λ�������洢
	if (M == NULL) {
		printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
		return -1;
	}
	for (b = 0; b < words; b++) {//71291������ѭ��
		a = 0;
		while (1) {//��һ������
			vocab[b * max_w + a] = (char)fgetc(f);//��һ���ַ�
			if (feof(f) || (vocab[b * max_w + a] == ' ')) break;//�ո���ļ�����
			if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
		}
		vocab[b * max_w + a] = 0;//���ʽ�����
		for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);//��ȡһ�����ʵ�200��float
		len = 0;
		if (0 == b)print_word_vec(&vocab[b * max_w], &M[b * size], size);//�������
		for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];//����ƽ����
		len = (float)sqrt(len);
		for (a = 0; a < size; a++) M[a + b * size] /= len;//��һ��������
		if (0 == b)print_word_vec(&vocab[b * max_w], &M[b * size], size);//�������
	}
	fclose(f);
	while (1) {
		for (a = 0; a < N; a++) bestd[a] = 0;
		for (a = 0; a < N; a++) bestw[a][0] = 0;
		printf("Enter word or sentence (EXIT to break): ");
		a = 0;
		while (1) {//��ȡ�û�����ĵ��ʻ���ӣ����� france
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
			if (st1[c] == ' ') {//������Ǿ��ӣ���Ϊ������ʴ��
				cn++;
				b = 0;
				c++;
			}
		}
		cn++;
		for (a = 0; a < cn; a++) {
			for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
			if (b == words) b = -1;
			bi[a] = b;//�ҵ�ÿ�����ʵ���ţ����� france ��Ӧ vocab[303]
			printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
			if (b == -1) {
				printf("Out of dictionary word!\n");
				break;
			}
		}
		if (b == -1) continue;
		printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
		for (a = 0; a < size; a++) vec[a] = 0;//���������ۼ�ֵ
		for (b = 0; b < cn; b++) {
			if (bi[b] == -1) continue;
			for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
		}
		len = 0;
		for (a = 0; a < size; a++) len += vec[a] * vec[a];
		len = (float)sqrt(len);
		for (a = 0; a < size; a++) vec[a] /= len;
		for (a = 0; a < N; a++) bestd[a] = -1;//��ʼ������
		for (a = 0; a < N; a++) bestw[a][0] = 0;//��ʼ���ַ���
		for (c = 0; c < words; c++) {//�����е��ʼ������
			a = 0;
			for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
			if (a == 1) continue;
			dist = 0;
			for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];//�������
			for (a = 0; a < N; a++) {
				if (dist > bestd[a]) {//�����ҵ��ľ�������ĵ���
					for (d = N - 1; d > a; d--) {//�ƶ�����ĵ���
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
	//��ӡһ�����ʵ�����������ֵ����100��������
	const int count_per_line = 10;//ÿ�еĸ�����
	const int line_count = (int)(size / count_per_line);//����������
	const int less_count = size % count_per_line;//���һ�е���Ŀ
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