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
#define max_size ((long long)2000)			// max length of strings
#define closest_word_count  ((long long)40) // number of closest words that will be shown�����ʱչʾ�����뵥���������ƶ���ߵ�ǰ N ������
#define max_word_len  ((long long)50)		// max length of vocabulary entries���ʻ���е��ʵ���󳤶�
#else
const long long max_size = 2000;         // max length of strings
const long long closest_word_count = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
#endif

void print_word_vec(char* word_str, float* f_mat, long long size); //��ӡ�������ʵ�����ֵ

int main(int argc, char** argv) {
	FILE* word_vec_bin_file;		//word2vec����ѵ�������Ķ����������ļ���C:/code/word2vec/vectors.bin
	char input_str_line[max_size];	//�洢�û�������ַ��������ʻ���ӣ�
	char* best_word_str[closest_word_count];//�洢�����뵥����ӽ���ǰ N �����ʡ�
	char file_name[max_size];		//�洢�������ļ���·����
	char str_array[100][max_size];	//���û�������ַ�����ֳɵ������ʴ洢
	float dist;						//���ڼ���������������������֮����������ƶȡ�
	float len;						//�����ĳ��ȣ�ģ�������ڹ�һ����
	float best_dist[closest_word_count];//�洢�����뵥����ӽ���ǰ N �����ʵ��������ƶ�ֵ��
	float vec[max_size];			//�û����뵥���������ۻ�ֵ��
	long long words_count;			//�ļ��а����ĵ���������
	long long dim_size;				//ÿ������������ά�ȡ�
	long long a, b, c, d;			//ѭ��������
	long long input_word_count;		//����ĵ�����
	long long input_word_index[100];//���뵥���ڴʻ���е�����
	float* f_mat;					//�洢���д������ľ��󣨰��д洢��
	char* vocab;					//�洢�ʻ�������е��ʡ�
	if (argc < 2) {
		printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
		return 0;
	}
	strcpy_s(file_name, sizeof(file_name), argv[1]);
	errno_t err = fopen_s(&word_vec_bin_file, file_name, "rb");
	if (word_vec_bin_file == NULL || 0 != err) {
		printf("Input file not found\n");
		return -1;
	}
	fscanf_s(word_vec_bin_file, "%lld", &words_count);	//71291 (���ַ����洢��������һ���ո�0x20)
	fscanf_s(word_vec_bin_file, "%lld", &dim_size);		//200 (���ַ����洢��������һ��0x0A==\n==����)
	vocab = (char*)malloc((long long)words_count * max_word_len * sizeof(char));//���ʱ�ÿ�������50
	for (a = 0; a < closest_word_count; a++) best_word_str[a] = (char*)malloc(max_size * sizeof(char));//���ƶ���ߵĴ��������
	f_mat = (float*)malloc((long long)words_count * (long long)dim_size * sizeof(float));//ÿ��������200��32λ�������洢
	if (f_mat == NULL) {
		printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words_count * dim_size * sizeof(float) / 1048576, words_count, dim_size);
		return -1;
	}
	for (b = 0; b < words_count; b++) {//71291������ѭ��
		a = 0;
		while (1) {//��һ�����ʣ��Կո����
			vocab[b * max_word_len + a] = (char)fgetc(word_vec_bin_file);//��һ���ַ�
			if (feof(word_vec_bin_file) || (vocab[b * max_word_len + a] == ' ')) break;//�ո���ļ�����
			if ((a < max_word_len) && (vocab[b * max_word_len + a] != '\n')) a++;//���Ի��з�\n
		}
		vocab[b * max_word_len + a] = 0;//���ʽ�����
		for (a = 0; a < dim_size; a++) fread(&f_mat[a + b * dim_size], sizeof(float), 1, word_vec_bin_file);//��ȡһ�����ʵ�200��float,������һ��\n
		len = 0;
		//if (0 == b)print_word_vec(&vocab[b * max_w], &M[b * size], size);//�������
		for (a = 0; a < dim_size; a++) len += f_mat[a + b * dim_size] * f_mat[a + b * dim_size];//����ƽ���ͣ�����ģ��ƽ��
		len = (float)sqrt(len);//����ģ����
		if (len == 0.0f) {
			fprintf(stderr, "Warning: Vector length is zero for word: %s\n", &vocab[b * max_word_len]);
			continue; // ������ǰ���ʵĹ�һ��
		}
		for (a = 0; a < dim_size; a++) f_mat[a + b * dim_size] /= len;//��һ������,��һ�����ƽ���͵���1
		//if (0 == b)print_word_vec(&vocab[b * max_w], &M[b * size], size);//�������
	}
	fclose(word_vec_bin_file);
	while (1) {
		for (a = 0; a < closest_word_count; a++) best_dist[a] = 0;//��ʼ��
		for (a = 0; a < closest_word_count; a++) best_word_str[a][0] = 0;//��ʼ��
		printf("Enter word or sentence (EXIT to break): ");
		a = 0;
		while (1) {//��ȡ�û�����ĵ��ʻ���ӣ����� france
			input_str_line[a] = (char)fgetc(stdin);
			if ((input_str_line[a] == '\n') || (a >= max_size - 1)) {
				input_str_line[a] = 0;
				break;
			}
			a++;
		}
		if (!strcmp(input_str_line, "EXIT")) break;//�˳�����
		input_word_count = 0;
		b = 0;
		c = 0;
		while (1) {
			str_array[input_word_count][b] = input_str_line[c];
			b++;
			c++;
			str_array[input_word_count][b] = 0;
			if (input_str_line[c] == 0) break;
			if (input_str_line[c] == ' ') {//������Ǿ��ӣ���Ϊ������ʴ��
				input_word_count++;
				b = 0;
				c++;
			}
		}
		input_word_count++;
		for (a = 0; a < input_word_count; a++) {
			for (b = 0; b < words_count; b++) if (!strcmp(&vocab[b * max_word_len], str_array[a])) break;
			if (b == words_count) b = -1;//û�ҵ��û�����ĵ���
			input_word_index[a] = b;//�ҵ�ÿ�����ʵ���ţ����� france ��Ӧ vocab[303]
			printf("\nWord: %s  Position in vocabulary: %lld\n", str_array[a], input_word_index[a]);
			if (b == -1) {
				printf("Out of dictionary word!\n");
				break;
			}
		}
		if (b == -1) continue;
		printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
		for (a = 0; a < dim_size; a++) vec[a] = 0;//��ʼ��������뵥�ʶ�Ӧ�������ۼ�ֵ
		for (b = 0; b < input_word_count; b++) {
			if (input_word_index[b] == -1) continue;
			//�Ѷ�����뵥�ʵ�������ӣ��������Ϊ���ӵ�����
			for (a = 0; a < dim_size; a++) vec[a] += f_mat[a + input_word_index[b] * dim_size];
		}
		len = 0;
		for (a = 0; a < dim_size; a++) len += vec[a] * vec[a];//���ӵ�����ģƽ��
		len = (float)sqrt(len);//���ӵ�����ģ��
		for (a = 0; a < dim_size; a++) vec[a] /= len;//���ӵ�������һ��
		for (a = 0; a < closest_word_count; a++) best_dist[a] = -1;//��ʼ������
		for (a = 0; a < closest_word_count; a++) best_word_str[a][0] = 0;//��ʼ���ַ���
		for (c = 0; c < words_count; c++) {//�����е��ʼ������
			a = 0;
			for (b = 0; b < input_word_count; b++) if (input_word_index[b] == c) a = 1;
			if (a == 1) continue;//���������г��ֹ��ĵ���
			dist = 0;
			for (a = 0; a < dim_size; a++) dist += vec[a] * f_mat[a + c * dim_size];//��������뵥�ʵľ������
			for (a = 0; a < closest_word_count; a++) {
				if (dist > best_dist[a]) {//�����ҵ��ľ������ĵ���(distֵԽ����ض�Խ��)
					for (d = closest_word_count - 1; d > a; d--) {//�ƶ�����ĵ���
						best_dist[d] = best_dist[d - 1];
						strcpy_s(best_word_str[d], max_size, best_word_str[d - 1]);
					}
					best_dist[a] = dist;
					strcpy_s(best_word_str[a], max_size, &vocab[c * max_word_len]);
					break;
				}
			}
		}
		for (a = 0; a < closest_word_count; a++) printf("%50s\t\t%f\n", best_word_str[a], best_dist[a]);
	}
	return 0;
}

void print_word_vec(char* word_str, float* f_mat, long long size) {
	//��ӡһ�����ʵ�����������ֵ����100��������
	const int count_per_line = 10;//ÿ�еĸ�����
	const int line_count = (int)(size / count_per_line);//����������
	const int less_count = size % count_per_line;//���һ�е���Ŀ
	printf("%s\n", NULL == word_str ? "NULL" : word_str);
	for (int line_index = 0; line_index < line_count; ++line_index) {
		for (int data_index = 0; data_index < count_per_line; ++data_index) {
			printf("%10.6f ", f_mat[line_index * count_per_line + data_index]);
		}
		printf("\n");
	}
	if (less_count > 0) {
		for (int data_index = 0; data_index < less_count; ++data_index) {
			printf("%10.6f ", f_mat[line_count * count_per_line + data_index]);
		}
		printf("\n");
	}
}