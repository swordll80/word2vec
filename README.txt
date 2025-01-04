Tools for computing distributed representtion of words (���ڼ��㵥�ʷֲ�ʽ��ʾ�Ĺ���)
-------------------------------------------------------------------------------------

We provide an implementation of the Continuous Bag-of-Words (CBOW) and the Skip-gram model (SG), as well as several demo scripts.
�����ṩ�����ʴ� (CBOW) �� Skip-gram ģ�� (SG) ��ʵ�֣��Լ�������ʾ�ű���

Given a text corpus, the word2vec tool learns a vector for every word in the vocabulary using the Continuous
Bag-of-Words or the Skip-Gram neural network architectures. The user should to specify the following:
����һ���ı����Ͽ⣬word2vec ����ʹ�������ʴ��� Skip-Gram ������ܹ�Ϊ�ʻ���е�ÿ������ѧϰһ���������û�Ӧָ���������ݣ�
 - desired vector dimensionality. ���������ά��
 - the size of the context window for either the Skip-Gram or the Continuous Bag-of-Words model. Skip-Gram �������ʴ�ģ�͵������Ĵ��ڴ�С
 - training algorithm: hierarchical softmax and / or negative sampling. ѵ���㷨���ֲ� softmax ��/�򸺲���
 - threshold for downsampling the frequent words .��Ƶ���ʽ����²�������ֵ
 - number of threads to use. Ҫʹ�õ��߳���
 - the format of the output word vector file (text or binary). ����������ļ��ĸ�ʽ���ı�������ƣ�

Usually, the other hyper-parameters such as the learning rate do not need to be tuned for different training sets. 
ͨ��������������������ѧϰ�ʣ�����Ҫ��Բ�ͬ��ѵ�������е�����

The script demo-word.sh downloads a small (100MB) text corpus from the web, and trains a small word vector model. After the training
is finished, the user can interactively explore the similarity of the words.
�ű� demo-word.sh ����������һ��С�ͣ�100MB���ı����Ͽ⣬��ѵ��һ��С�ʹ�����ģ�͡�ѵ����ɺ��û����Խ���ʽ̽�����ʵ������ԡ�

More information about the scripts is provided at https://code.google.com/p/word2vec/
�йؽű��ĸ�����Ϣ������� https://code.google.com/p/word2vec/

-------------------------------------------------------------------------------------
win11��vs2022��.sln�����ļ������ɣ�
mkdir build
cd build
cmake ..

-------------------------------------------------------------------------------------
../demo-word.sh �ļ����в������ݵ����ص�ַ
http://mattmahoney.net/dc/text8.zip
��ѹ��õ�text8�ļ���
text8�ͷ��ڵ�ǰĿ¼�������޸Ľű���

-------------------------------------------------------------------------------------
�ο�demo-word.sh�ļ�(����ļ��õĲ�ͬ�������ݣ������Ȳ���)
./word2vec.exe -train text8 -output vectors.bin -cbow 1 -size 200 -window 8 -negative 25 -hs 0 -sample 1e-4 -threads 20 -binary 1 -iter 15
./distance.exe vectors.bin

./word2vec.exe��������ʱ��ϳ���

-------------------------------------------------------------------------------------
./word-analogy.exe vectors.bin



-------------------------------------------------------------------------------------
./word2vec.exe -train text8 -output vectors.bin
               -cbow 1      ʹ�������ʴ�ģ��cbow��Ĭ��ֵΪ 1������ skip-gram ģ��ʹ�� 0��
               -size 200    ��������С(ά��)
               -window 8    ��֮��������������
               -negative 25 ����ʾ��������
               -hs 0        ʹ�÷ֲ� Softmax��Ĭ��ֵΪ 0��δʹ�ã�
               -sample 1e-4 �ʳ��ֵ���ֵ
               -threads 20 
               -binary 1    �Զ�����ģʽ������������Ĭ��ֵΪ 0���رգ�
               -iter 15     ѵ������


PS C:\code\word2vec> ./word2vec.exe
WORD VECTOR estimation toolkit v 0.1c

Options:
Parameters for training:
        -train <file>
                Use text data from <file> to train the model
        -output <file>
                Use <file> to save the resulting word vectors / word clusters
        -size <int>
                Set size of word vectors; default is 100
        -window <int>
                Set max skip length between words; default is 5
        -sample <float>
                Set threshold for occurrence of words. Those that appear with higher frequency in the training data
                will be randomly down-sampled; default is 1e-3, useful range is (0, 1e-5)
        -hs <int>
                Use Hierarchical Softmax; default is 0 (not used)
        -negative <int>
                Number of negative examples; default is 5, common values are 3 - 10 (0 = not used)
        -threads <int>
                Use <int> threads (default 12)
        -iter <int>
                Run more training iterations (default 5)
        -min-count <int>
                This will discard words that appear less than <int> times; default is 5
        -alpha <float>
                Set the starting learning rate; default is 0.025 for skip-gram and 0.05 for CBOW
        -classes <int>
                Output word classes rather than word vectors; default number of classes is 0 (vectors are written)
        -debug <int>
                Set the debug mode (default = 2 = more info during training)
        -binary <int>
                Save the resulting vectors in binary moded; default is 0 (off)
        -save-vocab <file>
                The vocabulary will be saved to <file>
        -read-vocab <file>
                The vocabulary will be read from <file>, not constructed from the training data
        -cbow <int>
                Use the continuous bag of words model; default is 1 (use 0 for skip-gram model)

Examples:
./word2vec -train data.txt -output vec.txt -size 200 -window 5 -sample 1e-4 -negative 5 -hs 0 -binary 0 -cbow 1 -iter 3
-------------------------------------------------------------------------------------












