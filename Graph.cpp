/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "Graph.h"

Graph::Graph() {
	// TODO Auto-generated constructor stub
	R = C = F = nullptr;
	n = m = -1;
}

void Graph::print_adjacency_list() {
	if (R == NULL) {
		std::cerr
				<< "Error: Attempt to print adjacency list of graph that has not been parsed."
				<< std::endl;
		exit(-1);
	}

	std::cout << "Edge lists for each vertex: " << std::endl;

	for (int i = 0; i < n; i++) {
		int begin = R[i];
		int end = R[i + 1];
		boost::bimap<unsigned, std::string>::left_map::iterator itr =
				IDs.left.find(i);
		for (int j = begin; j < end; j++) {
			boost::bimap<unsigned, std::string>::left_map::iterator itc =
					IDs.left.find(C[j]);
			if (j == begin) {
				std::cout << itr->second << " | " << itc->second;
			} else {
				std::cout << ", " << itc->second;
			}
		}
		if (begin == end) //Single, unconnected node
				{
			std::cout << itr->second << " | ";
		}
		std::cout << std::endl;
	}
}

void Graph::print_BC_scores(const std::vector<float> bc, char* outfile) {
	std::ofstream ofs;

	std::ostream &os = (outfile ? ofs : std::cout);
	for (int i = 0; i < n; i++) {
		boost::bimap<unsigned, std::string>::left_map::iterator it =
				IDs.left.find(i);
		if (it != IDs.left.end()) {
			os << it->second << " " << bc[i] << std::endl;
		} else {
			//Just print the numeric id
			os << i << " " << bc[i] << std::endl;
		}
	}
}

void Graph::print_CSR() {
	if ((R == NULL) || (C == NULL) || (F == NULL)) {
		std::cerr
				<< "Error: Attempt to print CSR of a graph that has not been parsed."
				<< std::endl;
		exit(-1);
	}

	std::cout << "R = [";
	for (int i = 0; i < (n + 1); i++) {
		if (i == n) {
			std::cout << R[i] << "]" << std::endl;
		} else {
			std::cout << R[i] << ",";
		}
	}

	std::cout << "C = [";
	for (int i = 0; i < (2 * m); i++) {
		if (i == ((2 * m) - 1)) {
			std::cout << C[i] << "]" << std::endl;
		} else {
			std::cout << C[i] << ",";
		}
	}

	std::cout << "F = [";
	for (int i = 0; i < (2 * m); i++) {
		if (i == ((2 * m) - 1)) {
			std::cout << F[i] << "]" << std::endl;
		} else {
			std::cout << F[i] << ",";
		}
	}
}

void Graph::print_R() {
	if (R == NULL) {
		std::cerr
				<< "Error: Attempt to print CSR of a graph that has not been parsed."
				<< std::endl;
	}

	std::cout << "R = [";
	for (int i = 0; i < (n + 1); i++) {
		if (i == n) {
			std::cout << R[i] << "]" << std::endl;
		} else {
			std::cout << R[i] << ",";
		}
	}
}

void Graph::print_high_degree_vertices() {
	if (R == NULL) {
		std::cerr
				<< "Error: Attempt to search adjacency list of graph that has not been parsed."
				<< std::endl;
		exit(-1);
	}

	int max_degree = 0;
	for (int i = 0; i < n; i++) {
		int degree = R[i + 1] - R[i];
		if (degree > max_degree) {
			max_degree = degree;
			std::cout << "Max degree: " << degree << std::endl;
		}
	}
}

void Graph::print_numerical_edge_file(char* outfile) {
	std::ofstream ofs(outfile, std::ios::out);
	if (!ofs.good()) {
		std::cerr << "Error opening output file." << std::endl;
		exit(-1);
	}
	for (int i = 0; i < 2 * m; i++) {
		if (F[i] < C[i]) {
			ofs << F[i] << " " << C[i] << std::endl;
		}
	}
}

void Graph::print_number_of_isolated_vertices() {
	if (R == NULL) {
		std::cerr
				<< "Error: Attempt to print CSR of a graph that has not been parsed."
				<< std::endl;
	}

	int isolated = 0;
	for (int i = 0; i < n; i++) {
		int degree = R[i + 1] - R[i];
		if (degree == 0) {
			isolated++;
		}
	}

	std::cout << "Number of isolated vertices: " << isolated << std::endl;
}

void Graph::parse(char* file) {
	std::string s(file);

	if (s.find(".graph") != std::string::npos) {
		 parse_metis(file);
	} else if (s.find(".txt") != std::string::npos) {
		 parse_edgelist(file);
	} else if (s.find(".edge") != std::string::npos) {
		 parse_edgelist(file);
	} else {
		std::cerr << "Error: Unsupported file type." << std::endl;
		exit(-1);
	}
}

void Graph::parse_metis(char* file) {
}

void Graph::parse_edgelist(char* file) {
}

Graph::~Graph() {
	// TODO Auto-generated destructor stub
}

