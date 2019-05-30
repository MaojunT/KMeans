#include<iostream>
#include<fstream>
#include<vector>
#include<cstdlib>
#include<time.h>

using namespace std;

class KMEANS
{
private:
	vector<vector<int> > cluster;     //uses a 2D vector to represent the cluster
	vector<vector<int> > centroids;   //uses a 2D vector to represent the corresponding centroid
	int k;
	int col;
	int row;                         //size of the column and row of cluster
	typedef struct MinMax
	{
		int Min;
        int Max;
		MinMax(int min, int max): Min(min), Max(max) {}     //constructor
	}Min_Max;
	typedef struct point
	{
		int minIndex;
		double minDist;
		point(int idx, double dist): minIndex(idx), minDist(dist) {}       //constructor
	}pt;
	vector<pt> assig_cluster;

public:
	KMEANS(int k);
	void loadFile();
	void selectCentroids();
    Min_Max getMinMax(int idx);
	void setCentroids(Min_Max &min_max, int idx);
	double euclideanDistance(vector<int> &v1, vector<int> &v2);
	void print();
	void kmeans();
};

/*
    Constructor
*/
KMEANS::KMEANS(int k)
{
	this -> k = k;
}

/*
    initialize the cluster
*/
void KMEANS::loadFile()
{
    ifstream fin;
    fin.open("input.txt");

    int input1, input2;
    vector<int> temp;
    while (fin >> input1 >> input2)
    {
        temp.push_back(input1);
        temp.push_back(input2);
        cluster.push_back(temp);
        temp.clear();
    }

    fin.close();

	row = cluster.size();
	col = cluster[0].size();
}

/*
    set the centroids to each cluster
*/
void KMEANS::setCentroids(Min_Max &min_max, int idx)
{
	int range = min_max.Max - min_max.Min;
	for(int i = 0; i < k; i++)
		centroids[i].at(idx) = min_max.Min + range *  (rand() / (double)RAND_MAX);
}

/*
    select k random centroids
*/
void KMEANS::selectCentroids()
{
	vector<int> temp(col, 0);
	for(int i = 0; i < k; i++)
		centroids.push_back(temp);

	srand(time(NULL));
	for(int j = 0; j < col; j++)
	{
	    Min_Max min_max = getMinMax(j);
		setCentroids(min_max,j);
	}
}

/*
    return min and max distance as a tMinMax struct
*/
typename KMEANS::Min_Max KMEANS::getMinMax(int idx)
{
    int min , max ;
	cluster[0].at(idx) > cluster[1].at(idx) ? (max = cluster[0].at(idx),min = cluster[1].at(idx)) : (max = cluster[1].at(idx),min = cluster[0].at(idx)) ;

	for(int i = 2; i< row; i++)
	{
		if(cluster[i].at(idx) < min)
            min = cluster[i].at(idx);
		else if(cluster[i].at(idx) > max)
            max = cluster[i].at(idx);
		else continue;
	}

	Min_Max min_max(min, max);
	return min_max;
}

/*
    return Euclidean distance
*/
double KMEANS::euclideanDistance(vector<int> &v1 , vector<int> &v2)
{
	int dis = 0;
	int size = v1.size();
	for(int i = 0; i<size; i++)
	{
		dis += (v1[i] - v2[i])*(v1[i] - v2[i]);
	}
	return dis;
}

/*
    print to file
*/
void KMEANS::print()
{
	ofstream fout;
	fout.open("output.txt");

	typename vector<vector<int> >::iterator it = cluster.begin();
	typename vector<pt>::iterator itt = assig_cluster.begin();
	for(int i = 0; i < row; i++)
	{
		typename vector<int>::iterator it2 = (*it).begin();
		while(it2 != (*it).end())
		{
			fout << *it2 << "\t";
			it2++;
		}
		fout << ((*itt).minIndex + 1) << endl;
		itt++;
		it++;
	}
}

/*
    process cluster by kmeans
*/
void KMEANS::kmeans()
{
    pt point(-1, -1);

	for(int i = 0; i < row; i++)
		assig_cluster.push_back(point);

	bool clusterChanged = true;
	while(clusterChanged)      //while there is a change in cluster
	{
		clusterChanged = false;

		cout << "-----Finding nearest centroid for each point-----" << endl;
		for(int i = 0; i < row; i++)
		{
			int minIndex = -1;
			double minDist = INT_MAX;
			for(int j = 0; j < k; j++)
			{
				double dist = euclideanDistance(centroids[j], cluster[i]);
				if(dist < minDist)
				{
					minDist = dist;
					minIndex = j;
				}
			}
			if(assig_cluster[i].minIndex != minIndex)   //if the cluster moves
			{
				clusterChanged = true;
				assig_cluster[i].minIndex = minIndex;
				assig_cluster[i].minDist = minDist;
			}
		}

		cout << "-----Updating centroids-----" << endl;
		for(int cent = 0; cent < k; cent++)
		{
			vector<int> temp(col,0);
			int cnt = 0;
			for(int i = 0; i < row; i++)
			{
				if(assig_cluster[i].minIndex == cent)
				{
					cnt++;
					for(int j = 0; j < col; j++)
						temp[j] += cluster[i].at(j);
				}
			}

			for(int i = 0; i < col; i++)
			{
				if(cnt != 0)
                    temp[i] /= cnt;

				centroids[cent].at(i) = temp[i];
			}
		}
		print();
	}
}

int main( int argc , char *argv[])
{
	int k = atoi(argv[1]);
	KMEANS kms(k);
	kms.loadFile();
	kms.selectCentroids();
	kms.kmeans();

	return 0;
}
