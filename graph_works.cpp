/*=============================================================================
 * Program: Graph Works
 * Author: Ben Pogrund
 *
 * Date: 4-14-2016
 * Description: This program can calculate the minimum spanning tree of a given
 *				graph, or generate all possible graphs up to a given number of
 * 				vertices.  
 *
 *				Minimum spanning tree is implemented with Prim's algorithm.
 *
 * Input: Edge matrix for graph G
 * Output: Edge matrix for graph T (Minimum weight spanning tree)
 * 
 * Compilation instructions: g++ -o graph_works.exe graph_works.cpp
 * Usage: ./graph_works.exe
 * 
 *==========================================================================*/

/**============================================================================
 ** Implementation options and time complexity:
 **      adjacency matrix, searching	    O(|V|^2)
 **      binary heap and adjacency list	    O((|V| + |E|) log |V|) 
 **                                       = O(|E| log |V|)
 **      Fibonacci heap and adjacency list	O(|E| + |V| log |V|)
 **==========================================================================*/

#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

using namespace std;

/*========================= global type definitions =========================*/

class WeightedEdge
{
  private:  
    int u;  // First unordered vertex
    int v;  // Second unordered vertex
    int w;  // Weight of edge between vertices
    
  public:
    // Constructor (u v w)
	WeightedEdge(int setU, int setV, int setW) { u=setU; v=setV; w=setW; };
	
	// Print function for debug
	void print_edge() const { cout << "<" << u << ", " << v << "> " 
                                   << "weight[ " << w << " ]";};
	
	// Accessors
	int getU() const { return u; };
	int getV() const { return v; };
	int getW() const { return w; };
};

// Matrix info
struct MatrixInfo
{
    int vertexCount;
    int maxEdgeCount;
};

/*=========================== function prototypes ===========================*/

int launch_menu();

bool valid_choice(char c);

void spanning_tree();

void graph_generation();

bool check_file ( ifstream &inputFile );

void print_graph( vector< WeightedEdge > G );

void create_graph( ifstream &inputFile, vector< WeightedEdge > &G, 
                   unsigned int &vertexCount );

int min_incident( vector< WeightedEdge > &G, vector< int > &vT );

void make_graphs( const int vertex_count, const int combination_count );

void write_graph( const int indices[], const int VERTEX );

int triangle_number( int k );

// Debug functions
void make_combinations( const int input_count, const int combination_count );
 
void print_permu( const int indices[], const int count );

/*=============================================================================
 * Function: Main
 * Description: High level organizer
 * Parameters: None
=============================================================================*/
int main()
{
	switch (launch_menu())
	{
		case 1: spanning_tree(); 	break;
		case 2: graph_generation(); break;
		/** room for more features... **/
	}
	
	return 0;
}

/*=============================================================================
Function: launch_menu
Description: Provides an interface for different graph operations
=============================================================================*/
int launch_menu()
{
	char c;		// Stores menu selection from user
	
	printf("\n");
	printf("--------------------------------------------\n");
	printf(" Graph Works"); 
	printf("                  version 0.2.0 \n");
	printf("--------------------------------------------\n");
	printf("\n");
	
	do
	{
		printf(" 1: Spanning Tree\n");
		printf(" 2: Graph Generation\n");
		printf(" > ");
		cin >> c;
	} while ( !valid_choice(c) );
	
	printf("\n");
	
	return c-48; // ascii to integer
}

/*=============================================================================
Function: launch_menu
Description: Provides an interface for different graph operations
=============================================================================*/
bool valid_choice(char c)
{
	switch (c)
	{
		case '1':	// Spanning Tree
		case '2':	// Graph Generation
			return true;
		default:
			return false;
	}
}

/*=============================================================================
Function: spanning_tree
Description: Calculates a spanning a tree from an input file.
=============================================================================*/
void spanning_tree()
{
	unsigned int vertexCount = 0;   // Stores vertex count from input file
    int totalWeight = 0;            // Tracks weight of T
    ifstream inputFile;             // Stores input file data to read from
    vector< WeightedEdge > G;       // Our graph
    vector< WeightedEdge > T;       // Our tree
    
    /** Read in from input file **/
    if ( !check_file( inputFile ) ) return;
    
    /** Read edge information **/
    create_graph( inputFile, G, vertexCount );
    cout << endl;
    
    // Print Display Info
    cout << "Weighted edges will be shown as follows," << endl
         << "   index: ( unordered vertices ) [ weight ]" << endl << endl;
         
    /** Print G **/
    cout << "For the given graph, G:" << endl;
    print_graph(G);
    
    // Close input file
    inputFile.close();
    
    /** Initialize saturated verticy group for T **/
    vector< int > vT;            // Stores vertices of the graph T
    vT.push_back( G[0].getU() ); // We can start anywhere, why not here
    
    /** Traverse G with Prim's algorithm to find T **/
    // Until edge cardinality of T is one less than vertex cardinality of G
    do
    {
        int min_incident_index;     // Stores index of edge to add to T
        
        // Find minimum incident edge to saturated verteces thus far
        min_incident_index = min_incident( G, vT );
        
        // Add to tree, T
        T.push_back( G[ min_incident_index ] );
        totalWeight += G[ min_incident_index ].getW();
        
    } while ( T.size() < vertexCount-1 );
    
    /** Print T **/
    cout << "The spanning tree T of G:" 
         << endl;
    print_graph(T);
    
    // Print weight
    cout << "Total weight of T: " << endl 
         << "   " << totalWeight << endl << endl;
    
}

/*=============================================================================
Function: graph_generation
Description: Generates all graphs up to n vertices
=============================================================================*/
void graph_generation()
{
	int n;				// Maximum graph vertices
	ofstream outfile;	// Stores output file data for graphs
    
	do
	{
		printf(" Generate all graphs up to how many vertices?\n");
		printf(" > ");
		cin >> n;
	} while ( !(n > 2) );
	
    // Clear output file
    outfile.open( "generated_graphs.txt" );
    outfile.close();
    
    /** Make all graphs up to n vertices **/
    // Iterate through each vertex count
    for ( int v = 2; v <= n; v++)
    {
        int edges = triangle_number(v-1);
        // Iterate through each edge permutation count
        for (int e = 1; e <= edges; e++)
        {
            make_graphs( v, e );
        }
    }
    
}

/*=============================================================================
Function: check_file
Description: Opens and checks files, displaying message and exit upon error
Parameters: inputFile - file storing weighted adjacency matrix
=============================================================================*/
bool check_file ( ifstream &inputFile )
{
    // Open adjacency matrix file
    inputFile.open( "input.txt" );
    
    if (!inputFile)
    {
        // Absent file
        cout << "input.txt is absent from the exe directory." 
             << endl << endl << "Program terminated." << endl << endl;
            
        // Close file
        inputFile.close();
        return false;
    }
    return true;
}

/*=============================================================================
Function: print_graph
Description: Shows the weighted edges of the graph
Parameters: G - vector set of weighted edges
=============================================================================*/
void print_graph( vector< WeightedEdge > G )
{
    // Print each weighted edge
    for ( unsigned int i = 0; i < G.size(); i++ )
    {
        cout << "   Edge " << i << ": ";
        G[i].print_edge();
        cout << endl;
    }
    cout << endl;
}

/*=============================================================================
Function: create_graph
Description: Reads data from input file and stores as UVW vectors
Parameters: inputFile - file with weighted edge data
            G - weighted edges stored as UVW vector set
            vertexCount - verticy cardinality for G
=============================================================================*/
void create_graph( ifstream &inputFile, vector< WeightedEdge > &G, 
                   unsigned int &vertexCount )
{
    // Read vertex count
    inputFile >> vertexCount;
    
    // Iterate vertically
    for (unsigned int j = 0; j < vertexCount; j++)
    {
        // Iterate horizontally
        for (unsigned int i = 0; i < vertexCount; i++)
        {
            int k;
            inputFile >> k;
            
            // Only store upper triangular values
            if (i >= j && k!= 0)
            {
                // Store as UVW vectors
                WeightedEdge wEdge = WeightedEdge(i,j,k);
                G.push_back(wEdge);
            }
            
        }
        
    }
    
}

/*=============================================================================
Function: min_incident
Description: Returns the index in G for the vector with minimum weight incident
             with the vertices in the set vT
Parameters: G - weighted edges stored as UVW vector set
            vT - verices of tree being built by Prim's algorithm
=============================================================================*/
int min_incident( vector< WeightedEdge > &G, vector< int > &vT )
{
    int minIndex = 0;       // Stores the index of the edge to add from G to T
    int minWeight;          // Used to track minimum weight
    bool result_uMatch = 0; // True if u from our new edge already exists in T
                            // False if v already exists in T
    
    // Initialize minweight at maximum weight
    minWeight = numeric_limits<int>::max();
    
    // Search for incident edges outside the tree thus far
    for ( unsigned int i = 0; i < G.size(); i++ )
    {
        bool uMatch = 0; // True if the u vertex already exists in T 
        bool vMatch = 0; // True if the v vertex already exists in T 
        
        // Check if the u and v of our edge already exists in T
        for ( unsigned int j = 0; j < vT.size(); j++ )
        {
            if ( G[i].getU() == vT[j] ) uMatch = true;
            if ( G[i].getV() == vT[j] ) vMatch = true;
        }
        
        // Stores the minimum weight found for edges exterior to T so far
        if ( uMatch+vMatch == 1 && G[i].getW() < minWeight)
        {
            minWeight = G[i].getW();
            minIndex = i;
            result_uMatch = uMatch;
        }
    }
    
    /** Append new vertex to vT from our minimum weight edge **/
    // If u already exists in T, then add v, otherwise add u
    vT.push_back( ( result_uMatch ) ? G[minIndex].getV() : G[minIndex].getU() );
    
    return minIndex;
}


/*=============================================================================
Function: make_combinations
Description: calculates reverse colexicographical permutations of a list of
             number objects
Parameters: input_count - defines the list of number objects by length
            combination_count - length of permutations
=============================================================================*/
void make_combinations( const int input_count, const int combination_count )
{
    int k=0;                            // Target object
    int indices[combination_count];     // integers to uhh permutate
   
    // Check that we have enough to populate each array
    if (input_count <= combination_count)
    {
        cout << "Error: combination count higher than or equal to input count."
             << endl;
        return;
    }
   
    // Initialize indices for reverse colexicographical order
    for ( int i = 0; i < combination_count; i++ )
    {
        indices[combination_count-1-i] = input_count-i-1;
    }
   
    /** Iterate through permutations **/
    // Print initial permutation
    print_permu(indices, combination_count);
    do
    {
        // Bring k back to its "home" position
        while ( indices[k] != k )
        {
            indices[k]--;
            print_permu(indices, combination_count);
        };
       
        if (combination_count == 1) break;
        
        /// For debug,
        //cout << endl;
   
        // find the next k that is not "home"
        do
        {
            k++;
        } while (indices[k] == k && k != combination_count);
       
        // Move the k back
        indices[k]--;
       
        // Check if the k is not home yet
        if (indices[k] != k)
        {
            // Bring up all the children
            for ( int i = 0; i<k; i++)
            {
                indices[i] = indices[k]-(k-i);
            }
            // Begin at first child
            k=0;
        }
       
        // Print permutation
        print_permu(indices, combination_count);
       
    } while ( (k != combination_count-1 || indices[k] != k) );
   
    cout << endl;
}
 
 
/*=============================================================================
Function: make_graphs
Description: IT MAKES GRAPHS
Parameters: vertex_count - cardinality of vertices for the generated graphs
            EDGE_COUNT - the number of edges that will exist
=============================================================================*/
void make_graphs( const int VERTEX_COUNT, const int EDGE_COUNT )
{
    int maxEdgeCount = triangle_number(VERTEX_COUNT-1); // Max Edge cardinality
    int ordinals[EDGE_COUNT];    // ordinals of edges that exist in the graph
    int k = 0;                   // Edge Ordinal Number
    
    // Check that we have enough to populate each array
    if (maxEdgeCount < EDGE_COUNT) 
    {
        cout << "Error: combination count higher than or equal to input count."
             << endl;
        return;
    }
    
    // Initialize ordinals for reverse colexicographical order
    for ( int i = 0; i < EDGE_COUNT; i++ )
    {
        ordinals[EDGE_COUNT-1-i] = maxEdgeCount-i-1;
    }
    
    /** Iterate through permutations **/
    // Write initial permutation
    write_graph( ordinals, VERTEX_COUNT );
    
    // Special case for choose all
    if (maxEdgeCount == EDGE_COUNT) return;
    
    // Permutation loops
    do
    {
        // Bring k back to its "home" position
        while ( ordinals[k] > k )
        {
            ordinals[k]--;
            write_graph( ordinals, VERTEX_COUNT );
        };
        
        // Special case for single permuations
        if (EDGE_COUNT == 1) break;
        
        // find the next k that is not "home"
        do
        {
            k++;
        } while (ordinals[k] == k && k != EDGE_COUNT );
        
        // Move the k back
        ordinals[k]--;
        
        // Check if the k is not home yet
        if (ordinals[k] != k)
        {
            // Bring up all the children
            for ( int i = 0; i<k; i++)
            {
                ordinals[i] = ordinals[k]-(k-i);
            }
            // Begin at first child
            k = 0;
        }
        
        // Write permutation
        write_graph( ordinals, VERTEX_COUNT );
        
    } while ( (k != EDGE_COUNT-1 || ordinals[k] != k) );
    
    // Notify completion of permuation
    cout << EDGE_COUNT << " edge permutations for " 
         << VERTEX_COUNT << " vertices complete." << endl;
}

/*=============================================================================
Function: print_permu
Description: prints a permutations
Parameters: indices - an array of objects in the permutation
            count - length of permutation
=============================================================================*/
void print_permu( const int indices[], const int count )
{
    if (!count) return;
    
    for (int i = 0; i < count; i++)
    {
        cout << indices[i] << " ";
    }
    cout << endl;
}
    
/*=============================================================================
Function: write_graph
Description: prints a permutations
Parameters: indices - an array of objects in the permutation
            count - length of permutation
            VERTEX - number of vertices
=============================================================================*/
void write_graph( const int indices[], const int VERTEX )
{
    ofstream outfile;   // Outfile
    
    const int EDGES = triangle_number(VERTEX-1);
    int adjMatrix[VERTEX][VERTEX];
    bool edgeSet[EDGES];
    
    // Initialize adjacencies (in list form)
    // 1 = exist, 0 = non existent
    for (int i = 0, k = 0; i < EDGES; i++)
    {
        edgeSet[i] = (i == indices[k]);
        if (i == indices[k]) k++;
    }
    
    // Initialize Matrix
    for ( int k = 0; k < VERTEX; k++)
        for ( int i = 0; i < VERTEX; i++)
            adjMatrix[i][k] = 0;
    
    // Open adjacency matrix file
    outfile.open( "generated_graphs.txt", ofstream::app );
    
    // Write edges to matrix upper triangle
    outfile << VERTEX << endl;
    int j = 0;
    for ( int k = 0; k < VERTEX; k++)
    {
        // Write edges to file
        for ( int i = 0; i < VERTEX; i++)
        {
            // Establish matrix
            if (i >= k+1)
            {
                // Diagonal Symmetry
                adjMatrix[i][k] = edgeSet[j];
                adjMatrix[k][i] = edgeSet[j];
                j++;
            }
            
            // Write to file
            outfile << adjMatrix[i][k];
        }
        outfile << endl;
    }
    outfile << endl;
    
    // Close file
    outfile.close();
    
}
    
    
/*=============================================================================
Function: triangle_number
Description: returns the triangle number of the given integer
Parameters: k - the given integer to find the triangle number of
=============================================================================*/
int triangle_number( int k )
{
    return (k * (k+1) /2);
}
    
    
    
    
    
    
    
    
    
    
    
    
    






