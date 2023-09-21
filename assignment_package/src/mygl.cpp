#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      prog_flat(this),
      m_geomGrid(this), m_geomSquare(this, {glm::vec3(0.5f, 0.5f, 1.f),
                                            glm::vec3(-0.5f, 0.5f, 1.f),
                                            glm::vec3(-0.5f, -0.5f, 1.f),
                                            glm::vec3(0.5f, -0.5f, 1.f)}),
      m_geomTriangle(this, 3),
      m_showGrid(true),
      mp_selectedNode(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();

    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    m_geomGrid.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the scene geometry
    m_geomGrid.create();
    m_geomSquare.create();
    m_geomTriangle.create();

    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    // TODO: Call your scene graph construction function here
    m_rootNode = constructSceneGraph();

    //MyGL emit its signal sig_sendRootNode with the root node of your scene graph as its argument.
    emit sig_sendRootNode(m_rootNode.get());

}

std::unique_ptr<Node> MyGL::constructSceneGraph(){
    // A torso that serves as the "root" of all body transformations.
    uPtr translateTorso = mkU<TranslateNode>("TorsoT", 0.0f, 0.0f);

    //set color and geometry of torso
    translateTorso->setColor(glm::vec3(0,1,0));
    translateTorso->setGeometry(&m_geomSquare);

    //head
    uPtr scaleHead = mkU<ScaleNode>("ScaleHead", 0.75f, 0.75f);
    scaleHead->setColor(glm::vec3(0,0,1));
    scaleHead->setGeometry(&m_geomSquare);

    uPtr translateHead = mkU<TranslateNode>("TranslateHead", 0, 1);  // Example translation

    //1
    //move pivot point
    uPtr translateIntermediateTorsoLeft = mkU<TranslateNode>("TranslatePivotTorsoArmLeft", 0.4f, 0.4f);
    uPtr translateIntermediateTorsoRight = mkU<TranslateNode>("TranslatePivotTorsoArmRight", -0.4f, 0.4f);

    translateHead->addChild(std::move(scaleHead));

    //lower arms
    // CHILD <- PARENT
    // S <- T <- R
    uPtr scaleLowerArmLeft = mkU<ScaleNode>("ScaleLowerArmLeft", 0.8f, 0.2f);
    scaleLowerArmLeft->setGeometry(&m_geomSquare);
    scaleLowerArmLeft->setColor(glm::vec3(0,0,1));

    uPtr scaleLowerArmRight = mkU<ScaleNode>("ScaleLowerArmRight", 0.8f, 0.2f);
    scaleLowerArmRight->setGeometry(&m_geomSquare);
    scaleLowerArmRight->setColor(glm::vec3(0,0,1));

    //3
    uPtr translateLowerArmLeft = mkU<TranslateNode>("TranslateLowerArmLeft", 0.4f, 0.0f);  // Move to left
    uPtr translateLowerArmRight = mkU<TranslateNode>("TranslateLowerArmRight", -0.4f, 0.0f);  // Move to left

    translateLowerArmLeft->addChild(std::move(scaleLowerArmLeft));
    translateLowerArmRight->addChild(std::move(scaleLowerArmRight));

    uPtr rotateLowerArmLeft = mkU<RotateNode>("RotateLowerArmLeft", 0);
    uPtr rotateLowerArmRight = mkU<RotateNode>("RotateLowerArmRight", 0);

    rotateLowerArmLeft->addChild(std::move(translateLowerArmLeft));
    rotateLowerArmRight->addChild(std::move(translateLowerArmRight));

    // intermediate Translate Node between RotateNode of Lower Arms and Upper Arms
    uPtr translateIntUpperLowerLeft = mkU<TranslateNode>("TranslatePivotUpperLowerLeft", 0.8f, 0.0f);  // Move to left
    uPtr translateIntUpperLowerRight = mkU<TranslateNode>("TranslatePivotUpperLowerRight", -0.8f, 0.0f);  // Move to right
    translateIntUpperLowerLeft->addChild(std::move(rotateLowerArmLeft));
    translateIntUpperLowerRight->addChild(std::move(rotateLowerArmRight));

    //upper arms
    // CHILD <- PARENT
    // S <- T <- R
    uPtr scaleUpperArmLeft = mkU<ScaleNode>("ScaleUpperArmLeft", 0.8f, 0.2f);
    scaleUpperArmLeft->setGeometry(&m_geomSquare);
    scaleUpperArmLeft->setColor(glm::vec3(0.5,0,0.5));

    uPtr scaleUpperArmRight = mkU<ScaleNode>("ScaleUpperArmRight", 0.8f, 0.2f);
    scaleUpperArmRight->setGeometry(&m_geomSquare);
    scaleUpperArmRight->setColor(glm::vec3(0.5,0,0.5));

    //2
    uPtr translateUpperArmLeft = mkU<TranslateNode>("TranslateUpperArmLeft", 0.5, 0.0);  // Move to left
    uPtr translateUpperArmRight = mkU<TranslateNode>("TranslateUpperArmRight", -0.5f, 0.0f);  // Move to right
    translateUpperArmLeft->addChild(std::move(scaleUpperArmLeft));
    translateUpperArmRight->addChild(std::move(scaleUpperArmRight));

    uPtr rotateUpperArmLeft = mkU<RotateNode>("RotateUpperArmLeft", -65);  // Initial rotation
    uPtr rotateUpperArmRight = mkU<RotateNode>("RotateUpperArmRight", 65);
    rotateUpperArmLeft->addChild(std::move(translateUpperArmLeft));
    rotateUpperArmRight->addChild(std::move(translateUpperArmRight));

    //add intermediate translate nodes as child to upper arms rotate nodes
    rotateUpperArmLeft->addChild(std::move(translateIntUpperLowerLeft));
    rotateUpperArmRight->addChild(std::move(translateIntUpperLowerRight));

    translateIntermediateTorsoLeft->addChild(std::move(rotateUpperArmLeft));
    translateIntermediateTorsoRight->addChild(std::move(rotateUpperArmRight));

    translateTorso->addChild(std::move(translateIntermediateTorsoLeft));
    translateTorso->addChild(std::move(translateIntermediateTorsoRight));

    //***EXTRA CREDIT***//

    //legs
    uPtr scaleLegLeft = mkU<ScaleNode>("ScaleLegLeft", 0.3f, 1.5f);
    scaleLegLeft->setGeometry(&m_geomSquare);
    scaleLegLeft->setColor(glm::vec3(0.0, 0.0, 1.0));

    uPtr scaleLegRight = mkU<ScaleNode>("ScaleLegRight", 0.3f, 1.5f);
    scaleLegRight->setGeometry(&m_geomSquare);
    scaleLegRight->setColor(glm::vec3(0.0, 0.0, 1.0));

    uPtr translateLegLeft = mkU<TranslateNode>("TranslateLegLeft", 0.25f, -1.25f);  // Move to right
    uPtr translateLegRight = mkU<TranslateNode>("TranslateLegRight", -0.25f, -1.25f);  // Move to left

    translateLegLeft->addChild(std::move(scaleLegLeft));
    translateLegRight->addChild(std::move(scaleLegRight));

    //hat
    uPtr scaleHat = mkU<ScaleNode>("ScaleHatNode", 1.0f, 1.0f);
    scaleHat->setGeometry(&m_geomTriangle);
    scaleHat->setColor(glm::vec3(1.0, 1.0, 0.0));

    uPtr rotateHate = mkU<RotateNode>("RotateHatNode", 90);
    rotateHate->addChild(std::move(scaleHat));

    uPtr translateHat = mkU<TranslateNode>("TranslateHatNode", 0.0f, 0.6f);
    translateHat->addChild(std::move(rotateHate));

    //eyes
    //left eye
    uPtr scaleNodeLeftEye = mkU<ScaleNode>("ScaleEyeLeft", 0.1f, 0.1f);
    scaleNodeLeftEye->setGeometry(&m_geomSquare);
    scaleNodeLeftEye->setColor(glm::vec3(1.0, 1.0, 1.0));
    uPtr translateNodeLeftEye = mkU<TranslateNode>("TranslateEyeLeft", 0.175f, 0.15f);
    translateNodeLeftEye->addChild(std::move(scaleNodeLeftEye));

    //right eye
    uPtr scaleNodeRightEye = mkU<ScaleNode>("ScaleEyeRight", 0.1f, 0.1f);
    scaleNodeRightEye->setGeometry(&m_geomSquare);
    scaleNodeRightEye->setColor(glm::vec3(1.0,1.0,1.0));
    uPtr translateNodeRightEye = mkU<TranslateNode>("TranslateEyeRight", -0.175f, 0.15f);
    translateNodeRightEye->addChild(std::move(scaleNodeRightEye));

    //add eyes to head
    translateHead->addChild(std::move(translateNodeLeftEye));
    translateHead->addChild(std::move(translateNodeRightEye));

    //nose
    uPtr scaleNodeNose = mkU<ScaleNode>("ScaleNose", 0.1f, 0.2f);
    scaleNodeNose->setGeometry(&m_geomSquare);
    scaleNodeNose->setColor(glm::vec3(1.0f,0.71f, 0.75f));
    uPtr translateNodeNose = mkU<TranslateNode>("TranslateNose", 0.0f, 0.09f);
    translateNodeNose->addChild(std::move(scaleNodeNose));

    //add nose to head
    translateHead->addChild(std::move(translateNodeNose));

    //mouth
    uPtr scaleNodeMouth = mkU<ScaleNode>("ScaleMouth", 0.3f, 0.1f);
    scaleNodeMouth->setGeometry(&m_geomSquare);
    scaleNodeMouth->setColor(glm::vec3(1.0f,0.0f, 0.0f));
    uPtr translateNodeMouth = mkU<TranslateNode>("TranslateMouth", 0.0f, -0.15f);
    translateNodeMouth->addChild(std::move(scaleNodeMouth));

    translateHead->addChild(std::move(translateHat));
    //add nose to head
    translateHead->addChild(std::move(translateNodeMouth));

    // Attach the transformation nodes to the torso
    translateTorso->addChild(std::move(translateHead));
    translateTorso->addChild(std::move(translateLegLeft));
    translateTorso->addChild(std::move(translateLegRight));

    // Return the torso, which is the root of the scene graph
    return translateTorso;
}

//SCENE GRAPH TRAVERSAL
//function invoked in MyGL::paintG
void MyGL::sceneGraphTraversal(Node* node, const glm::mat3& transformationMatrix){
    //base case
    if(!node){
        return;
    }

    //combine current transformation with accumulated transformation
    glm::mat3 currentTransformationMatrix = transformationMatrix * node->computeTransformationMatrix();

    //draw polygon

    if(node->getPolygon() != nullptr){
        Polygon2D * polygon = node->getPolygon();
        polygon->setColor(node->getColor());
        prog_flat.setModelMatrix(currentTransformationMatrix);
//        prog_flat.draw(*this, *(node->getPolygon()));
        prog_flat.draw(*this, *(polygon));

    }

    //recursively traverse the node's children
    for(const uPtr<Node>& child : node->getChildren()){
        //child.get(): gets raw pointer
        sceneGraphTraversal(child.get(), currentTransformationMatrix);
    }
}


void MyGL::resizeGL(int w, int h)
{
    glm::mat3 viewMat = glm::scale(glm::mat3(), glm::vec2(0.2, 0.2)); // Screen is -5 to 5

    // Upload the view matrix to our shader (i.e. onto the graphics card)
    prog_flat.setViewMatrix(viewMat);

    printGLErrorLog();
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_showGrid)
    {
        prog_flat.setModelMatrix(glm::mat3());
        prog_flat.draw(*this, m_geomGrid);
    }

    //VVV CLEAR THIS CODE WHEN YOU IMPLEMENT SCENE GRAPH TRAVERSAL VVV///////////////////

    // Shapes will be drawn on top of one another, so the last object
    // drawn will appear in front of everything else

//    prog_flat.setModelMatrix(glm::mat3());
//    m_geomSquare.setColor(glm::vec3(0,1,0));
//    prog_flat.draw(*this, m_geomSquare);

//    m_geomSquare.setColor(glm::vec3(1,0,0));
//    prog_flat.setModelMatrix(glm::translate(glm::mat3(), glm::vec2(-1.f, 0.f)) * glm::rotate(glm::mat3(), glm::radians(-30.f)));
//    prog_flat.draw(*this, m_geomSquare);

//    m_geomSquare.setColor(glm::vec3(0,0,1));
//    prog_flat.setModelMatrix(glm::translate(glm::mat3(), glm::vec2(1.f, 0.f)) * glm::rotate(glm::mat3(), glm::radians(30.f)));
//    prog_flat.draw(*this, m_geomSquare);

    //^^^ CLEAR THIS CODE WHEN YOU IMPLEMENT SCENE GRAPH TRAVERSAL ^^^/////////////////

    // Here is a good spot to call your scene graph traversal function.

    //calling scene graph traversal and starting at the root node with the identity matrix as the transformation matrix
    sceneGraphTraversal(m_rootNode.get(), glm::mat3());

    // Any time you want to draw an instance of geometry, call
    // prog_flat.draw(*this, yourNonPointerGeometry);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    switch(e->key())
    {
    case(Qt::Key_Escape):
        QApplication::quit();
        break;

    case(Qt::Key_G):
        m_showGrid = !m_showGrid;
        break;
    }
}

void MyGL::slot_setSelectedNode(QTreeWidgetItem *i) {
    mp_selectedNode = static_cast<Node*>(i);
}

//SLOT FOR TRANSLATING X
void MyGL::slot_setTX(double x) {
    // TODO update the currently selected Node's
    // X translation value IF AND ONLY IF
    // the currently selected node can be validly
    // dynamic_cast to a TranslateNode.
    // Remember that a failed dynamic_cast
    // will return a null pointer.

    //return if no node is selected
    if(!mp_selectedNode){
        return;
    }
    TranslateNode *tn = dynamic_cast<TranslateNode*>(mp_selectedNode);
    //check if dynamic cast was successful
    if (tn) {
        tn->setTX(static_cast<float>(x));
    }
}

void MyGL::slot_setTY(double Y){
    if(!mp_selectedNode){
        return;
    }
    TranslateNode *tn = dynamic_cast<TranslateNode*>(mp_selectedNode);
    if (tn) {
        tn->setTY(static_cast<float>(Y));
    }
}

//rotate
void MyGL::slot_setRotate(double angle){
    if(!mp_selectedNode){
        return;
    }
    RotateNode *rn = dynamic_cast<RotateNode*>(mp_selectedNode);
    if (rn) {
        rn->setRotate(static_cast<float>(angle));
    }

}

//scale
void MyGL::slot_setScaleX(double sx){
    if(!mp_selectedNode){
        return;
    }
    ScaleNode *sn = dynamic_cast<ScaleNode*>(mp_selectedNode);
    if (sn) {
        sn->setSX(static_cast<float>(sx));
    }
}

void MyGL::slot_setScaleY(double sy){
    if(!mp_selectedNode){
        return;
    }
    ScaleNode *sn = dynamic_cast<ScaleNode*>(mp_selectedNode);
    if (sn) {
        sn->setSY(static_cast<float>(sy));
    }
}

void MyGL::slot_addTranslateNode() {
    // TODO invoke the currently selected Node's
    // addChild function on a newly-instantiated
    // TranslateNode.
    //return if no node is selected
    if(!mp_selectedNode){
        return;
    }
    uPtr newTranslateNode = mkU<TranslateNode>("newTranslateNode", 0.0f, 0.0f);
    mp_selectedNode->addChild(std::move(newTranslateNode));
}

void MyGL::slot_addRotateNode(){
    if(!mp_selectedNode){
        return;
    }
    uPtr newRotateNode = mkU<RotateNode>("newRotateNode", 0.0f);
    mp_selectedNode->addChild(std::move(newRotateNode));
}

void MyGL::slot_addScaleNode(){
    if(!mp_selectedNode){
        return;
    }
    uPtr newScaleNode = mkU<ScaleNode>("newScaleNode", 0.0f, 0.0f);
    mp_selectedNode->addChild(std::move(newScaleNode));
}

void MyGL::enableWidgetsBasedOnSelectedNode(){

}


void MyGL::slot_setPolygon2DPointerToSquare() {
    //check for nullptr
    if (mp_selectedNode) {
        mp_selectedNode->setGeometry(&m_geomSquare);
    }
}

