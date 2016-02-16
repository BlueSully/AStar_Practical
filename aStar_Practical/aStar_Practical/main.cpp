#include "stdafx.h" 

#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"glu32.lib") 

#include "SFML/Graphics.hpp" 
#include "SFML/OpenGL.hpp" 
#include <iostream> 
#define _USE_MATH_DEFINES
#include <math.h>

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "Graph.h"

using namespace std;

typedef GraphNode<string, int> Node;

void visit( Node * pNode ) 
{
	cout << "Visiting: " << pNode->data() << endl;
}

std::pair<sf::CircleShape, sf::Text> SetupCircles(string name, int posx, int posy, int radius, sf::Font* font);
std::pair<sf::VertexArray, sf::Text> SetupEdges(sf::Vector2f from, sf::Vector2f to, int Weight, int radius, sf::Font* font);
std::pair<bool, int> nodeCollision(sf::Vector2i mousePos, vector<std::pair<sf::CircleShape, sf::Text>> nodes);

int main()
{
	// Create the main window 
	sf::RenderWindow window(sf::VideoMode(800, 600, 32), "SFML First Program");

	//load a font
	sf::Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\GARA.TTF");
	
	bool leftClicked = false;//Bools for button presses
	bool rightClicked = false;
	bool resetPressed = false;
	bool keyPressed = false;

	sf::Vector2i position = sf::Mouse::getPosition(window);//mouse position
	bool originFound = false;//if Origin not was selected
	bool goalFound = false;// if goal node was selected
	bool aStarDone = false;//if astar was run

	//create a circle
	vector<std::pair<sf::CircleShape, sf::Text>> nodes;//Holds Circle pos
	Graph<string, int> myGraph(30);//Holds graph data
	int radius = 20;//size of circles
	Node * origin = 0;//Node for Origin
	Node * goal = 0;//Node for Desintation
	vector<std::pair<sf::VertexArray, sf::Text>> arcs;//lines to draw arcs
	vector<Node *> vecpath;//Optimal Path

	string c = "";
	int i = 0;
	ifstream myfile;

	//Loading Nodes
	myfile.open("AstarNodes2.txt");
	int posx, posy;
	while (myfile >> c >> posx >> posy)
	{
		myGraph.addNode(c, sf::Vector2f(posx, posy), i);
		nodes.push_back(SetupCircles(c, (int)myGraph.nodeArray()[i]->getPos().x, (int)myGraph.nodeArray()[i]->getPos().y, radius, &font));//setup Circles sprites
		i++;
	}//intialise Nodes and Circles pos
	myfile.close();

	//loading arcs
	myfile.open("AstarArcs2.txt");
	int from, to, weight;
	while (myfile >> from >> to >> weight)
	{
		myGraph.addArc(from, to, weight);
		//setting up arcs // Slight error that will take into account duplicates
		arcs.push_back(SetupEdges(myGraph.nodeArray()[from]->getPos(), myGraph.nodeArray()[to]->getPos(), weight, radius, &font));
	}
	myfile.close();

	// Now traverse the graph.
	while (window.isOpen())
	{
		// Process events 
		sf::Event Event;
		while (window.pollEvent(Event))
		{
			// Close window : exit 
			if (Event.type == sf::Event::Closed)
				window.close();

			// Escape key : exit 
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && resetPressed == false)//Reset button
		{
			resetPressed = true;
			originFound = false;
			goalFound = false;
			aStarDone = false;
			origin = 0;
			goal = 0;
			myGraph.clearMarks();
			vecpath.clear();

			for (size_t i = 0; i < 30; i++)
			{
				myGraph.nodeArray()[i]->setCostDist(999999);
				myGraph.nodeArray()[i]->setEstGoalDist(0);
				nodes[i].first.setFillColor(sf::Color::White);
			}
		}
		else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R) )
		{
			resetPressed = false;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && keyPressed == false && aStarDone == false)//Start Astar Button
		{
			keyPressed = true;
			if (origin != nullptr && goal != nullptr && goal != origin)
			{
				myGraph.aStar(origin, goal, visit, vecpath);//Do Astar
				aStarDone = true;
			}
			else
			{
				originFound = false;
				goalFound = false;
			}
		}
		else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			keyPressed = false;
		}

		position = sf::Mouse::getPosition(window);

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && leftClicked == false)//Select Origin
		{
			std::pair<bool, int>result = nodeCollision(position, nodes);
			if (result.first && originFound == false)
			{
				if (result.second != -1)
				{
					nodes[result.second].first.setFillColor(sf::Color::Blue);
					originFound = true;
					origin = myGraph.nodeArray()[result.second];
				}
			}
			leftClicked = true;
		}
		else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			leftClicked = false;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && rightClicked == false)//Select Dest
		{
			std::pair<bool, int>result = nodeCollision(position, nodes);
			if (result.first && goalFound == false)
			{
				if (result.second != -1)
				{
					nodes[result.second].first.setFillColor(sf::Color::Red);
					goal = myGraph.nodeArray()[result.second];
					goalFound = true;

				}
			}
			rightClicked = true;
		}
		else if (!sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			rightClicked = false;
		}

		//prepare frame
		window.clear();

		//draw frame items
		for (size_t i = 0; i < arcs.size(); i++)//Draw Arcs
		{
			window.draw(arcs[i].first);
			window.draw(arcs[i].second);
		}

		for (size_t i = 0; i < nodes.size(); i++)//Draw Nodes
		{
			window.draw(nodes[i].first);
			window.draw(nodes[i].second);
		}

		for (size_t i = 0; i < nodes.size(); i++)//Draw Information on nodes and optimial Path
		{
			sf::Text costDist;
			costDist.setPosition(nodes[i].first.getPosition().x + radius / 2 - 2, nodes[i].first.getPosition().y + 10);
			costDist.setCharacterSize(12);
			costDist.setString("G(" + to_string(myGraph.nodeArray()[i]->getCostDist()) + ")");
			if (myGraph.nodeArray()[i]->getCostDist() >= 999999 || myGraph.nodeArray()[i]->getCostDist() <= 0)
			{
				costDist.setString("G(n)");
			}
			
			costDist.setColor(sf::Color::Black);
			costDist.setFont(font);

			sf::Text estcostDist;
			estcostDist.setPosition(nodes[i].first.getPosition().x + radius / 2 - 2, nodes[i].first.getPosition().y + 20);
			estcostDist.setCharacterSize(12);
			estcostDist.setString("H(" + to_string(myGraph.nodeArray()[i]->getEstGoalDist()) + ")");
			if (myGraph.nodeArray()[i]->getEstGoalDist() <= 0)
			{
				estcostDist.setString("H(n)");
			}
			estcostDist.setColor(sf::Color::Black);
			estcostDist.setFont(font);

			if (myGraph.nodeArray()[i]->marked())
			{
				nodes[i].first.setFillColor(sf::Color::Magenta);
			}

			window.draw(costDist);
			window.draw(estcostDist);
		}

		if (!vecpath.empty())
		{
			std::vector<Node *>::reverse_iterator rit = vecpath.rbegin();
			for (; rit != vecpath.rend(); ++rit)
			{
				for (size_t i = 0; i < nodes.size(); i++)
				{
					if ((*rit)->getPos().x == nodes[i].first.getPosition().x - 100 
					 && (*rit)->getPos().y == nodes[i].first.getPosition().y - 100)
					{
						nodes[i].first.setFillColor(sf::Color::Green);
					}
				}
			}
		}
		// Finally, display rendered frame on screen 
		window.display();
	} //loop back for next frame

	return EXIT_SUCCESS;
}

std::pair<bool, int> nodeCollision(sf::Vector2i mousePos, vector<std::pair<sf::CircleShape, sf::Text>> nodes)
{
	for (size_t i = 0; i < nodes.size(); i++)
	{
		int distance = sqrt(((mousePos.x - nodes[i].first.getPosition().x) * (mousePos.x - nodes[i].first.getPosition().x))
			+ ((mousePos.y - nodes[i].first.getPosition().y) * (mousePos.y - nodes[i].first.getPosition().y)));
		if (distance < 30 + nodes[i].first.getRadius())
		{
			
			return make_pair(true, i);
		}
	}

	return make_pair(false, -1);
}

std::pair<sf::CircleShape, sf::Text> SetupCircles(string name, int posx, int posy, int radius, sf::Font* font)
{
	sf::CircleShape node(radius);
	node.setPosition((float)100 + posx, (float)100 + posy);

	sf::Text cirName;
	cirName.setPosition(node.getPosition().x + radius / 2 + 5, node.getPosition().y);
	cirName.setCharacterSize(12);
	cirName.setString(name);
	cirName.setColor(sf::Color::Black);
	cirName.setFont(*font);

	return make_pair(node, cirName);
}

std::pair<sf::VertexArray, sf::Text> SetupEdges(sf::Vector2f from, sf::Vector2f to, int Weight, int radius, sf::Font* font)
{
	sf::VertexArray lines(sf::Lines, 2);
	lines[0].position = sf::Vector2f(100 + from.x + radius, 100 + from.y + radius);
	lines[1].position = sf::Vector2f(100 + to.x + radius, 100 + to.y + radius);

	sf::Text text;
	text.setPosition((lines[0].position + lines[1].position) * 0.5f);
	text.setCharacterSize(12);
	text.setString(to_string(Weight));
	text.setColor(sf::Color::Yellow);
	text.setFont(*font);

	return make_pair(lines, text);
}

/*
“I declare that the work I am submitting for assessing by the Institute examiner(s) is entirely my own
work, save for any portion(s) thereof where the author or source has been duly referenced and
attributed. Signed: Anthony O'Sullivan & C00179413”
*/