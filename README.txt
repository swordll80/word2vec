Tools for computing distributed representtion of words (用于计算单词分布式表示的工具)
-------------------------------------------------------------------------------------

We provide an implementation of the Continuous Bag-of-Words (CBOW) and the Skip-gram model (SG), as well as several demo scripts.
我们提供连续词袋 (CBOW) 和 Skip-gram 模型 (SG) 的实现，以及几个演示脚本。

Given a text corpus, the word2vec tool learns a vector for every word in the vocabulary using the Continuous
Bag-of-Words or the Skip-Gram neural network architectures. The user should to specify the following:
给定一个文本语料库，word2vec 工具使用连续词袋或 Skip-Gram 神经网络架构为词汇表中的每个单词学习一个向量。用户应指定以下内容：
 - desired vector dimensionality. 所需的向量维数
 - the size of the context window for either the Skip-Gram or the Continuous Bag-of-Words model. Skip-Gram 或连续词袋模型的上下文窗口大小
 - training algorithm: hierarchical softmax and / or negative sampling. 训练算法：分层 softmax 和/或负采样
 - threshold for downsampling the frequent words .对频繁词进行下采样的阈值
 - number of threads to use. 要使用的线程数
 - the format of the output word vector file (text or binary). 输出词向量文件的格式（文本或二进制）

Usually, the other hyper-parameters such as the learning rate do not need to be tuned for different training sets. 
通常，其他超参数（例如学习率）不需要针对不同的训练集进行调整。

The script demo-word.sh downloads a small (100MB) text corpus from the web, and trains a small word vector model. After the training
is finished, the user can interactively explore the similarity of the words.
脚本 demo-word.sh 从网络下载一个小型（100MB）文本语料库，并训练一个小型词向量模型。训练完成后，用户可以交互式探索单词的相似性。

More information about the scripts is provided at https://code.google.com/p/word2vec/
有关脚本的更多信息，请访问 https://code.google.com/p/word2vec/

-------------------------------------------------------------------------------------
../demo-word.sh 文件中有测试数据的下载地址
http://mattmahoney.net/dc/text8.zip
解压后得到text8文件。
text8就放在当前目录，避免修改脚本。

-------------------------------------------------------------------------------------
参考demo-word.sh文件(别的文件用的不同测试数据，可以先不管)
./word2vec.exe -train text8 -output vectors.bin -cbow 1 -size 200 -window 8 -negative 25 -hs 0 -sample 1e-4 -threads 20 -binary 1 -iter 15
./distance.exe vectors.bin

./word2vec.exe命令运行时间较长。

-------------------------------------------------------------------------------------
./word-analogy.exe vectors.bin