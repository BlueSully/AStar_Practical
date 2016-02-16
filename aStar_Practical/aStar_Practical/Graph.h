 #ifndef GRAPH_H
#define GRAPH_H

#include "SFML/Graphics.hpp" 
#include "SFML/OpenGL.hpp" 
#include <list>
#include <queue>

using namespace std;

template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    This is the graph class, it contains all the
//                  nodes.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
class Graph {
private:

    // typedef the classes to make our lives easier.
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;

// ----------------------------------------------------------------
//  Description:    An array of all the nodes in the graph.
// ----------------------------------------------------------------
    Node** m_pNodes;

// ----------------------------------------------------------------
//  Description:    The maximum number of nodes in the graph.
// ----------------------------------------------------------------
    int m_maxNodes;

// ----------------------------------------------------------------
//  Description:    The actual number of nodes in the graph.
// ----------------------------------------------------------------
    int m_count;

public:           
    // Constructor and destructor functions
    Graph( int size );
    ~Graph();

    // Accessors
    Node** nodeArray() const 
	{
       return m_pNodes;
    }

    // Public member functions.
    bool addNode( NodeType data, sf::Vector2f pos, int index );
    void removeNode( int index );
    bool addArc( int from, int to, ArcType weight );
    void removeArc( int from, int to );
    Arc* getArc( int from, int to );        
    void clearMarks();
    void depthFirst( Node* pNode, void (*pProcess)(Node*) );
    void breadthFirst( Node* pNode, void (*pProcess)(Node*) );
	void advbreadthFirst(Node* pNode, Node* goal, void(*pProcess)(Node*));
	void ucs(Node* pStart, Node* pDest, void(*pVisitFunc)(Node*), std::vector<Node *>& path);
	void aStar(Node* pStart, Node* pDest, void(*pProcess)(Node*), std::vector<Node *> & path);

	struct UCSCostCompare{
	public:
		bool operator()(Node * n1, Node * n2){
			return n1->getCostDist() > n2->getCostDist();
		}
	};

	struct AStarCostCompare{
	public:
		bool operator()(Node * n1, Node * n2){
			return (n1->getCostDist() + n1->getEstGoalDist()) > (n2->getCostDist() + n2->getEstGoalDist());
		}
	};
};

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    Constructor, this constructs an empty graph
//  Arguments:      The maximum number of nodes.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::Graph( int size ) : m_maxNodes( size ) {
   int i;
   m_pNodes = new Node * [m_maxNodes];
   // go through every index and clear it to null (0)
   for( i = 0; i < m_maxNodes; i++ ) {
        m_pNodes[i] = 0;
   }

   // set the node count to 0.
   m_count = 0;
}

// ----------------------------------------------------------------
//  Name:           ~Graph
//  Description:    destructor, This deletes every node
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::~Graph() {
   int index;
   for( index = 0; index < m_maxNodes; index++ ) {
        if( m_pNodes[index] != 0 ) {
            delete m_pNodes[index];
        }
   }
   // Delete the actual array
   delete m_pNodes;
}

// ----------------------------------------------------------------
//  Name:           addNode
//  Description:    This adds a node at a given index in the graph.
//  Arguments:      The first parameter is the data to store in the node.
//                  The second parameter is the index to store the node.
//  Return Value:   true if successful
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addNode(NodeType data, sf::Vector2f pos, int index) {
   bool nodeNotPresent = false;
   // find out if a node does not exist at that index.
   if ( m_pNodes[index] == 0) {
      nodeNotPresent = true;
      // create a new node, put the data in it, and unmark it.
      m_pNodes[index] = new Node;
      m_pNodes[index]->setData(data);
	  m_pNodes[index]->setPos(pos);
      m_pNodes[index]->setMarked(false);
      // increase the count and return success.
      m_count++;
    }
        
    return nodeNotPresent;
}

// ----------------------------------------------------------------
//  Name:           removeNode
//  Description:    This removes a node from the graph
//  Arguments:      The index of the node to return.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeNode( int index ) {
     // Only proceed if node does exist.
     if( m_pNodes[index] != 0 ) {
         // now find every arc that points to the node that
         // is being removed and remove it.
         int node;
         Arc* arc;

         // loop through every node
         for( node = 0; node < m_maxNodes; node++ ) {
              // if the node is valid...
              if( m_pNodes[node] != 0 ) {
                  // see if the node has an arc pointing to the current node.
                  arc = m_pNodes[node]->getArc( m_pNodes[index] );
              }
              // if it has an arc pointing to the current node, then
              // remove the arc.
              if( arc != 0 ) {
                  removeArc( node, index );
              }
         }
        

        // now that every arc pointing to the current node has been removed,
        // the node can be deleted.
        delete m_pNodes[index];
        m_pNodes[index] = 0;
        m_count--;
    }
}

// ----------------------------------------------------------------
//  Name:           addArd
//  Description:    Adds an arc from the first index to the 
//                  second index with the specified weight.
//  Arguments:      The first argument is the originating node index
//                  The second argument is the ending node index
//                  The third argument is the weight of the arc
//  Return Value:   true on success.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addArc( int from, int to, ArcType weight ) {
     bool proceed = true; 
     // make sure both nodes exist.
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         proceed = false;
     }
        
     // if an arc already exists we should not proceed
     if( m_pNodes[from]->getArc( m_pNodes[to] ) != 0 ) {
         proceed = false;
     }

     if (proceed == true) {
        // add the arc to the "from" node.
        m_pNodes[from]->addArc( m_pNodes[to], weight );
     }
        
     return proceed;
}

// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This removes the arc from the first index to the second index
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeArc( int from, int to ) {
     // Make sure that the node exists before trying to remove
     // an arc from it.
     bool nodeExists = true;
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         nodeExists = false;
     }

     if (nodeExists == true) {
        // remove the arc.
        m_pNodes[from]->removeArc( m_pNodes[to] );
     }
}

// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    Gets a pointer to an arc from the first index
//                  to the second index.
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   pointer to the arc, or 0 if it doesn't exist.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
// Dev-CPP doesn't like Arc* as the (typedef'd) return type?
GraphArc<NodeType, ArcType>* Graph<NodeType, ArcType>::getArc( int from, int to )
{
     Arc* pArc = 0;
	 list<Arc>::iterator iter = m_arcList.begin();

	 // find the arc that matches the node
	 for (; iter != endIter && pArc == 0; ++iter) 
	 {
		 if ((*iter).node() == pNode) {
			 pArc = &((*iter));
		 }
	 }

	 // returns null if not found
	 return pArc;

}

// ----------------------------------------------------------------
//  Name:           clearMarks
//  Description:    This clears every mark on every node.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::clearMarks() {
     int index;
     for( index = 0; index < m_maxNodes; index++ ) {
          if( m_pNodes[index] != 0 ) {
              m_pNodes[index]->setMarked(false);
          }
     }
}

// ----------------------------------------------------------------
//  Name:           depthFirst
//  Description:    Performs a depth-first traversal on the specified 
//                  node.
//  Arguments:      The first argument is the starting node
//                  The second argument is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::depthFirst( Node* pNode, void (*pProcess)(Node*) ) {
     if( pNode != 0 ) {
           // process the current node and mark it
           pProcess( pNode );
           pNode->setMarked(true);

           // go through each connecting node
           list<Arc>::iterator iter = pNode->arcList().begin();
           list<Arc>::iterator endIter = pNode->arcList().end();
        
		   for( ; iter != endIter; ++iter) {
			    // process the linked node if it isn't already marked.
                if ( (*iter).node()->marked() == false ) {
                   depthFirst( (*iter).node(), pProcess);
                }            
           }
     }
}

// ----------------------------------------------------------------
//  Name:           breadthFirst
//  Description:    Performs a depth-first traversal the starting node
//                  specified as an input parameter.
//  Arguments:      The first parameter is the starting node
//                  The second parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::breadthFirst( Node* pNode, void (*pProcess)(Node*) ) 
{
	if (pNode != 0)
	{
		queue<Node*> nodeQueue;
		// place the first node on the queue, and mark it.
		nodeQueue.push(pNode);
		pNode->setMarked(true);

		// loop through the queue while there are nodes in it.
		while (nodeQueue.size() != 0)
		{
			// process the node at the front of the queue.
			pProcess(nodeQueue.front());

			// add all of the child nodes that have not been 
			// marked into the queue
			list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
			list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();

			for (; iter != endIter; iter++)
			{
				if ((*iter).node()->marked() == false)
				{
					// mark the node and add it to the queue.
					(*iter).node()->setMarked(true);
					nodeQueue.push((*iter).node());
				}
			}

			// dequeue the current node.
			nodeQueue.pop();
		}
	}
}

// ----------------------------------------------------------------
//  Name:           advbreadthFirst
//  Description:    Performs a depth-first traversal the starting node
//                  specified as an input parameter and search through paths to find the goal.
//  Arguments:      The first parameter is the starting node
//                  The second parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::advbreadthFirst(Node* pNode, Node* goal, void(*pProcess)(Node*))
{
	if (pNode != 0)
	{
		bool goalReached = false;

		queue<Node*> nodeQueue;
		// place the first node on the queue, and mark it.
		nodeQueue.push(pNode);
		pNode->setMarked(true);
		// process the node at the front of the queue.
		// loop through the queue while there are nodes in it.
		while (nodeQueue.size() != 0 && goalReached == false)
		{	
			//pProcess(nodeQueue.front());
			list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
			list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();
			
			for (; iter != endIter; iter++)
			{
				if ((*iter).node() == goal)
				{
					goalReached = true;
					goal->setPrevNode(nodeQueue.front());//Shows Path
					break;
				}
				if ((*iter).node()->marked() == false)
				{
					// mark the node and add it to the queue.
					(*iter).node()->setMarked(true);
					(*iter).node()->setPrevNode(nodeQueue.front());
					nodeQueue.push((*iter).node());
				}
			}	
			nodeQueue.pop();
		}

		Graph::Node path = *goal;//printing // but causes memory leaks
		while (path.getPrevNode() != NULL)
		{
			pProcess(&path);
			path = *path.getPrevNode();
		}
		pProcess(&path);
	}
}


template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::ucs(Node* pStart, Node* pDest, void(*pVisitFunc)(Node*), std::vector<Node *>& path)
{
	/*Pesudo Code from Ross Palmer
	Let s = the starting node, g = goal or destination node
	Let pq = a new priority queue
	Initialise d[s] to 0
	For each node v in graph G
	Initialise d[v] to infinity // don’t yet know the distances to these nodes

	Add s to the pq
	Mark(s)
	While the queue is not empty AND pq.top() != g
	For each child node c of pq.top()
	//If(c != previous(pq.top()) // don’t go back to a predecessor
	//Let distC = w(pq.top(), c) + d[pq.top]
	If ( distC < d[c] )
	let d[c] = distC
	Set previous pointer of c to pq.top()
	End if
	If (notMarked(c))
	Add c to the pq
	Mark(c)
	End if
	End if
	End for
	Remove pq.top()
	End while
	*/

	Node * s = pStart;//Let s = the starting node, 
	Node * g = pDest;//Let s = the starting node, 
	priority_queue<Node*, vector<Node *>, UCSCostCompare> pq;//Let pq = a new priority queue
	s->setCostDist(0);//Initialise distanceCost[s] to 0// d == setting distance cost

	for (int i = 0; i < m_count; i++)//For each node v in graph G
	{
		if (m_pNodes[i] != s)
			m_pNodes[i]->setCostDist(numeric_limits<int>::max());//Initialise distanceCost[TotalNodes] to infinity // don’t yet know the distances to these nodes
	}
	pq.push(s);//Add s to the pq
	s->setMarked(true);//Mark(s)
	while (pq.empty() == false && pq.top() != g)//While the queue is not empty AND pq.top() != g
	{
		//For each child node c of pq.top()
		list<Arc>::const_iterator child = pq.top()->arcList().begin();
		list<Arc>::const_iterator endchild = pq.top()->arcList().end();
		for (; child != endchild; child++)//iterate through arcs
		{
			if ((*child).node() != pq.top()->getPrevNode())
			{

				int dist = (*child).weight() + pq.top()->getCostDist(); //Let distC = weight(pq.top(), c) + d[pq.top]
				if (dist < (*child).node()->getCostDist()) //If ( distC < distanceCost[c] )
				{
					(*child).node()->setCostDist(dist);//let distanceCost[c] = distC
					(*child).node()->setEstGoalDist(dist * 0.9f);
					(*child).node()->setPrevNode(pq.top());//Set previous pointer of c to pq.top()
				}
				if (!(*child).node()->marked())//If(notMarked(c))
				{
					pq.push((*child).node());//Add child to the pq
					(*child).node()->setMarked(true);//Mark(s)
				}
			}
		}
		pq.pop();//Remove pq.top()
	}

	Node * node = g;
	while (node != nullptr)//populating vector with the best path
	{
		path.push_back(node);
		node = node->getPrevNode();//gets next previous node
	}
	
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::aStar(Node* pStart, Node* pDest, void(*pProcess)(Node*), std::vector<Node *> & path)
{
	/*
	Let s = the starting node, g = goal node
	Let pq = a new priority queue
	Initialise g[s] to 0
	For each node v in graph G
		Calculate h[v] // Compute estimated distance to goal for each node.
		Initialise g[v] to infinity // Don’t yet know the distances to these nodes 

	Add s to the pq
	Mark(s)
	While the queue is not empty AND pq.top() != g
		For each child node c of pq.top()
			If(c != previous(pq.top() AND c has not been removed from the pq)
				Let distC = h(c) + g(c) // g(c) is actual path cost to child
				If(distC < f(c))
					let f[c] = distC
					Set previous pointer of c to pq.top()
				End if
				If(notMarked(c))
					Add c to the pq
					Mark(c)
				End if
			End if
		End for
		Remove pq.top()
	End while*/

	Node * s = pStart;//Let s = the starting node, 
	Node * g = pDest;//Let g = goal node
	priority_queue<Node*, vector<Node *>, AStarCostCompare> pq;//Let pq = a new priority queue
	vector<Node *> newNodes;
	s->setCostDist(0);//Initialise distanceCost[s] to 0// d == setting distance cost

	for (int i = 0; i < m_count; i++)//For each node v in graph G
	{
		if (m_pNodes[i] != s)
		{
			float x2 = (g->getPos().x - m_pNodes[i]->getPos().x) * (g->getPos().x - m_pNodes[i]->getPos().x);//geting x pos between targets
			float y2 = (g->getPos().y - m_pNodes[i]->getPos().y) * (g->getPos().y - m_pNodes[i]->getPos().y);//geting y pos between targets
			
			m_pNodes[i]->setPrevNode(nullptr);
			m_pNodes[i]->setMarked(false);
			m_pNodes[i]->setEstGoalDist(sqrt(x2 + y2));//calculating y
			m_pNodes[i]->setCostDist(9999999);//Initialise distanceCost[TotalNodes] to infinity // don’t yet know the distances to these nodes
		}
	}
	pq.push(s);//Add s to the pq
	s->setMarked(true);//Mark(s)
	while (pq.empty() == false && pq.top() != g)//While the queue is not empty AND pq.top() != g
	{
		Node * currNode = pq.top();
		pq.pop();

		pProcess(currNode);
		//For each child node c of pq.top()
		list<Arc>::const_iterator child = currNode->arcList().begin();
		list<Arc>::const_iterator endchild = currNode->arcList().end();
		for (; child != endchild; child++)//iterate through arcs
		{
			if ((*child).node() != currNode->getPrevNode())
			{
				//Let distC = h(c) + g(c) // g(c) is actual path cost to child
				int dist = (*child).weight() + currNode->getCostDist() + (*child).node()->getEstGoalDist();
				if (dist < ((*child).node()->getCostDist() + (*child).node()->getEstGoalDist())) //If ( distC < distanceCost[c] )
				{
					(*child).node()->setCostDist((*child).weight() + currNode->getCostDist());//let distanceCost[c] = distC
					(*child).node()->setPrevNode(currNode);//Set previous pointer of c to pq.top()

					if (!pq.empty() && find(const_cast<Node**>(&pq.top()),//Fix for Invalid Heap Memory on Certain Positions
					const_cast<Node**>(&pq.top()) + pq.size(), (*child).node()) != (const_cast<Node**>(&pq.top()) + pq.size()))
					{
						std::make_heap(const_cast<Node**>(&pq.top()),
							const_cast<Node**>(&pq.top()) + pq.size(),
							AStarCostCompare());//Rearranges the elements in the range [first,last) in such a way that they form a vaild heap.
					}
				}
				if (!(*child).node()->marked())//If(notMarked(c))
				{
					newNodes.push_back((*child).node());
					//pq.push((*child).node());//Add child to the pq
					(*child).node()->setMarked(true);//Mark(s)
				}
				
			}
		}
		for (int i = 0; i < newNodes.size(); i++)
		{
			pq.push(newNodes[i]);
		}
		newNodes.clear();

	}

	Node * node = g;
	while (node != nullptr)//populating vector with the best path
	{
		path.push_back(node);
		node = node->getPrevNode();//gets next previous node
	}
}

#include "GraphNode.h"
#include "GraphArc.h"


#endif
