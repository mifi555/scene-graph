#pragma once

#include <openglcontext.h>
#include <shaderprogram.h>
#include <scene/grid.h>
#include <scene/polygon.h>
#include <QTreeWidgetItem>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include "scene/node.h"


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)

    Grid m_geomGrid; // The instance of the object used to render the 5x5 grid
    Polygon2D m_geomSquare; // The singular instance of our square object that can be re-drawn with different colors
                         // to create the appearance of there being multiple squares

    //
    Polygon2D m_geomTriangle;
    //

    bool m_showGrid; // Read in paintGL to determine whether or not to draw the grid.

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.

    Node *mp_selectedNode; // A pointer to the Node that was last clicked on in the GUI's Tree Widget

    uPtr<Node> m_rootNode; //root node of the Scene Graph

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    // construct scene graph
    std::unique_ptr<Node> constructSceneGraph();

    //scene graph traversal
    void sceneGraphTraversal(Node* Node, const glm::mat3& transformationMatrix);

protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void sig_sendRootNode(QTreeWidgetItem*);

public slots:
    // Assigns mp_selectedNode to the input pointer.
    // Is connected to a signal from the Tree Widget in the GUI
    // that is emitted every time an element in the widget is clicked.
    void slot_setSelectedNode(QTreeWidgetItem*);

    // TODO: Add slots for altering the currently selected Node's
    // translate / rotate / scale value(s). We have provided
    // an example for updating a Translate Node's X value.

    //translate
    void slot_setTX(double);

    void slot_setTY(double);

    //rotate
    void slot_setRotate(double angle);

    //scale
    void slot_setScaleX(double sx);

    void slot_setScaleY(double sy);

    // TODO: Add slots to add a new Translate / Rotate / Scale Node
    // as a child to the currently selected Node. We have provided
    // an example for adding a Translate Node.
    void slot_addTranslateNode();

    void slot_addRotateNode();

    void slot_addScaleNode();

    // TODO: Add a slot to set the Polygon2D pointer of the currently
    // selected Node to the provided m_geomSquare

    void slot_setPolygon2DPointerToSquare();

    // extra credit: enable widgets dependent on type of node
    void enableWidgetsBasedOnSelectedNode();
};

