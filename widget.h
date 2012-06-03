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

#ifndef WIDGET_H
#define WIDGET_H
#include <Qt3D/qglview.h>
#include <Qt3D/qglabstractscene.h>
#include <QWheelEvent>

class Widget : public QGLView
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
    void setupMainNode(char *fileName);
    void addNode(char *fileName);

protected:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:

public slots:
    void objectPicked();

private:
    QGLAbstractScene *_scene;
    QGLSceneNode *_mainNode;
    QGLSceneNode *_selectedNode;
    float _scale;
    float _previousScale;

};

#endif // WIDGET_H
