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
#define closest_word_count  ((long long)40) // number of closest words that will be shown，输出时展示与输入单词余弦相似度最高的前 N 个单词
#define max_word_len  ((long long)50)		// max length of vocabulary entries，词汇表中单词的最大长度
#else
const long long max_size = 2000;         // max length of strings
const long long closest_word_count = 40;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
#endif

void print_word_vec(char* word_str, float* f_mat, long long size); //打印单个单词的向量值

int main(int argc, char** argv) {
	FILE* word_vec_bin_file;		//word2vec程序训练出来的二进制数据文件。C:/code/word2vec/vectors.bin
	char input_str_line[max_size];	//存储用户输入的字符串（单词或句子）
	char* best_word_str[closest_word_count];//存储与输入单词最接近的前 N 个单词。
	char file_name[max_size];		//存储词向量文件的路径。
	char str_array[100][max_size];	//将用户输入的字符串拆分成单个单词存储
	float dist;						//用于计算输入向量与其他向量之间的余弦相似度。
	float len;						//向量的长度（模），用于归一化。
	float best_dist[closest_word_count];//存储与输入单词最接近的前 N 个单词的余弦相似度值。
	float vec[max_size];			//用户输入单词向量的累积值。
	long long words_count;			//文件中包含的单词总数。
	long long dim_size;				//每个单词向量的维度。
	long long a, b, c, d;			//循环变量。
	long long input_word_count;		//输入的单词数
	long long input_word_index[100];//输入单词在词汇表中的索引
	float* f_mat;					//存储所有词向量的矩阵（按行存储）
	char* vocab;					//存储词汇表中所有单词。
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
	fscanf_s(word_vec_bin_file, "%lld", &words_count);	//71291 (用字符串存储，后面是一个空格0x20)
	fscanf_s(word_vec_bin_file, "%lld", &dim_size);		//200 (用字符串存储，后面是一个0x0A==\n==换行)
	vocab = (char*)malloc((long long)words_count * max_word_len * sizeof(char));//单词表，每个单词最长50
	for (a = 0; a < closest_word_count; a++) best_word_str[a] = (char*)malloc(max_size * sizeof(char));//相似度最高的词条或句子
	f_mat = (float*)malloc((long long)words_count * (long long)dim_size * sizeof(float));//每个单词用200个32位浮点数存储
	if (f_mat == NULL) {
		printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words_count * dim_size * sizeof(float) / 1048576, words_count, dim_size);
		return -1;
	}
	for (b = 0; b < words_count; b++) {//71291个单词循环
		a = 0;
		while (1) {//读一个单词，以空格结束
			vocab[b * max_word_len + a] = (char)fgetc(word_vec_bin_file);//读一个字符
			if (feof(word_vec_bin_file) || (vocab[b * max_word_len + a] == ' ')) break;//空格或文件结束
			if ((a < max_word_len) && (vocab[b * max_word_len + a] != '\n')) a++;//忽略换行符\n
		}
		vocab[b * max_word_len + a] = 0;//单词结束符
		for (a = 0; a < dim_size; a++) fread(&f_mat[a + b * dim_size], sizeof(float), 1, word_vec_bin_file);//读取一个单词的200个float,后面有一个\n
		len = 0;
		//if (0 == b)print_word_vec(&vocab[b * max_w], &M[b * size], size);//输出测试
		for (a = 0; a < dim_size; a++) len += f_mat[a + b * dim_size] * f_mat[a + b * dim_size];//向量平方和，向量模的平方
		len = (float)sqrt(len);//向量模长度
		if (len == 0.0f) {
			fprintf(stderr, "Warning: Vector length is zero for word: %s\n", &vocab[b * max_word_len]);
			continue; // 跳过当前单词的归一化
		}
		for (a = 0; a < dim_size; a++) f_mat[a + b * dim_size] /= len;//归一化向量,归一化后的平方和等于1
		//if (0 == b)print_word_vec(&vocab[b * max_w], &M[b * size], size);//输出测试
	}
	fclose(word_vec_bin_file);
	while (1) {
		for (a = 0; a < closest_word_count; a++) best_dist[a] = 0;//初始化
		for (a = 0; a < closest_word_count; a++) best_word_str[a][0] = 0;//初始化
		printf("Enter word or sentence (EXIT to break): ");
		a = 0;
		while (1) {//读取用户输入的单词或句子，例如 france
			input_str_line[a] = (char)fgetc(stdin);
			if ((input_str_line[a] == '\n') || (a >= max_size - 1)) {
				input_str_line[a] = 0;
				break;
			}
			a++;
		}
		if (!strcmp(input_str_line, "EXIT")) break;//退出程序
		input_word_count = 0;
		b = 0;
		c = 0;
		while (1) {
			str_array[input_word_count][b] = input_str_line[c];
			b++;
			c++;
			str_array[input_word_count][b] = 0;
			if (input_str_line[c] == 0) break;
			if (input_str_line[c] == ' ') {//输入的是句子，拆为多个单词存放
				input_word_count++;
				b = 0;
				c++;
			}
		}
		input_word_count++;
		for (a = 0; a < input_word_count; a++) {
			for (b = 0; b < words_count; b++) if (!strcmp(&vocab[b * max_word_len], str_array[a])) break;
			if (b == words_count) b = -1;//没找到用户输入的单词
			input_word_index[a] = b;//找到每个单词的序号，例如 france 对应 vocab[303]
			printf("\nWord: %s  Position in vocabulary: %lld\n", str_array[a], input_word_index[a]);
			if (b == -1) {
				printf("Out of dictionary word!\n");
				break;
			}
		}
		if (b == -1) continue;
		printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
		for (a = 0; a < dim_size; a++) vec[a] = 0;//初始化多个输入单词对应的向量累计值
		for (b = 0; b < input_word_count; b++) {
			if (input_word_index[b] == -1) continue;
			//把多个输入单词的向量相加，可以理解为句子的向量
			for (a = 0; a < dim_size; a++) vec[a] += f_mat[a + input_word_index[b] * dim_size];
		}
		len = 0;
		for (a = 0; a < dim_size; a++) len += vec[a] * vec[a];//句子的向量模平方
		len = (float)sqrt(len);//句子的向量模长
		for (a = 0; a < dim_size; a++) vec[a] /= len;//句子的向量归一化
		for (a = 0; a < closest_word_count; a++) best_dist[a] = -1;//初始化距离
		for (a = 0; a < closest_word_count; a++) best_word_str[a][0] = 0;//初始化字符串
		for (c = 0; c < words_count; c++) {//与所有单词计算距离
			a = 0;
			for (b = 0; b < input_word_count; b++) if (input_word_index[b] == c) a = 1;
			if (a == 1) continue;//跳过句子中出现过的单词
			dist = 0;
			for (a = 0; a < dim_size; a++) dist += vec[a] * f_mat[a + c * dim_size];//输入句子与单词的距离计算
			for (a = 0; a < closest_word_count; a++) {
				if (dist > best_dist[a]) {//插入找到的距离更大的单词(dist值越大，相关度越大)
					for (d = closest_word_count - 1; d > a; d--) {//移动后面的单词
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
	//打印一个单词的向量，典型值就是100个浮点数
	const int count_per_line = 10;//每行的浮点数
	const int line_count = (int)(size / count_per_line);//完整的行数
	const int less_count = size % count_per_line;//最后一行的数目
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