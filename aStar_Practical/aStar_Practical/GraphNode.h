#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <list>

// Forward references
template <typename NodeType, typename ArcType> class GraphArc;

// -------------------------------------------------------
// Name:        GraphNode
// Description: This is the node class. The node class 
//              contains data, and has a linked list of 
//              arcs.
// -------------------------------------------------------
template<class NodeType, class ArcType>
class GraphNode {
private:    
// typedef the classes to make our lives easier.
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;
// -------------------------------------------------------
// Description: data inside the node
// -------------------------------------------------------
    NodeType m_data;
	Node * m_pNode;//pointer to previous node
// -------------------------------------------------------
// Description: list of arcs that the node has.
// -------------------------------------------------------
    list<Arc> m_arcList;
	

// -------------------------------------------------------
// Description: This remembers if the node is marked.
// -------------------------------------------------------
    bool m_marked;

// -------------------------------------------------------
// Description: This remembers if the cost to travel to nodes
// -------------------------------------------------------
	int m_costDist;
	int m_goalcostDist;
	sf::Vector2f position;

public:
    // Accessor functions
    list<Arc> const & arcList() const 
	{
        return m_arcList;              
    }

    bool marked() const 
	{
        return m_marked;
    }

    NodeType const & data() const 
	{
        return m_data;
    }

    // Manipulator functions
    void setData(NodeType data) 
	{
        m_data = data;
    }
    
    void setMarked(bool mark) 
	{
        m_marked = mark;
    }

	int getCostDist()
	{ 
		return m_costDist; 
	}

	sf::Vector2f getPos()
	{
		return position;
	}

	void setPos(sf::Vector2f value)
	{
		position = value;
	}

	int getEstGoalDist()
	{
		return m_goalcostDist;
	}

    Arc* getArc( Node* pNode );  
	
	void setCostDist(int value);
	void setEstGoalDist(int value);
    void addArc( Node* pNode, ArcType pWeight );
	void setPrevNode(Node* pNode);
	GraphNode<NodeType, ArcType>* getPrevNode();
    void removeArc( Node* pNode );
};

// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    This finds the arc in the current node that
//                  points to the node in the parameter.
//  Arguments:      The node that the arc connects to.
//  Return Value:   A pointer to the arc, or 0 if an arc doesn't
//                  exist from this to the specified input node.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
GraphArc<NodeType, ArcType>* GraphNode<NodeType, ArcType>::getArc( Node* pNode ) {

     list<Arc>::iterator iter = m_arcList.begin();
     list<Arc>::iterator endIter = m_arcList.end();
     Arc* pArc = 0;

     // find the arc that matches the node
	 for (; iter != endIter && pArc == 0; ++iter)
	 {
		 if ((*iter).node() == pNode)
		 {
			 pArc = &((*iter));
		 }
	 }

     // returns null if not found
     return pArc;
}

// ----------------------------------------------------------------
//  Name:           addArc
//  Description:    This adds an arc from the current node pointing
//                  to the first parameter, with the second parameter 
//                  as the weight.
//  Arguments:      First argument is the node to connect the arc to.
//                  Second argument is the weight of the arc.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::addArc( Node* pNode, ArcType weight ) {
   // Create a new arc.
   Arc a;
   a.setNode(pNode);
   a.setWeight(weight);   
   // Add it to the arc list.
   m_arcList.push_back( a );
}


// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This finds an arc from this node to input node 
//                  and removes it.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::removeArc( Node* pNode ) {
	list<Arc>::iterator iter = m_arcList.begin();
	list<Arc>::iterator endIter = m_arcList.end();

	int size = m_arcList.size();

	// find the arc that matches the node
	for (; iter != endIter && m_arcList.size() == size; ++iter) {
		if ((*iter).node() == pNode) {
			m_arcList.remove((*iter));
		}
	}

}

template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::setPrevNode(Node* pNode)
{
	m_pNode = pNode;
}

template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::setCostDist(int value)
{
	m_costDist = value;
}

template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::setEstGoalDist(int value)
{
	m_goalcostDist = value;
}

template<typename NodeType, typename ArcType>
GraphNode<NodeType, ArcType>* GraphNode<NodeType, ArcType>::getPrevNode()
{
	return m_pNode;
}

#include "GraphArc.h"

#endif
