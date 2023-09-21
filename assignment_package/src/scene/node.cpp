#include "node.h"

//constructor implementation:

Node::Node(const QString& nodeName) : polygon(nullptr), color(0.0f, 0.0f, 0.0f), name(nodeName) {
    //TreeWidget modification
    this->setText(0, name);
}

// copy constructr
// needs to make a deep copy
// for each chld determine what node it is
// dynamicast<Node*>
// if dynamic cast fails, the pointer will point to 0
// covered in lecture!!!
Node::Node(const Node& other)
    :
    polygon(other.polygon),
    color(other.color),
    name(other.name){

    for (const auto& child : other.children) {
        // Using dynamic_cast to ascertain the type of each child
        if (auto translateNode = dynamic_cast<TranslateNode*>(child.get())) {
            children.push_back(std::make_unique<TranslateNode>(*translateNode));
        } else if (auto rotateNode = dynamic_cast<RotateNode*>(child.get())) {
            children.push_back(std::make_unique<RotateNode>(*rotateNode));
        } else if (auto scaleNode = dynamic_cast<ScaleNode*>(child.get())) {
            children.push_back(std::make_unique<ScaleNode>(*scaleNode));
        } else {
            // If it's a base Node type or some other unknown type:
            children.push_back(std::make_unique<Node>(*child));
        }
    }
}

Node& Node::operator=(const Node& other) {
    if (this != &other) {
        color = other.color;
        name = other.name;
        polygon = other.polygon;

        children.clear();

        //    for (uPtr<Node> &n : children){
        //        TranslateNode *tn = dynamic_cast<Node*>(n);
        //    }

        for (const auto& child : other.children) {
            if (auto translateNode = dynamic_cast<TranslateNode*>(child.get())) {
                children.push_back(std::make_unique<TranslateNode>(*translateNode));
            } else if (auto rotateNode = dynamic_cast<RotateNode*>(child.get())) {
                children.push_back(std::make_unique<RotateNode>(*rotateNode));
            } else if (auto scaleNode = dynamic_cast<ScaleNode*>(child.get())) {
                children.push_back(std::make_unique<ScaleNode>(*scaleNode));
            } else {
                children.push_back(std::make_unique<Node>(*child));
            }
        }
        // update name in TreeWidget
        this->setText(0, name);
    }
    return *this;
}

Node::~Node() = default;

// default implementation
glm::mat3 Node::computeTransformationMatrix() {
    return glm::mat3(1.0f); // identity matrix
}
Node& Node::addChild(uPtr<Node> n) {
    Node& ref = *n;
    this->children.push_back(std::move(n));
    //update Tree Widget
    this->QTreeWidgetItem::addChild(&ref);
    return ref;
}

void Node::setColor(const glm::vec3& color){
    this->color = color;
}

void Node::setGeometry(Polygon2D* geometry) {
    polygon = geometry;
}

std::vector<uPtr<Node>>& Node::getChildren() {
    return children;
}

Polygon2D* Node::getPolygon() const {
        return polygon;
}

glm::vec3 Node::getColor() const {
        return color;
}

//A purely virtual function that computes and returns a 3x3 homogeneous matrix representing the transformation in the node.

//translation matrix [[1 0 0], [0 1 0], [tx ty 1]]
glm::mat3 TranslateNode::computeTransformationMatrix() {
    glm::mat3 translationMatrix = glm::mat3(1);
    translationMatrix[2][0] = xTranslation;
    translationMatrix[2][1] = yTranslation;

    return glm::translate(glm::mat3(), glm::vec2(xTranslation, yTranslation));;
}

void TranslateNode::setTX(float x){
    xTranslation = x;
}
void TranslateNode::setTY(float y){
    yTranslation = y;
}

glm::mat3 RotateNode::computeTransformationMatrix() {
    float radians = glm::radians(rotationMagnitude);
//    glm::mat3 rotationMatrix = glm::mat3(
//        glm::vec3(cos(radians), -sin(radians), 0),
//        glm::vec3(sin(radians), cos(radians), 0),
//        glm::vec3(0, 0, 1));

    return glm::rotate(glm::mat3(), radians);
}

void RotateNode::setRotate(float rotationValue){
    rotationMagnitude = rotationValue;
}


glm::mat3 ScaleNode::computeTransformationMatrix() {
    glm::mat3 scalingMatrix = glm::mat3(1.0f);
    scalingMatrix[0][0] = xScale;
    scalingMatrix[1][1] = yScale;

    return glm::scale(glm::mat3(), glm::vec2(xScale, yScale));
}

void ScaleNode::setSX(float x){
    xScale = x;

}
void ScaleNode::setSY(float y){
    yScale = y;
}
