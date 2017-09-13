/****************************************************************************
**
** Some parts of the code are based on the implementation of Mapbox GL Native
** QtLocation plugin at
** https://github.com/qt/qtlocation/tree/5.9/src/plugins/geoservices/mapboxgl
**
** The original code license is below
**
** Copyright (C) 2017 The Qt Company Ltd.
** Copyright (C) 2017 Mapbox, Inc.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickitemmapboxgl.h"
#include "qsgmapboxglnode.h"

#include <QDebug>

QQuickItemMapboxGL::QQuickItemMapboxGL(QQuickItem *parent):
  QQuickItem(parent)
{
  setFlag(ItemHasContents);

  m_timer.setInterval(250);
  connect(&m_timer, &QTimer::timeout, this, &QQuickItemMapboxGL::update);
  connect(this, SIGNAL(startRefreshTimer()), &m_timer, SLOT(start()));
  connect(this, &QQuickItemMapboxGL::stopRefreshTimer, &m_timer, &QTimer::stop);
}

QQuickItemMapboxGL::~QQuickItemMapboxGL()
{
}

QSGNode* QQuickItemMapboxGL::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
  QSGMapboxGLTextureNode *n = static_cast<QSGMapboxGLTextureNode *>(node);
  QSize sz(width(), height());

  if (!n)
    {
      QMapboxGLSettings settings;
      settings.setAccessToken(qgetenv("MAPBOX_ACCESS_TOKEN"));
      settings.setCacheDatabasePath("/tmp/mbgl-cache.db");
      settings.setCacheDatabaseMaximumSize(20 * 1024 * 1024);
      settings.setViewportMode(QMapboxGLSettings::DefaultViewport);

      n = new QSGMapboxGLTextureNode(settings, sz, window()->devicePixelRatio(), this);
    }

  if (sz != m_last_size)
    {
      n->resize(sz, window()->devicePixelRatio());
      m_last_size = sz;
    }

  bool loaded = n->render(window());
  if (!loaded && !m_timer.isActive())
    {
      emit startRefreshTimer();
    }
  else if (loaded && m_timer.isActive())
    {
      emit stopRefreshTimer();
    }

  return n;
}

