/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "GraphUtility.h"

GraphUtiliy::GraphUtiliy() {
	// TODO Auto-generated constructor stub
	R = C = F = nullptr;
	n = m = -1;
}

void GraphUtiliy::print_adjacency_list() {
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

void GraphUtiliy::print_BC_scores(const std::vector<float> bc, char* outfile) {
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

void GraphUtiliy::print_CSR() {
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

void GraphUtiliy::print_R() {
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

void GraphUtiliy::print_high_degree_vertices() {
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

void GraphUtiliy::print_numerical_edge_file(char* outfile) {
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

void GraphUtiliy::print_number_of_isolated_vertices() {
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

void GraphUtiliy::parse(char* file) {
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

void GraphUtiliy::parse_metis(char* file) {
	std::ifstream metis(file, std::ifstream::in);
	std::string line;
	bool firstline = true;
	int current_node = 0;
	int current_edge = 0;

	if (!metis.good()) {
		std::cerr << "Error opening graph file." << std::endl;
		exit(-1);
	}

	while (std::getline(metis, line)) {
		if (line[0] == '%') {
			continue;
		}

		std::vector<std::string> splitvec;
		boost::split(splitvec, line, boost::is_any_of(" \t"),
				boost::token_compress_on); //Now tokenize

		//If the first or last element is a space or tab itself, erase it
		if (!splitvec.empty() && !is_number(splitvec[0])) {
			splitvec.erase(splitvec.begin());
		}
		 if(!splitvec.empty() && !is_number(splitvec[splitvec.size()-1])) {
			splitvec.erase(splitvec.end() - 1);
		}

		if (firstline) {
			this->n = std::stoi(splitvec[0]);
			this->m = std::stoi(splitvec[1]);
			if (splitvec.size() > 3) {
				std::cerr << "Error: Weighted graphs are not yet supported."
						<< std::endl;
				exit(-2);
			} else if ((splitvec.size() == 3) && (std::stoi(splitvec[2]) != 0)) {
				std::cerr << "Error: Weighted graphs are not yet supported."
						<< std::endl;
				exit(-2);
			}
			firstline = false;
			this->R = new int[this->n + 1];
			this->F = new int[2 * this->m];
			this->C = new int[2 * this->m];
			this->R[0] = 0;
			current_node++;
		} else {
			//Count the number of edges that this vertex has and add that to the most recent value in R
			this->R[current_node] = splitvec.size() + this->R[current_node - 1];
			for (unsigned i = 0; i < splitvec.size(); i++) {
				//coPapersDBLP uses a space to mark the beginning of each line, so we'll account for that here
				if (!is_number(splitvec[i])) {
					//Need to adjust this->R
					this->R[current_node]--;
					continue;
				}
				//Store the neighbors in C
				//METIS graphs are indexed by one, but for our convenience we represent them as if
				//they were zero-indexed
				this->C[current_edge] = std::stoi(splitvec[i]) - 1;
				this->F[current_edge] = current_node - 1;
				current_edge++;
			}
			current_node++;
		}
	}
}

void GraphUtiliy::parse_edgelist(char* file) {
	std::set<std::string> vertices;

	//Scan the file
	std::ifstream edgelist(file, std::ifstream::in);
	std::string line;

	if (!edgelist.good()) {
		std::cerr << "Error opening graph file." << std::endl;
		exit(-1);
	}

	std::vector<std::string> from;
	std::vector<std::string> to;
	while (std::getline(edgelist, line)) {
		if ((line[0] == '%') || (line[0] == '#')) //Allow comments
				{
			continue;
		}

		std::vector<std::string> splitvec;
		boost::split(splitvec, line, boost::is_any_of(" \t"),
				boost::token_compress_on);

		if (splitvec.size() != 2) {
			std::cerr
					<< "Warning: Found a row that does not represent an edge or comment."
					<< std::endl;
			std::cerr << "Row in question: " << std::endl;
			for (unsigned i = 0; i < splitvec.size(); i++) {
				std::cout << splitvec[i] << std::endl;
			}
			exit(-1);
		}

		for (unsigned i = 0; i < splitvec.size(); i++) {
			vertices.insert(splitvec[i]);
		}

		from.push_back(splitvec[0]);
		to.push_back(splitvec[1]);
	}

	edgelist.close();

	this->n = vertices.size();
	this->m = from.size();

	unsigned id = 0;
	for (std::set<std::string>::iterator i = vertices.begin(), e =
			vertices.end(); i != e; ++i) {
		this->IDs.insert(boost::bimap<unsigned, std::string>::value_type(id++, *i));
	}

	this->R = new int[this->n + 1];
	this->F = new int[2 * this->m];
	this->C = new int[2 * this->m];

	for (int i = 0; i < this->m; i++) {
		boost::bimap<unsigned, std::string>::right_map::iterator itf =
				this->IDs.right.find(from[i]);
		boost::bimap<unsigned, std::string>::right_map::iterator itc =
				this->IDs.right.find(to[i]);

		if ((itf == this->IDs.right.end()) || (itc == this->IDs.right.end())) {
			std::cerr << "Error parsing graph file." << std::endl;
			exit(-1);
		} else {
			if (itf->second == itc->second) {
				std::cerr << "Error: self edge! " << itf->second << " -> "
						<< itc->second << std::endl;
				std::cerr
						<< "Aborting. Graphs with self-edges aren't supported."
						<< std::endl;
				exit(-1);
			}
			this->F[2 * i] = itf->second;
			this->C[2 * i] = itc->second;
			//Treat undirected edges as two directed edges
			this->F[(2 * i) + 1] = itc->second;
			this->C[(2 * i) + 1] = itf->second;
		}
	}

	//Sort edges by F
	std::vector<std::pair<int, int> > edges;
	for (int i = 0; i < 2 * this->m; i++) {
		edges.push_back(std::make_pair(this->F[i], this->C[i]));
	}
	std::sort(edges.begin(), edges.end()); //By default, pair sorts with precedence to it's first member, which is precisely what we want.
	this->R[0] = 0;
	int last_node = 0;
	for (int i = 0; i < 2 * this->m; i++) {
		this->F[i] = edges[i].first;
		this->C[i] = edges[i].second;
		while (edges[i].first > last_node) {
			this->R[++last_node] = i;
		}
	}
	this->R[this->n] = 2 * this->m;
	edges.clear();
}

bool GraphUtiliy::is_number(const std::string& s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it))
		++it;
	return !s.empty() && it == s.end();
}

bool GraphUtiliy::is_alphanumeric(const std::string& s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && (std::isalnum(*it)))
		++it;
	return !s.empty() && it == s.end();
}

GraphUtiliy::~GraphUtiliy() {
	// TODO Auto-generated destructor stub
}

