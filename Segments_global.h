#ifndef SEGMENTS_GLOBAL_H
#define SEGMENTS_GLOBAL_H
#include <QtCore/qglobal.h>

#if defined(SEGMENTS_LIBRARY)
#  define SEGMENTS_EXPORT Q_DECL_EXPORT
#else
#  define SEGMENTS_EXPORT Q_DECL_IMPORT
#endif
#endif // SEGMENTS_GLOBAL_H
