/****************************************************************************
* Fancy Decorator
*
* Copyright (C) 2012 Timo Makimattila
*
* Contact: Timo Makimattila <timo.makimattila@kapsi.fi>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
* USA.
*
****************************************************************************/

#include <qt3d/qglpicknode.h>
#include "widget.h"
#include <qt3d/qgraphicstranslation3d.h>
#include <qt3d/qgraphicsrotation3d.h>

Widget::Widget(QWidget *parent): _scale(0.5), _previousScale(_scale),
    _selectedNode(NULL)
    , QGLView(parent)
{
    setOption(QGLView::ObjectPicking, true);
    setupMainNode("bottom.3ds");
    addNode("table.3ds");
    addNode("tvtable.3ds");
    addNode("couch.3ds");
    _scene->generatePickNodes();

    QList<QGLPickNode *>nodes = _scene->pickNodes();
    foreach (QGLPickNode *node, nodes) {
        registerObject(node->id(), node);
        connect(node, SIGNAL(clicked()),
                this, SLOT(objectPicked()));
    }
    showFullScreen();

}

void Widget::initializeGL(QGLPainter *painter) {
    QGLCamera *camera = new QGLCamera();
    camera->setNearPlane(0.5f);
    camera->setFarPlane(1000.0f);
    camera->setFieldOfView(34.0f);
    setCamera(camera);

    QGLLightParameters *lp = new QGLLightParameters(this);
    lp->setPosition(QVector3D(-1.0, -2.0, 1.0));
    lp->setSpotAngle(180);
    lp->setSpotExponent(24);
    lp->setSpotDirection(QVector3D(1.0, 4.0, 4.0));

    QGLLightModel *lm = new QGLLightModel(this);
    lm->setAmbientSceneColor(QColor(200, 200, 200));

    painter->setLightModel(lm);
    painter->setMainLight(lp);
    painter->setClearColor(Qt::black);
}


void Widget::setupMainNode(char *fileName) {
    _scene = QGLAbstractScene::loadScene(QString(fileName));
    Q_ASSERT(_scene);
    _scene->setPickable(false);
    _mainNode = _scene->mainNode();
    _mainNode->setObjectName(QString(fileName));
    QGraphicsRotation3D *rotY = new QGraphicsRotation3D();
    QGraphicsRotation3D *rotX = new QGraphicsRotation3D();
    rotY->setAxis(QVector3D(0, 1, 0));
    rotY->setAngle(-45.0);
    rotX->setAxis(QVector3D(1, 0, 0));
    rotX->setAngle(20.0);
    _mainNode->addTransform(rotY);
    _mainNode->addTransform(rotX);

}

void Widget::addNode(char *fileName) {
    Q_ASSERT(_mainNode);
    QGLAbstractScene *scene = QGLAbstractScene::loadScene(QString(fileName));
    Q_ASSERT(scene);
    QGLSceneNode *node = scene->mainNode();
    node->setObjectName(QString(fileName));
    scene->setPickable(true);
    _mainNode->addNode(node);
}

void Widget::paintGL(QGLPainter *painter) {
    glEnable (GL_LINE_SMOOTH);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glEnable(GL_DEPTH_TEST);

    painter->projectionMatrix().push();
    painter->modelViewMatrix().push();
    painter->modelViewMatrix().scale(_scale);
    _mainNode->draw(painter);
    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();

    glDisable(GL_DEPTH_TEST);
    glDisable (GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
}


void Widget::wheelEvent(QWheelEvent *event) {
    int numDegrees = event->delta() / 8;
    _previousScale = _scale;
    _scale += (float)numDegrees * 0.01;
    if (_scale < 0) _scale = 0;
    updateGL();
}

void Widget::keyPressEvent(QKeyEvent *event) {
    if (_selectedNode) {
        qreal diff = 0.1;
        qreal x = _selectedNode->x();
        qreal z = _selectedNode->z();
        QGraphicsRotation3D *rot;
        int key = event->key();
        switch(key) {
        case Qt::Key_A:
            x -= diff;
            break;
        case Qt::Key_D:
            x += diff;
            break;
        case Qt::Key_W:
            z -= diff;
            break;
        case Qt::Key_X:
            z += diff;
            break;
        case Qt::Key_S:
            rot = new QGraphicsRotation3D();
            rot->setAxis(QVector3D(0, 1, 0));
            rot->setAngle(90);
            _selectedNode->addTransform(rot);
            break;

        default:
            break;
        }
        _selectedNode->setX(x);
        _selectedNode->setZ(z);
        updateGL();
    }
    QGLView::keyPressEvent(event);

}



void Widget::objectPicked() {
    QGLPickNode *origin = (QGLPickNode*)sender();
    QGLSceneNode *node = origin->target();
    QObject *parent = node->parent();
    while (parent->parent()) {
        node = (QGLSceneNode*)parent;
        parent = node->parent();
    }
    qDebug() << node->objectName() << node->pickNode()->id();
    if (node->pickNode()->id() > 1) {
        _selectedNode = node;
    } else {
        _selectedNode = NULL;
    }

}
