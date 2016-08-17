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

    /*
     * see Sariyüce, A. E., Kaya, K., Saule, E., & Çatalyürek, Ü. V. (2013). Betweenness centrality on GPUs and
     * heterogeneous architectures. In Proceedings of the 6th Workshop on General Purpose Processor Using Graphics
     * Processing Units - GPGPU-6 (pp. 76–85). New York, New York, USA: ACM Press.
     * http://doi.org/10.1145/2458523.2458531
     */
    int *weight;
    int *which_components;
    int *components_sizes;
    int total_comp;
    int *bc;

    virtual ~Graph();
};

#endif /* GRAPH_H_ */
