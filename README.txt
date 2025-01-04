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