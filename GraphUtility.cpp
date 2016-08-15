/*
 *
 *  Author : Lipeng Wang
 *  E-mail : wang.lp@outlook.com
 *  Github : https://github.com/WANG-lp
 * 
 */

#include "GraphUtility.h"

GraphUtility::GraphUtility(Graph *g) {
    this->g = g;
}

void GraphUtility::print_adjacency_list() {
    if (g->R == NULL) {
        std::cerr
                << "Error: Attempt to print adjacency list of graph that has not been parsed."
                << std::endl;
        exit(-1);
    }

    std::cout << "Edge lists for each vertex: " << std::endl;

    for (int i = 0; i < g->n; i++) {
        int begin = g->R[i];
        int end = g->R[i + 1];
        boost::bimap<unsigned, std::string>::left_map::iterator itr =
                IDs.left.find(i);
        for (int j = begin; j < end; j++) {
            boost::bimap<unsigned, std::string>::left_map::iterator itc =
                    IDs.left.find(g->C[j]);
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

void GraphUtility::print_BC_scores(std::vector<float> bc, char *outfile) {
    std::ofstream ofs;
    if (outfile != NULL) {
        ofs.open(outfile, std::ios::out);
    }
    std::ostream &os = (outfile ? ofs : std::cout);
    for (int i = 0; i < g->n; i++) {
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

void GraphUtility::print_CSR() {
    if ((g->R == NULL) || (g->C == NULL) || (g->F == NULL)) {
        std::cerr
                << "Error: Attempt to print CSR of a graph that has not been parsed."
                << std::endl;
        exit(-1);
    }

    std::cout << "R = [";
    for (int i = 0; i < (g->n + 1); i++) {
        if (i == g->n) {
            std::cout << g->R[i] << "]" << std::endl;
        } else {
            std::cout << g->R[i] << ",";
        }
    }

    std::cout << "C = [";
    for (int i = 0; i < (2 * g->m); i++) {
        if (i == ((2 * g->m) - 1)) {
            std::cout << g->C[i] << "]" << std::endl;
        } else {
            std::cout << g->C[i] << ",";
        }
    }

    std::cout << "F = [";
    for (int i = 0; i < (2 * g->m); i++) {
        if (i == ((2 * g->m) - 1)) {
            std::cout << g->F[i] << "]" << std::endl;
        } else {
            std::cout << g->F[i] << ",";
        }
    }
}

void GraphUtility::print_R() {
    if (g->R == NULL) {
        std::cerr
                << "Error: Attempt to print CSR of a graph that has not been parsed."
                << std::endl;
    }

    std::cout << "R = [";
    for (int i = 0; i < (g->n + 1); i++) {
        if (i == g->n) {
            std::cout << g->R[i] << "]" << std::endl;
        } else {
            std::cout << g->R[i] << ",";
        }
    }
}

void GraphUtility::print_high_degree_vertices() {
    if (g->R == NULL) {
        std::cerr
                << "Error: Attempt to search adjacency list of graph that has not been parsed."
                << std::endl;
        exit(-1);
    }

    int max_degree = 0;
    for (int i = 0; i < g->n; i++) {
        int degree = g->R[i + 1] - g->R[i];
        if (degree > max_degree) {
            max_degree = degree;
            std::cout << "Max degree: " << degree << std::endl;
        }
    }
}

void GraphUtility::print_numerical_edge_file(char *outfile) {
    std::ofstream ofs(outfile, std::ios::out);
    if (!ofs.good()) {
        std::cerr << "Error opening output file." << std::endl;
        exit(-1);
    }
    for (int i = 0; i < 2 * g->m; i++) {
        if (g->F[i] < g->C[i]) {
            ofs << g->F[i] << " " << g->C[i] << std::endl;
        }
    }
}

void GraphUtility::print_number_of_isolated_vertices() {
    if (g->R == NULL) {
        std::cerr
                << "Error: Attempt to print CSR of a graph that has not been parsed."
                << std::endl;
    }

    int isolated = 0;
    for (int i = 0; i < g->n; i++) {
        int degree = g->R[i + 1] - g->R[i];
        if (degree == 0) {
            isolated++;
        }
    }

    std::cout << "Number of isolated vertices: " << isolated << std::endl;
}

void GraphUtility::parse(char *file) {
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

void GraphUtility::parse_metis(char *file) {
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
        if (line[0] == '%' || line[0] == '#') {
            continue;
        }

        std::vector<std::string> splitvec;
        boost::split(splitvec, line, boost::is_any_of(" \t"),
                     boost::token_compress_on); //Now tokenize

        //If the first or last element is a space or tab itself, erase it
        if (!splitvec.empty() && !is_number(splitvec[0])) {
            splitvec.erase(splitvec.begin());
        }
        if (!splitvec.empty() && !is_number(splitvec[splitvec.size() - 1])) {
            splitvec.erase(splitvec.end() - 1);
        }

        if (firstline) {
            g->n = std::stoi(splitvec[0]);
            g->m = std::stoi(splitvec[1]);
            if (splitvec.size() > 3) {
                std::cerr << "Error: Weighted graphs are not yet supported."
                          << std::endl;
                exit(-2);
            } else if ((splitvec.size() == 3)
                       && (std::stoi(splitvec[2]) != 0)) {
                std::cerr << "Error: Weighted graphs are not yet supported."
                          << std::endl;
                exit(-2);
            }
            firstline = false;
            g->R = new int[g->n + 1];
            g->F = new int[2 * g->m];
            g->C = new int[2 * g->m];
            g->R[0] = 0;
            current_node++;
        } else {
            //Count the number of edges that this vertex has and add that to the most recent value in R
            g->R[current_node] = splitvec.size() + g->R[current_node - 1];
            for (unsigned i = 0; i < splitvec.size(); i++) {
                //coPapersDBLP uses a space to mark the beginning of each line, so we'll account for that here
                if (!is_number(splitvec[i])) {
                    //Need to adjust this->R
                    g->R[current_node]--;
                    continue;
                }
                //Store the neighbors in C
                //METIS graphs are indexed by one, but for our convenience we represent them as if
                //they were zero-indexed
                g->C[current_edge] = std::stoi(splitvec[i]) - 1;
                g->F[current_edge] = current_node - 1;
                current_edge++;
            }
            current_node++;
        }
    }
}

void GraphUtility::parse_edgelist(char *file) {
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

    g->n = vertices.size();
    g->m = from.size();

    unsigned id = 0;
    for (std::set<std::string>::iterator i = vertices.begin(), e =
            vertices.end(); i != e; ++i) {
        this->IDs.insert(
                boost::bimap<unsigned, std::string>::value_type(id++, *i));
    }

    g->R = new int[g->n + 1];
    g->F = new int[2 * g->m];
    g->C = new int[2 * g->m];

    for (int i = 0; i < g->m; i++) {
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
            g->F[2 * i] = itf->second;
            g->C[2 * i] = itc->second;
            //Treat undirected edges as two directed edges
            g->F[(2 * i) + 1] = itc->second;
            g->C[(2 * i) + 1] = itf->second;
        }
    }

    //Sort edges by F
    std::vector<std::pair<int, int> > edges;
    for (int i = 0; i < 2 * g->m; i++) {
        edges.push_back(std::make_pair(g->F[i], g->C[i]));
    }
    std::sort(edges.begin(),
              edges.end()); //By default, pair sorts with precedence to it's first member, which is precisely what we want.
    g->R[0] = 0;
    int last_node = 0;
    for (int i = 0; i < 2 * g->m; i++) {
        g->F[i] = edges[i].first;
        g->C[i] = edges[i].second;
        while (edges[i].first > last_node) {
            g->R[++last_node] = i;
        }
    }
    g->R[g->n] = 2 * g->m;
    edges.clear();
}

bool GraphUtility::is_number(const std::string &s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

bool GraphUtility::is_alphanumeric(const std::string &s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (std::isalnum(*it)))
        ++it;
    return !s.empty() && it == s.end();
}

void GraphUtility::verify(Graph g, const std::vector<float> bc_cpu,
                          const std::vector<float> bc_gpu) {
    double error = 0;
    double max_error = 0;
    for (int i = 0; i < g.n; i++) {
        double current_error = std::abs(bc_cpu[i] - bc_gpu[i]);
        error += current_error * current_error;
        if (current_error > max_error) {
            max_error = current_error;
        }
    }
    error = error / (float) g.n;
    error = std::sqrt(error);
    std::cout << "\tRMS Error: " << error << std::endl;
    std::cout << "\tMaximum error: " << max_error << std::endl;
}

bool GraphUtility::reduce_1_degree_vertices(Graph *in_g, Graph *out_g) {
    find_components_size(in_g);

    if (out_g->which_components == nullptr) {
        out_g->R = new int[in_g->n + 1];
        out_g->F = new int[in_g->m * 2];
        out_g->C = new int[in_g->m * 2];
        out_g->weight = new int[in_g->n];
        std::fill_n(out_g->weight, in_g->n, 1);
        out_g->bc = new int[in_g->n];
        std::memset(out_g->bc, 0, in_g->n * sizeof(int));
        out_g->components_sizes = in_g->components_sizes;
        out_g->which_components = in_g->which_components;
        out_g->n = in_g->n;
        out_g->m = in_g->m;
    }

    int *R = new int[in_g->n + 1];
    int *F = new int[in_g->m * 2];
    int *C = new int[in_g->m * 2];

    std::memcpy(R, in_g->R, sizeof(int) * (in_g->n + 1));
    std::memcpy(F, in_g->F, sizeof(int) * (in_g->m * 2));
    std::memcpy(C, in_g->C, sizeof(int) * (in_g->m * 2));


    bool finish = true;
    if (in_g->m > 1) {
        for (int i = 0; i < in_g->n; i++) {
            if (R[i + 1] - R[i] == 1) {
                finish = false;
                int v = C[R[i]];

                out_g->bc[i] += (out_g->weight[i] - 1) *
                                (out_g->components_sizes[out_g->which_components[i]] - out_g->weight[i]);
                out_g->bc[v] += (out_g->components_sizes[out_g->which_components[i]] - 1 - out_g->weight[i]) *
                                (out_g->weight[i]);
                out_g->weight[v] += out_g->weight[i];
//            out_g->bc[v] += 2 * (out_g->components_sizes[out_g->which_components[v]] -
//                                out_g->weight[v] - 1);
                //in_g->R[i] = in_g->n;
                out_g->m--;
                C[R[i]] = -1;
                for (int j = R[v]; j < R[v + 1]; ++j) {
                    if (C[j] == i) {
                        C[j] = -1;
                    }
                }
            }
        }
    }
    int r_index = 0;
    //int m = 0;
    for (int i = 0; i < in_g->n; i++) {
        out_g->R[i] = r_index;
        for (int j = R[i]; j < R[i + 1]; j++) {
            if (C[j] != -1) {
                out_g->C[r_index] = C[j];
                out_g->F[r_index++] = i;
            }
        }
    }
    out_g->R[in_g->n] = r_index;

    delete[] R;
    delete[] F;
    delete[] C;
    return finish;
}

void GraphUtility::find_components_size(Graph *g) {
    if (g->which_components != nullptr)
        return;

    g->which_components = new int[g->n];

    std::vector<int> components_sizes(g->n, 0);

    std::vector<bool> vis(g->n, false);


    int total_components = 0;

    for (int i = 0; i < g->n; i++) {
        if (!vis[i]) {
            std::queue<int> Q;
            Q.push(i);
            vis[i] = true;
            components_sizes[total_components] = 1;
            g->which_components[i] = total_components;
            while (!Q.empty()) {
                int v = Q.front();
                Q.pop();
                for (int j = g->R[v]; j < g->R[v + 1]; j++) {
                    int u = g->C[j];
                    if (!vis[u]) {
                        vis[u] = true;
                        Q.push(u);
                        components_sizes[total_components]++;
                        g->which_components[u] = total_components;
                    }
                }
            }
            total_components++;
        }

    }
    g->components_sizes = new int[total_components];
    for (int i = 0; i < total_components; i++) {
        g->components_sizes[i] = components_sizes[i];
    }

#ifdef DEBUG
    std::cout << "\tTotal components: " << total_components;
//    std::cout <<"Component sizes:\n";
//    for(int i = 0 ; i < total_components; i++){
//        std::cout << i+1 << ": " << components_sizes[i] << "\n";
//    }
    std::cout << std::endl;
#endif
}


GraphUtility::~GraphUtility() {
    // TODO Auto-generated destructor stub
}

