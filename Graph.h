/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef GRAPH_H_
#define GRAPH_H_

class Graph {
public:
	Graph();


	int *R;
	int *C;
	int *F;
	int n; //Number of vertices
	int m; //Number of edges

	virtual ~Graph();
};

#endif /* GRAPH_H_ */
