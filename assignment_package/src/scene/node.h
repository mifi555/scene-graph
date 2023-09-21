#pragma once
#include <QTreeWidgetItem>
#include <vector>
#include <smartpointerhelp.h>
#include "polygon.h"

// NODE CLASS

//Node class inherits from QTreeWidgetItem
class Node : public QTreeWidgetItem {
    // TODO

private:
    // A set of unique_ptrs to the node's children.
    std::vector<uPtr<Node>> children;
    //A raw, C-style pointer to one instance of Polygon2D
    Polygon2D* polygon;
    //The color with which to draw the Polygon2D pointed to by the node,
    glm::vec3 color;
    //QString to represent a name for the node
    QString name;

public:
    //CONSTRUCTORS

    //constructor
    Node(const QString& nodeName);

    //copy constructor
    Node(const Node& other);

    //virtual destructor
    virtual ~Node();

    //METHODS

    // assignment operator
    Node& operator=(const Node& other);

    //purely virtual function that computes and returns a 3x3 homogeneous matrix representing the transformation in the node.
    virtual glm::mat3 computeTransformationMatrix();

    //A function that adds a given unique_ptr as a child to this node. You'll have to make use of std::move to make this work. Additionally, to make scene graph construction easier for you, this function should return a Node& that refers directly to the Node that is pointed to by the unique_ptr passed into the function. This will allow you to modify that heap-based Node from within your scene graph construction function without worrying about std::move-ing unique pointers around.
    Node& addChild(uPtr<Node> n);

    //A function that allows the user to modify the color stored in this node
    void setColor(const glm::vec3& color);

    //A function that allows the user to modify the geometry stored in the node
    virtual void setGeometry(Polygon2D* geometry);

    //Getter for children
    std::vector<uPtr<Node>>& getChildren();

    //Getter for Polygon
    Polygon2D* getPolygon() const;

    //Getter for Color
    glm::vec3 getColor() const;

};

// DERIVED CLASSES that inherit from Node Base Class.

//TranslateNode, which stores two floating point numbers: one that represents its translation in the X direction, and one that represents its translation in the Y direction.

class TranslateNode : public Node {
private:
    float xTranslation; //translation in x direction
    float yTranslation; //translation in y direction

public:
    //call base class constructor
    TranslateNode(const QString& nodeName, float x, float y) : Node(nodeName), xTranslation(x), yTranslation(y) {}

    //destructor
    ~TranslateNode() override = default;

    //method to compute the transformation matrix
    glm::mat3 computeTransformationMatrix() override;

    //setters
    void setTX(float x);
    void setTY(float y);


};

//RotateNode, which stores one floating point number which represents the magnitude of its rotation in degrees.
class RotateNode : public Node {
private:
    float rotationMagnitude;

public:
    //call base class constructor
    RotateNode(const QString& nodeName, float rotationValue) : Node(nodeName), rotationMagnitude(rotationValue) {}
    //destructor
    ~RotateNode() override = default;

    //method to compute the transformation matrix
    glm::mat3 computeTransformationMatrix() override;

    //setter
    void setRotate(float rotationValue);
};

//ScaleNode, which stores two floating point numbers: one that represents its scale in the X direction, and one that represents its scale in the Y direction.
class ScaleNode : public Node {
private:
    float xScale; //scale in x direction
    float yScale; //scale in y direction

public:
    //call base class constructor
    ScaleNode(const QString& nodeName, float x, float(y)) : Node(nodeName), xScale(x), yScale(y) {}

    //destructor
    ~ScaleNode() override = default;

    //method to compute the transformation matrix
    glm::mat3 computeTransformationMatrix() override;

    //setters
    void setSX(float x);
    void setSY(float y);
};


