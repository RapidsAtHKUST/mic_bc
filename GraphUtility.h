/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#ifndef GRAPHUTILITY_H_
#define GRAPHUTILITY_H_


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

#ifndef __MIC__
#include <boost/algorithm/string.hpp>
#include <boost/bimap.hpp>
#endif

#include "Graph.h"

class GraphUtility {
public:
	GraphUtility(Graph *g);
	void print_adjacency_list();
	void print_BC_scores(const std::vector<float> bc, char *outfile);
	void print_CSR();
	void print_R();
	void print_high_degree_vertices();
	void print_numerical_edge_file(char *outfile);
	void print_number_of_isolated_vertices();

	void parse(char *file);
	void parse_metis(char *file);
	void parse_edgelist(char *file);

	void verify(Graph g,const std::vector<float> bc_cpu, const std::vector<float> bc_mic);

	bool is_number(const std::string& s);
	bool is_alphanumeric(const std::string &s);

	Graph *g;

#ifndef __MIC__
	boost::bimap<unsigned, std::string> IDs; //Associate vertices with other data.
	//In general the unsigned could be replaced with a struct of attributes.
#endif
	virtual ~GraphUtility();
};

#endif /* GRAPHUTILITY_H_ */
